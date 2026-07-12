using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Media;
using Tenaille.Models;
using Tenaille.Services;
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
    }

    protected override void OnDetachedFromVisualTree(VisualTreeAttachmentEventArgs e)
    {
        base.OnDetachedFromVisualTree(e);
        Renderer?.DetachFromVisualTree(this);
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
                MouseX = (float)position.X,
                MouseY = (float)position.Y,
                WheelX = 0.0f,
                WheelY = 0.0f
            }
        ], 1);
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