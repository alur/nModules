//-------------------------------------------------------------------------------------------------
// /Utilities/StopWatch.cpp
// The nModules Project
//
// Measures time between events.
//-------------------------------------------------------------------------------------------------
#include "Common.h"
#include "StopWatch.hpp"


StopWatch::StopWatch() {
  QueryPerformanceFrequency((LARGE_INTEGER*)&mClockRate);
  QueryPerformanceCounter((LARGE_INTEGER*)&mLastClock);
  mCreationTime = mLastClock;
}


float StopWatch::Clock() {
  __int64 currentClock;
  float timeSinceLastCall;

  QueryPerformanceCounter((LARGE_INTEGER*)&currentClock);
  timeSinceLastCall = (currentClock - mLastClock) / (float)mClockRate;
  mLastClock = currentClock;

  return timeSinceLastCall;
}


float StopWatch::GetTime() const {
  __int64 currentClock;
  QueryPerformanceCounter((LARGE_INTEGER*)&currentClock);
  return (currentClock - mLastClock) / (float)mClockRate;
}


float StopWatch::GetTotalTime() const {
  __int64 currentClock;
  QueryPerformanceCounter((LARGE_INTEGER*)&currentClock);
  return (currentClock - mCreationTime) / (float)mClockRate;
}
