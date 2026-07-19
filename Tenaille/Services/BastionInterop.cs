using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using Tenaille.Models;

[assembly: DisableRuntimeMarshalling]

namespace Tenaille.Services;

public static partial class BastionInterop
{
    private const string Dll = "Bastion";

    [LibraryImport(Dll, EntryPoint = "PreInitWithId")]
    [return: MarshalAs(UnmanagedType.U1)]
    internal static partial bool PreInitWithId(byte[] idBytes, uint idLen, [MarshalAs(UnmanagedType.U1)] bool isLuid);

    [LibraryImport(Dll, EntryPoint = "AddModel", StringMarshalling = StringMarshalling.Utf8)]
    [return: MarshalAs(UnmanagedType.U1)]
    internal static partial bool AddModel(string shaderFile, uint vertexCount);
    
    [LibraryImport(Dll, EntryPoint = "Resize")]
    [return: MarshalAs(UnmanagedType.U1)]
    internal static partial bool Resize(uint w, uint h);

    [LibraryImport(Dll, EntryPoint = "Render")]
    internal static partial void Render();
    
    [LibraryImport(Dll, EntryPoint = "HandleUserInput")]
    internal static partial void HandleUserInput([In] UserInput[] inputs, uint count);
    
    [LibraryImport(Dll, EntryPoint = "GetSharedFrame")]
    internal static partial void GetSharedFrame(out SharedFrame sharedFrame);
    
    [LibraryImport(Dll, EntryPoint = "Shutdown")]
    internal static partial void Shutdown();

    [LibraryImport(Dll, EntryPoint = "LastError")]
    internal static partial IntPtr LastErrorPtr();
    
    public static string LastError() => Marshal.PtrToStringAnsi(LastErrorPtr()) ?? "";
}