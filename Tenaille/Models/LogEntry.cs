using System;

namespace Tenaille.Models;

public struct LogEntry
{
    public DateTime Timestamp { get; set; }
    public LogLevel Level { get; set; }
    public string Message { get; set; }

    public string DisplayText => $"[{Timestamp:HH:mm:ss}][{Level}] {Message}";
}