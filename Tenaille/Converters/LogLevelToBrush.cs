using System;
using System.Globalization;
using Avalonia.Data.Converters;
using Avalonia.Media;
using Tenaille.Models;

namespace Tenaille.Converters;

public class LogLevelToBrush : IValueConverter
{
    public object? Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
    {
        return value switch
        {
            LogLevel.Info => Brushes.Gray,
            LogLevel.Warning => Brushes.Orange,
            LogLevel.Error => Brushes.OrangeRed,
            _ => Brushes.White
        };
    }

    public object? ConvertBack(object? value, Type targetType, object? parameter, CultureInfo culture)
    {
        throw new NotImplementedException();
    }
}