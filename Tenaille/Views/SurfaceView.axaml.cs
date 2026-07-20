using System.Collections.Generic;
using System.Diagnostics;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Media;
using Tenaille.Models;
using Tenaille.Services.Interfaces;
using Tenaille.ViewModels;

namespace Tenaille.Views;

public partial class SurfaceView : UserControl
{ 
    private IRendererService? Renderer => (DataContext as SurfaceViewModel)?.Renderer;
    private ICursorLockService? CursorLock => (DataContext as SurfaceViewModel)?.CursorLock;
    private Point? _lastPosition;
    
    public SurfaceView()
    {
        InitializeComponent();
    }

    public override void Render(DrawingContext context)
    {
        context.DrawRectangle(Brushes.Transparent, null, new Rect(Bounds.Size));
        base.Render(context);
    }

    protected override void OnAttachedToVisualTree(VisualTreeAttachmentEventArgs e)
    {
        base.OnAttachedToVisualTree(e);
        Renderer?.AttachToVisualTree(this);

        var topLevel = TopLevel.GetTopLevel(this)!;
        topLevel.KeyDown += OnKeyDown;
        topLevel.KeyUp += OnKeyUp;
        topLevel.PointerPressed += OnPointerPressed;
    }

    protected override void OnDetachedFromVisualTree(VisualTreeAttachmentEventArgs e)
    {
        CursorLock?.Unlock();
        base.OnDetachedFromVisualTree(e);
        Renderer?.DetachFromVisualTree(this);
        
        var topLevel = TopLevel.GetTopLevel(this)!;
        topLevel.KeyDown -= OnKeyDown;
        topLevel.KeyUp -= OnKeyUp;
        topLevel.PointerPressed -= OnPointerPressed;

        _lastPosition = null;
    }

    protected override void OnPointerMoved(PointerEventArgs e)
    {
        base.OnPointerMoved(e);

        ICursorLockService? cursorLock = CursorLock;
        if (cursorLock is not { IsLocked: true })
        {
            return;
        }
        
        Point position = e.GetPosition(this);
        Point center = new (Bounds.Width / 2, Bounds.Height / 2);
        Vector delta = position - center;

        if (delta.SquaredLength < 0.01)
        {
            return;
        }

        cursorLock.CenterPointer();
        
        Renderer?.HandleInput([
            new UserInput
            {
                Type = InputType.MouseMove,
                KeyCode = Key.None,
                MouseCode = MouseKey.None,
                MouseX = (float)delta.X,
                MouseY = (float)delta.Y,
                WheelX = 0.0f,
                WheelY = 0.0f
            }
        ]);
    }

    private void OnPointerPressed(object? sender, PointerPressedEventArgs e)
    {
        List<UserInput> inputs = new List<UserInput>();
        var point = e.GetCurrentPoint(sender as Control);
        Point position = e.GetPosition(this);
        if (_lastPosition is not Point prev)
        {
            _lastPosition = position;
            return;
        }

        Vector delta = position - prev;
        _lastPosition = position;
        if (point.Properties.IsLeftButtonPressed)
        {
            inputs.Add(new UserInput
            {
                Type = InputType.MouseButton,
                KeyCode = Key.None,
                MouseCode = MouseKey.LeftMouseDown,
                MouseX = (float)delta.X,
                MouseY = (float)delta.Y,
                WheelX = 0.0f,
                WheelY = 0.0f
            });
        }
        else if (point.Properties.IsRightButtonPressed)
        {
            inputs.Add(new UserInput
            {
                Type = InputType.MouseButton,
                KeyCode = Key.None,
                MouseCode = MouseKey.RightMouseDown,
                MouseX = (float)delta.X,
                MouseY = (float)delta.Y,
                WheelX = 0.0f,
                WheelY = 0.0f
            });
        }
        else if (point.Properties.IsMiddleButtonPressed)
        {
            inputs.Add(new UserInput
            {
                Type = InputType.MouseButton,
                KeyCode = Key.None,
                MouseCode = MouseKey.MiddleMouseDown,
                MouseX = (float)delta.X,
                MouseY = (float)delta.Y,
                WheelX = 0.0f,
                WheelY = 0.0f
            });
        }
        
        Renderer?.HandleInput(inputs.ToArray());
    }

    private void OnKeyDown(object? sender, KeyEventArgs e)
    {
        switch (e.Key)
        {
            case Key.Escape:
                CursorLock?.Unlock();
                e.Handled = true;
                return;
            case Key.OemTilde:
            {
                if (CursorLock is { IsLocked: false } cursorLock)
                {
                    Focus();
                    _lastPosition = null;
                    cursorLock.Lock(this);
                }
            
                e.Handled = true;
                return;
            }
            default:
            {
                Key keyDown = e.Key;

                Renderer?.HandleInput([
                    new UserInput
                    {
                        Type = InputType.KeyDown,
                        KeyCode = keyDown,
                        MouseX = 0.0f,
                        MouseY = 0.0f,
                        WheelX = 0.0f,
                        WheelY = 0.0f
                    }
                ]);
                break;
            }
        }
    }

    private void OnKeyUp(object? sender, KeyEventArgs e)
    {
        if (e.Key is Key.Escape or Key.OemTilde)
        {
            e.Handled = true;
            return;
        }
        
        Key keyUp = e.Key;

        Renderer?.HandleInput([
            new UserInput
            {
                Type = InputType.KeyUp,
                KeyCode = keyUp,
                MouseCode = MouseKey.None,
                MouseX = 0.0f,
                MouseY = 0.0f,
                WheelX = 0.0f,
                WheelY = 0.0f
            }
        ]);
    }

    protected override void OnSizeChanged(SizeChangedEventArgs e)
    {
        base.OnSizeChanged(e);
        double scaling = TopLevel.GetTopLevel(this)?.RenderScaling ?? 1.0;
        var px = PixelSize.FromSize(e.NewSize, scaling);
        if (px.Width <= 0 || px.Height <= 0) return;
        Renderer?.RequestResize(px);

        if (CursorLock is { IsLocked: false } cursorLock)
        {
            cursorLock.CenterPointer();
        }
    }

    protected override void OnPointerCaptureLost(PointerCaptureLostEventArgs e)
    {
        base.OnPointerCaptureLost(e);
        CursorLock?.Unlock();
    }
}