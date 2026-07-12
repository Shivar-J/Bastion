using System;
using System.Threading.Tasks;
using Avalonia.Rendering.Composition;

namespace Tenaille.Services;

internal class BastionInteropHandles
{
    public ICompositionImportedGpuImage? Image;
    public ICompositionImportedGpuSemaphore? WaitSemaphore;
    public ICompositionImportedGpuSemaphore? SignalSemaphore;
    
    public bool IsLost => (Image?.IsLost ?? true) || 
                          (WaitSemaphore?.IsLost ?? true) || 
                          (SignalSemaphore?.IsLost ?? true);

    public ValueTask ReleaseAsync()
    {
        try
        {
            Image?.DisposeAsync();
            WaitSemaphore?.DisposeAsync();
            SignalSemaphore?.DisposeAsync();

            Image = null;
            WaitSemaphore = null;
            SignalSemaphore = null;
            return ValueTask.CompletedTask;
        }
        catch (Exception exception)
        {
            return ValueTask.FromException(exception);
        }
    }
}