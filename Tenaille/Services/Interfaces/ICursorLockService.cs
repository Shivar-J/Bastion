using System;
using Avalonia.Controls;

namespace Tenaille.Services.Interfaces;

public interface ICursorLockService : IDisposable
{
    bool IsLocked { get; }
    
    void Lock(Control surface);
    void Unlock();
    bool CenterPointer();
}