using System;
using Tenaille.Services.Interfaces;

namespace Tenaille.Services.Implementation;

public class CursorLockServiceFactory
{
    public static ICursorLockService Create()
    {
        if (OperatingSystem.IsWindows())
        {
            return new WindowsCursorLockService();
        }

        return new LinuxCursorLockService();
    }
}