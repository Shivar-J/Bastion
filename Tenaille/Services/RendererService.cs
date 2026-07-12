using System;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Platform;
using Avalonia.Rendering.Composition;
using Avalonia.Threading;
using Tenaille.Models;

namespace Tenaille.Services;

public class RendererService : IRendererService
{
    private readonly ILogService _log;
    private readonly BastionInteropHandles _handles = new();
    private readonly PerformanceService _performance = new();

    private ICompositionGpuInterop? _gpu;
    private bool _gpuReady;
    private bool _nativeReady;
    private PixelSize _nativeSize;
    private bool _resizePending;
    private PixelSize _pendingSize;
    private float _anim;

    private Task? _loop;
    private bool _running;

    private Visual? _visual;
    private CompositionSurfaceVisual? _surfaceVisual;
    private CompositionDrawingSurface? _surface;
    public Action<double>? FpsUpdated { get; set; }
    
    public RendererService(ILogService log)
    {
        _log = log;
        _performance.Updated += fps => FpsUpdated?.Invoke(fps);
    }

    public void Start()
    {
        if (_loop != null)
        {
            return;
        }

        _running = true;
        _loop = Dispatcher.UIThread.InvokeAsync(RunAsync,  DispatcherPriority.Render);
    }

    public void Stop()
    {
        _running = false;
        _loop = null;
    }
    
    public async Task<bool> InitAsync(Compositor compositor)
    {
        _gpu = await compositor.TryGetCompositionGpuInterop();
        if (_gpu == null)
        {
            _log.Log("ICompositionGpuInterop not available", LogLevel.Error);
            return false;
        }
        
        string imageType = RuntimeInformation.IsOSPlatform(OSPlatform.Windows)
            ? KnownPlatformGraphicsExternalImageHandleTypes.VulkanOpaqueNtHandle
            : KnownPlatformGraphicsExternalImageHandleTypes.VulkanOpaquePosixFileDescriptor;

        if (_gpu.SupportedImageHandleTypes.All(t => t != imageType))
        {
            _log.Log("Backend does not support " + imageType,  LogLevel.Error);
            return false;
        }

        bool isWindows = RuntimeInformation.IsOSPlatform(OSPlatform.Windows);
        byte[]? gpuId = isWindows ? _gpu.DeviceLuid : _gpu.DeviceUuid;
        if (gpuId == null || gpuId.Length == 0)
        {
            _log.Log("No device LUID/UUID from compositor",  LogLevel.Error);
            return false;
        }
        
        if (!BastionInterop.PreInitWithId(gpuId, (uint)gpuId.Length, isWindows))
        {
            _log.Log("Pre-init failed " + BastionInterop.LastError(), LogLevel.Error);
            return false;
        }
        
        if (!BastionInterop.AddModel("slang.spv", 3))
        {
            _log.Log("AddModel failed " + BastionInterop.LastError(), LogLevel.Error);
            return false;
        }

        _surface = compositor.CreateDrawingSurface();
        _gpuReady = true;

        Start();        

        return true;
    }

    public async Task RunAsync()
    {
        while (_running)
        {
            try
            {
                await TickAsync();
            }
            catch (Exception e)
            {
                _log.Log(e.Message, LogLevel.Error);
                return;
            }
        }
    }

    public async Task TickAsync()
    {
        if (_resizePending && _gpuReady)
        {
            _resizePending = false;
            await ResizeAsync(_pendingSize);
            return;
        }

        if (!_nativeReady || _gpu == null || _surface == null)
        {
            return;
        }

        if (_handles.IsLost)
        {
            await ResizeAsync(_nativeSize);
            return;
        }

        _anim = (_anim + 1.0f) % 360.0f;
        BastionInterop.Render(_anim);

        await _surface.UpdateWithSemaphoresAsync(_handles.Image!, 
            _handles.SignalSemaphore!,
            _handles.WaitSemaphore!);

        _performance.Tick();
    }

    public void RequestResize(PixelSize pixelSize)
    {
        if (!_gpuReady || pixelSize.Width <= 0 || pixelSize.Height <= 0)
        {
            return;
        }
        
        _pendingSize = pixelSize;
        _resizePending = true;
    }

    public void HandleInput(UserInput[] states, uint count) => 
        BastionInterop.HandleUserInput(states, count);

