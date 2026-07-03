using System;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Media;
using Avalonia.Platform;
using Avalonia.Rendering.Composition;
using Avalonia.Threading;
using Microsoft.Extensions.DependencyInjection;
using Tenaille.Models;
using Tenaille.Services;

namespace Tenaille.Controls;

public class SurfaceControl : Control, IDisposable
{
    private ILogService? _log;
    
    private void Log(string msg)
    {
        var line = "[BastionSurface] " + msg;
        _log?.Log(msg, LogLevel.Info);
    }
    
    private Compositor? _compositor;
    private CompositionDrawingSurface? _drawingSurface;
    private CompositionSurfaceVisual? _surfaceVisual;
    private ICompositionGpuInterop? _gpuInterop;
    
    private ICompositionImportedGpuImage? _importedImage;
    private ICompositionImportedGpuSemaphore? _importedSignalSemaphore;
    private ICompositionImportedGpuSemaphore? _importedWaitSemaphore;

    private DispatcherTimer? _timer;
    private float _anim;
    private bool _frameInFlight;
    private bool _gpuInteropReady;
    private bool _nativeReady;
    private PixelSize _nativeSize;
    private bool _resizePending;
    private PixelSize _pendingSize;
    private bool _disposed;
    
    public override void Render(DrawingContext context)
    {
        context.DrawRectangle(Brushes.Transparent, null, new Rect(Bounds.Size));
        base.Render(context);
    }
    
    protected override void OnAttachedToVisualTree(VisualTreeAttachmentEventArgs e)
    {
        base.OnAttachedToVisualTree(e);
        _log = ((App)Application.Current!).Services!.GetRequiredService<ILogService>();
        _ = InitAsync();
    }

    protected override void OnDetachedFromVisualTree(VisualTreeAttachmentEventArgs e)
    {
        base.OnDetachedFromVisualTree(e);
        StopTimer();
        DestroyNative();
        DestroyComposition();
    }

    protected override void OnPointerMoved(PointerEventArgs e)
    {
        base.OnPointerMoved(e);
        Point position = e.GetPosition(this);
        
        Log($"x: {position.X}, y: {position.Y}");
    }
    
    protected override void OnSizeChanged(SizeChangedEventArgs e)
    {
        base.OnSizeChanged(e);
        if (!_gpuInteropReady) return;
        double scaling = TopLevel.GetTopLevel(this)?.RenderScaling ?? 1.0;
        var px = PixelSize.FromSize(e.NewSize, scaling);
        if (px.Width <= 0 || px.Height <= 0)
        {
            return;
        }
        _pendingSize   = px;
        _resizePending = true;
    }

    private async Task InitAsync()
    {
        _compositor = ElementComposition.GetElementVisual(this)?.Compositor;
        if (_compositor == null)
        {
            Log("No Compositor");
            return;
        }

        _gpuInterop = await _compositor.TryGetCompositionGpuInterop();
        if (_gpuInterop == null)
        {
            Log("ICompositionGpuInterop not available");
            return;
        }

        string imageType = RuntimeInformation.IsOSPlatform(OSPlatform.Windows)
            ? KnownPlatformGraphicsExternalImageHandleTypes.VulkanOpaqueNtHandle
            : KnownPlatformGraphicsExternalImageHandleTypes.VulkanOpaquePosixFileDescriptor;

        if (_gpuInterop.SupportedImageHandleTypes.All(t => t != imageType))
        {
            Log("Backend does not support " + imageType);
            return;
        }
        
        bool isWindows = RuntimeInformation.IsOSPlatform(OSPlatform.Windows);
        byte[]? gpuId = isWindows ? _gpuInterop.DeviceLuid : _gpuInterop.DeviceUuid;
        if (gpuId == null || gpuId.Length == 0)
        {
            Log("No device LUID/UUID from compositor");
            return;
        }
        
        if (!BastionInterop.PreInitWithId(gpuId, (uint)gpuId.Length, isWindows))
        {
            Log("Pre-init failed " + BastionInterop.LastError());
            return;
        }
        
        if (!BastionInterop.AddModel("slang.spv", 3))
        {
            Log("AddModel failed " + BastionInterop.LastError());
            return;
        }
        
        _drawingSurface = _compositor.CreateDrawingSurface();
        _surfaceVisual = _compositor.CreateSurfaceVisual();
        _surfaceVisual.Surface = _drawingSurface;
        ElementComposition.SetElementChildVisual(this, _surfaceVisual);
        _gpuInteropReady = true;
        
        double scaling = TopLevel.GetTopLevel(this)?.RenderScaling ?? 1.0;
        var px = PixelSize.FromSize(Bounds.Size, scaling);
        if (px.Width > 0 && px.Height > 0)
        {
            await ResizeAsync(px);
        }

        StartTimer();
    }

