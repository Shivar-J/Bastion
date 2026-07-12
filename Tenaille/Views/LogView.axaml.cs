using System.Collections.Specialized;
using Avalonia.Controls;
using Tenaille.ViewModels;

namespace Tenaille.Views;

public partial class LogView : UserControl
{
    public LogView()
    {
        InitializeComponent();
        DataContextChanged += OnDataContextChanged;
    }

    private void OnDataContextChanged(object? sender, System.EventArgs e)
    {
        if (DataContext is LogViewModel vm)
        {
            vm.Log.Entries.CollectionChanged += OnEntriesChanged;
        }
    }

    private void OnEntriesChanged(object? sender, NotifyCollectionChangedEventArgs e)
    {
        if (LogListBox.ItemCount > 0)
        {
            LogListBox.ScrollIntoView(LogListBox.ItemCount - 1);
        }
    }
}