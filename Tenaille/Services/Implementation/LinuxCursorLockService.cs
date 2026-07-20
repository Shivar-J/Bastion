using System.Runtime.InteropServices;
using Avalonia;

namespace Tenaille.Services.Implementation;

public partial class LinuxCursorLockService : CursorLockServiceBase
{
    private nint _display;
    
    protected override void OnLocked()
    {
        _display = XOpenDisplay(nint.Zero);
    }

    protected override void OnUnlocked()
    {
        CloseDisplay();
    }

    protected override bool CenterPointer(PixelPoint point)
    {
        if (_display == nint.Zero)
        {
            return false;
        }

        nint root = XDefaultRootWindow(_display);
        int result = XWarpPointer(
            _display,
            nint.Zero,
            root,
            0, 0, 0, 0,
            point.X, point.Y
        );

        XFlush(_display);
        return result != 0;
    }

    protected override void DisposePlatformResources()
    {
        CloseDisplay();
    }

    private void CloseDisplay()
    {
        if (_display == nint.Zero)
        {
            return;
        }

        XCloseDisplay(_display);
        _display = nint.Zero;
    }

    [LibraryImport("libX11.so.6")]
    private static partial nint XOpenDisplay(nint display);
    
    [LibraryImport("libX11.so.6")]
    private static partial nint XDefaultRootWindow(nint display);

    [LibraryImport("libX11.so.6")]
    private static partial int XWarpPointer(nint display, nint source, nint dest,
        int srcX, int srcY, uint width, uint height, int destX, int destY);

    [LibraryImport("libX11.so.6")]
    private static partial int XFlush(nint display);

    [LibraryImport("libX11.so.6")]
    private static partial int XCloseDisplay(nint display);
}