    private async Task ResizeAsync(PixelSize px)
    {
        if (_gpuInterop == null || _drawingSurface == null)
        {
            return;
        }
        StopTimer();
        ReleaseHandles();

        if (!BastionInterop.Resize((uint)px.Width, (uint)px.Height))
        {
            Log("BastionInterop resize failed: " + BastionInterop.LastError());
            _nativeReady = false;
            StartTimer();
            return;
        }

        _nativeReady = true;
        _nativeSize = px;
        
        BastionInterop.GetSharedFrame(out SharedFrame frame);
        Log($"frame mem = {frame.MemHandle} sig = {frame.SignalSemaphore} memSize = {frame.MemorySize}");
        if (frame.MemHandle == 0)
        {
            Log("MemHandle null: " + BastionInterop.LastError());
            StartTimer();
            return;
        }
        
        double scaling = TopLevel.GetTopLevel(this)?.RenderScaling ?? 1.0;
        _surfaceVisual!.Size = new Vector(frame.Width / scaling, frame.Height / scaling);

        string imageType = RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ? 
            KnownPlatformGraphicsExternalImageHandleTypes.VulkanOpaqueNtHandle :
            KnownPlatformGraphicsExternalImageHandleTypes.VulkanOpaquePosixFileDescriptor;
        
        string semaphoreType = RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ?
            KnownPlatformGraphicsExternalSemaphoreHandleTypes.VulkanOpaqueNtHandle :
            KnownPlatformGraphicsExternalSemaphoreHandleTypes.VulkanOpaquePosixFileDescriptor;

        var properties = new PlatformGraphicsExternalImageProperties
        {
            Width = (int)frame.Width,
            Height = (int)frame.Height,
            Format = PlatformGraphicsExternalImageFormat.B8G8R8A8UNorm,
            MemorySize = frame.MemorySize
        };

        try
        {
            _importedImage =
                _gpuInterop.ImportImage(new PlatformHandle((IntPtr)frame.MemHandle, imageType), properties);
            _importedSignalSemaphore =
                _gpuInterop.ImportSemaphore(new PlatformHandle((IntPtr)frame.SignalSemaphore, semaphoreType));
            _importedWaitSemaphore =
                _gpuInterop.ImportSemaphore(new PlatformHandle((IntPtr)frame.WaitSemaphore, semaphoreType));
            await _importedImage.ImportCompleted;
        }
        catch (Exception e)
        {
            Log("Import failed: " + e.Message);
            ReleaseHandles();
            StartTimer();
            return;
        }

        StartTimer();
    }
    
    private void StartTimer()
    {
        if (_timer != null)
        {
            return;
        }

        _timer = new DispatcherTimer(DispatcherPriority.Render)
        {
            Interval = TimeSpan.FromMilliseconds(1)
        };
        _timer.Tick += OnTick;
        _timer.Start();
    }
    
    private void StopTimer()
    {
        if (_timer == null)
        {
            return;
        }
        
        _timer.Stop();
        _timer.Tick -= OnTick;
        _timer = null;
    }

    private void OnTick(object? sender, EventArgs e)
    {
        if (_frameInFlight)
        {
            return;
        }

        if (_resizePending && _gpuInteropReady)
        {
            _resizePending = false;
            _frameInFlight = true;
            ResizeAsync(_pendingSize).ContinueWith(_ => _frameInFlight = false, TaskScheduler.FromCurrentSynchronizationContext());
            return;
        }

        if (!_nativeReady || _gpuInterop == null || _drawingSurface == null)
        {
            return;
        }

        if ((_importedImage?.IsLost ?? true) ||
            (_importedSignalSemaphore?.IsLost ?? true) ||
            (_importedWaitSemaphore?.IsLost ?? true))
        {
            _frameInFlight = true;
            ResizeAsync(_nativeSize).ContinueWith(_ => _frameInFlight = false, TaskScheduler.FromCurrentSynchronizationContext());
            return;
        }

        _anim = (_anim + 1.0f) % 360.0f;
        BastionInterop.Render(_anim);
        _frameInFlight = true;
        _drawingSurface.UpdateWithSemaphoresAsync(_importedImage!, _importedSignalSemaphore!, _importedWaitSemaphore!).ContinueWith(t =>
        {
            _frameInFlight = false;
            if (t.IsFaulted)
            {
                Log("UpdateWithSemaphoresAsync: " + t.Exception!.InnerException!.Message);
            }
        }, TaskScheduler.FromCurrentSynchronizationContext());
    }
    
    private void ReleaseHandles()
    {
        _importedImage?.DisposeAsync();
        _importedSignalSemaphore?.DisposeAsync();
        _importedWaitSemaphore?.DisposeAsync();
        _importedImage = null;
        _importedSignalSemaphore = null;
        _importedWaitSemaphore = null;
    }
    
    private void DestroyNative()
    {
        if (!_nativeReady)
        {
            return;
        }
        
        BastionInterop.Shutdown();
        _nativeReady = false;
    }

    private void DestroyComposition()
    {
        ReleaseHandles();
        ElementComposition.SetElementChildVisual(this, null);
        _surfaceVisual = null;
        _drawingSurface?.Dispose();
        _drawingSurface = null;
        _gpuInterop = null;
        _compositor = null;
        _gpuInteropReady = false;
    }

    public void Dispose()
    {
        if (_disposed)
        {
            return;
        }
        _disposed = true;
        StopTimer();
        DestroyNative();
        DestroyComposition();
    }
}