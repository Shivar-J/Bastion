using System;
using CommunityToolkit.Mvvm.ComponentModel;
using Tenaille.Services;

namespace Tenaille.ViewModels;

public partial class SurfaceViewModel : ViewModelBase, IDisposable
{
    [ObservableProperty] private IRendererService _renderer;

    [ObservableProperty] private double _fps;
    [ObservableProperty] private string _title = "Bastion";
    
    public SurfaceViewModel(IRendererService renderer)
    {
        _renderer = renderer;
        _renderer.FpsUpdated += OnFpsUpdated;
    }

    private void OnFpsUpdated(double fps)
    {
        Fps = fps;
        Title = $"Bastion [FPS: {fps:F0}]";
    }

    public void Dispose()
    {
        Renderer.FpsUpdated -= OnFpsUpdated;
    }
}