using CommunityToolkit.Mvvm.ComponentModel;
using Tenaille.Services;

namespace Tenaille.ViewModels;

public partial class LogViewModel : ViewModelBase
{
    [ObservableProperty]
    private ILogService _log;
    
    public LogViewModel(ILogService log)
    {
        _log = log;
    }
}