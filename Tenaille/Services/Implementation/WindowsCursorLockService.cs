using System.Runtime.InteropServices;
using Avalonia;

namespace Tenaille.Services.Implementation;

public partial class WindowsCursorLockService : CursorLockServiceBase
{
    protected override bool CenterPointer(PixelPoint pos)
    {
        return SetCursorPos(pos.X, pos.Y);
    }

    [LibraryImport("user32.dll")]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static partial bool SetCursorPos(int x, int y);
}