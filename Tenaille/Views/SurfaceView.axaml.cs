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
        base.OnDetachedFromVisualTree(e);
        Renderer?.DetachFromVisualTree(this);
        
        var topLevel = TopLevel.GetTopLevel(this)!;
        topLevel.KeyDown -= OnKeyDown;
        topLevel.KeyUp -= OnKeyUp;
        topLevel.PointerPressed -= OnPointerPressed;
    }

    protected override void OnPointerMoved(PointerEventArgs e)
    {
        base.OnPointerMoved(e);
        Point position = e.GetPosition(this);
        
        Renderer?.HandleInput([
            new UserInput
            {
                Type = InputType.MouseMove,
                KeyCode = Key.None,
                MouseCode = MouseKey.None,
                MouseX = (float)position.X,
                MouseY = (float)position.Y,
                WheelX = 0.0f,
                WheelY = 0.0f
            }
        ]);
    }

    private void OnPointerPressed(object? sender, PointerPressedEventArgs e)
    {
        List<UserInput> inputs = new List<UserInput>();
        var point = e.GetCurrentPoint(sender as Control);
        var x = point.Position.X;
        var y = point.Position.Y;
        if (point.Properties.IsLeftButtonPressed)
        {
            inputs.Add(new UserInput
            {
                Type = InputType.MouseButton,
                KeyCode = Key.None,
                MouseCode = MouseKey.LeftMouseDown,
                MouseX = (float)x,
                MouseY = (float)y,
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
                MouseX = (float)x,
                MouseY = (float)y,
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
                MouseX = (float)x,
                MouseY = (float)y,
                WheelX = 0.0f,
                WheelY = 0.0f
            });
        }
        
        Renderer?.HandleInput(inputs.ToArray());
    }

    private void OnKeyDown(object? sender, KeyEventArgs e)
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
    }

    private void OnKeyUp(object? sender, KeyEventArgs e)
    {
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
    }
}