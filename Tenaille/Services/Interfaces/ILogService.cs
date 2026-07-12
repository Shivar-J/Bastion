using System.Collections.ObjectModel;
using Tenaille.Models;

namespace Tenaille.Services.Interfaces;

public interface ILogService
{
    ObservableCollection<LogEntry> Entries { get; }
    
    void Log(string message, LogLevel level = LogLevel.Info);
    void Clear();
}