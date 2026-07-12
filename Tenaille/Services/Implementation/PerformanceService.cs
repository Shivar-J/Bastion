using System;
using System.Diagnostics;

namespace Tenaille.Services.Implementation;

internal class PerformanceService
{
    private readonly Stopwatch _stopwatch =  Stopwatch.StartNew();
    private long _lastTicks;
    private double _accumulator;
    private int _frameCount;

    public event Action<double>? Updated;

    public void Tick()
    {
        long currentTicks = _stopwatch.ElapsedTicks;
        double deltaTime = (double)(currentTicks - _lastTicks) / Stopwatch.Frequency;
        _lastTicks = currentTicks;

        _accumulator += deltaTime;
        _frameCount++;

        if (_accumulator >= 0.5)
        {
            double fps = _frameCount / _accumulator;
            _frameCount = 0;
            _accumulator = 0;

            Updated?.Invoke(fps);
        }
    }
}