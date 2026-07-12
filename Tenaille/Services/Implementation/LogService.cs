using System;
using System.Collections.ObjectModel;
using Tenaille.Models;
using Tenaille.Services.Interfaces;

namespace Tenaille.Services.Implementation;

public class LogService : ILogService
{
    public ObservableCollection<LogEntry> Entries { get; } = new();

    public void Log(string message, LogLevel level = LogLevel.Info)
    {
        Entries.Add(new LogEntry
        {
            Timestamp = DateTime.Now,
            Level = level,
            Message = message
        });
    }

    public void Clear()
    {
        Entries.Clear();
    }
}