    public async void AttachToVisualTree(Visual visual)
    {
        try
        {
            _visual = visual;
        
            var compositor = ElementComposition.GetElementVisual(visual)?.Compositor;
            if (compositor == null)
            {
                return;
            }

            if (!_gpuReady)
            {
                await InitAsync(compositor);
            }
        
            _surfaceVisual = compositor.CreateSurfaceVisual();
            _surfaceVisual.Surface = _surface;
            _surfaceVisual.Size = new Vector(visual.Bounds.Width, visual.Bounds.Height);
            ElementComposition.SetElementChildVisual(visual, _surfaceVisual);

            var scaling = TopLevel.GetTopLevel(visual)?.RenderScaling ?? 1.0;
            var px = PixelSize.FromSize(visual.Bounds.Size, scaling);
            if (px is { Width: > 0, Height: > 0 })
            {
                RequestResize(px);
            }
        
            Start();
        }
        catch (Exception e)
        {
            _log.Log(e.Message, LogLevel.Error);
        }
    }

    public void DetachFromVisualTree(Visual visual)
    {
        Dispose();
    }

    private async Task ResizeAsync(PixelSize pixelSize)
    {
        if (_gpu == null || _surface == null)
        {
            return;
        }

        await _handles.ReleaseAsync();

        if (!BastionInterop.Resize((uint)pixelSize.Width, (uint)pixelSize.Height))
        {
            _log.Log("Resize failed " + BastionInterop.LastError(), LogLevel.Error);
            _nativeReady = false;
            return;
        }
        
        _nativeReady = true;
        _nativeSize = pixelSize;
        
        BastionInterop.GetSharedFrame(out SharedFrame frame);
        _log.Log($"frame mem = {frame.MemHandle} sig = {frame.SignalSemaphore} memSize = {frame.MemorySize}");
        if (frame.MemHandle == 0)
        {
            _log.Log("GetSharedFrame failed " + BastionInterop.LastError(), LogLevel.Error);
            return;
        }

        if (_surfaceVisual == null)
        {
            return;
        }
        
        _surfaceVisual.Size = new Vector(_visual!.Bounds.Width,  _visual!.Bounds.Height);
        
        string imageType = RuntimeInformation.IsOSPlatform(OSPlatform.Windows)
            ? KnownPlatformGraphicsExternalImageHandleTypes.VulkanOpaqueNtHandle
            : KnownPlatformGraphicsExternalImageHandleTypes.VulkanOpaquePosixFileDescriptor;

        string semaphoreType = RuntimeInformation.IsOSPlatform(OSPlatform.Windows)
            ? KnownPlatformGraphicsExternalSemaphoreHandleTypes.VulkanOpaqueNtHandle
            : KnownPlatformGraphicsExternalSemaphoreHandleTypes.VulkanOpaquePosixFileDescriptor;

        var properties = new PlatformGraphicsExternalImageProperties
        {
            Width = (int)frame.Width,
            Height = (int)frame.Height,
            Format = PlatformGraphicsExternalImageFormat.B8G8R8A8UNorm,
            MemorySize = frame.MemorySize
        };
        
        try
        {
            _handles.Image = _gpu.ImportImage(new PlatformHandle((IntPtr)frame.MemHandle, imageType), properties);
            _handles.SignalSemaphore = _gpu.ImportSemaphore(new PlatformHandle((IntPtr)frame.SignalSemaphore, semaphoreType));
            _handles.WaitSemaphore = _gpu.ImportSemaphore(new PlatformHandle((IntPtr)frame.WaitSemaphore, semaphoreType));
            await _handles.Image.ImportCompleted;
        }
        catch (Exception e)
        {
            _log.Log("Import failed: " + e.Message, LogLevel.Error);
            await _handles.ReleaseAsync();
        }
    }

    public void Shutdown()
    {
        Stop();

        if (_visual != null)
        {
            ElementComposition.SetElementChildVisual(_visual, null);
            _visual = null;
            _surfaceVisual = null;
            _surface = null;
        }

        if (_nativeReady)
        {
            BastionInterop.Shutdown();
            _nativeReady = false;
        }

        _ = _handles.ReleaseAsync();
        _gpuReady = false;
    }

    public void Dispose()
    {
        Shutdown();
    }
}