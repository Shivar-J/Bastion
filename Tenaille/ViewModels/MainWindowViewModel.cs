using Tenaille.Services;

namespace Tenaille.ViewModels;

public partial class MainWindowViewModel : ViewModelBase
{
    public LogViewModel LogViewModel { get; }
    public SurfaceViewModel SurfaceViewModel { get; }
    
    public MainWindowViewModel(ILogService log, SurfaceViewModel surfaceViewModel)
    {
        LogViewModel = new LogViewModel(log);
        SurfaceViewModel = surfaceViewModel;
    }
}