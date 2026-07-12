using System.Runtime.InteropServices;
using Avalonia.Input;

namespace Tenaille.Models;

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct UserInput
{
    public InputType Type;
    public Key KeyCode;
    public MouseKey MouseCode;
    public float MouseX;
    public float MouseY;
    public float WheelX;
    public float WheelY;

    public override string ToString()
    {
        return $"InputType: {Type.ToString()}, Key: {KeyCode}, MouseKey: {MouseCode.ToString()}, MouseX: {MouseX}, MouseY: {MouseY}, WheelX: {WheelX}, WheelY: {WheelY}";
    }
}