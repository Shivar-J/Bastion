using System.Runtime.InteropServices;

namespace Tenaille.Models;

[StructLayout(LayoutKind.Sequential, Pack=1)]
public struct SharedFrame
{
    public long MemHandle;
    public long WaitSemaphore;
    public long SignalSemaphore;
    public uint Width;
    public uint Height;
    public uint Format;
    public ulong MemorySize;
}