//-------------------------------------------------------------------------------------------------
// /Utilities/StopWatch.hpp
// The nModules Project
//
// Measures time between events.
//-------------------------------------------------------------------------------------------------
#pragma once

/// <summary>
/// Measures time between events.
/// </sumary>
class StopWatch {
public:
  StopWatch();

public:
  /// <summary>
  /// Returns the number of seconds since this StopWatch was created, or Clock was called.
  /// </summary>
  float Clock();

  /// <summary>
  /// Returns the number of seconds since this StopWatch was created, or Clock was called.
  /// </summary>
  float GetTime() const;

  /// <summary>
  /// Returns the total number of seconds this clock has been alive.
  /// </summary>
  float GetTotalTime() const;

private:
  /// <summary>
  /// The tick at which Clock was last called, or this StopWatch was created.
  /// </summary>
  __int64 mLastClock;

  /// <summary>
  /// The tick at which this StopWatch was created.
  /// </summary>
  __int64 mCreationTime;

  /// <summary>
  /// The number of clock ticks per second.
  /// </summary>
  __int64 mClockRate;
};
