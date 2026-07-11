using Avalonia;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Markup.Xaml;
using Avalonia.Rendering;
using Microsoft.Extensions.DependencyInjection;
using Tenaille.Controls;
using Tenaille.Services;
using Tenaille.ViewModels;
using Tenaille.Views;

namespace Tenaille;

public partial class App : Application
{
    public ServiceProvider? Services { get; private set; } = null;

    public override void Initialize()
    {
        AvaloniaXamlLoader.Load(this);
    }

    public override void OnFrameworkInitializationCompleted()
    {
        Services = BuildServiceProvider();
        
        if (ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktop)
        {
            var mainViewModel = Services.GetRequiredService<MainWindowViewModel>();

            desktop.MainWindow = new MainWindow
            {
                DataContext = mainViewModel,
            };
        }

        base.OnFrameworkInitializationCompleted();
    }

    private static ServiceProvider BuildServiceProvider()
    {
        var collection = new ServiceCollection();

        collection.AddSingleton<ILogService, LogService>();
        collection.AddTransient<MainWindowViewModel>();
        
        return collection.BuildServiceProvider();
    }
}