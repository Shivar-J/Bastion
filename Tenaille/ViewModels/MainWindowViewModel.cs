using Tenaille.Services;

namespace Tenaille.ViewModels;

public partial class MainWindowViewModel : ViewModelBase
{
    public LogViewModel LogViewModel { get; }
    
    public MainWindowViewModel(ILogService log)
    {
        LogViewModel = new LogViewModel(log);
    }
}