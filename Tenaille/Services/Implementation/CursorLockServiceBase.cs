using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Tenaille.Services.Interfaces;

namespace Tenaille.Services.Implementation;

public abstract class CursorLockServiceBase : ICursorLockService
{
    private readonly Cursor _cursorHidden = new(StandardCursorType.None);
    private Cursor? _previousCursor;
    private Control? Surface { get; set; }

    public bool IsLocked { get; private set; }
    
    public void Lock(Control surface)
    {
        if (IsLocked)
        {
            return;
        }
        
        Surface = surface;
        _previousCursor = surface.Cursor;
        surface.Cursor = _cursorHidden;
        IsLocked = true;
        
        OnLocked();
        CenterPointer();
    }

    public void Unlock()
    {
        if (!IsLocked)
        {
            return;
        }
        
        OnUnlocked();

        if (Surface != null)
        {
            Surface.Cursor = _previousCursor;
        }
        
        _previousCursor = null;
        Surface = null;
        IsLocked = false;
    }

    public bool CenterPointer()
    {
        if (!IsLocked || Surface == null ||
            Surface.Bounds.Width <= 0 || Surface.Bounds.Height <= 0)
        {
            return false;
        }
        
        PixelPoint center = Surface.PointToScreen(new Point(Surface.Bounds.Width / 2.0,  Surface.Bounds.Height / 2.0));
        return CenterPointer(center);
    }

    protected virtual void OnLocked() {}
    protected virtual void OnUnlocked() {}
    protected abstract bool CenterPointer(PixelPoint point);

    public void Dispose()
    {
        Unlock();
        DisposePlatformResources();
    }
    
    protected virtual void DisposePlatformResources() {}
}