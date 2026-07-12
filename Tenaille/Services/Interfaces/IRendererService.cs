using System;
using System.Threading.Tasks;
using Avalonia;
using Avalonia.Rendering.Composition;
using Tenaille.Models;

namespace Tenaille.Services.Interfaces;

public interface IRendererService : IDisposable
{
    Action<double>? FpsUpdated { get; set; }

    void Start();
    void Stop();
    void Shutdown();
    Task<bool> InitAsync(Compositor compositor);
    Task RunAsync();
    Task TickAsync();
    void RequestResize(PixelSize pixelSize);
    void HandleInput(UserInput[] inputs, uint count);
    //void OnError(Exception error);
    void AttachToVisualTree(Visual visual);
    void DetachFromVisualTree(Visual visual);
}