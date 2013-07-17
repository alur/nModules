//--------------------------------------------------------------------------------------
// StopWatch.cpp
// The nModules project
//
// Measures time between events.
//
//--------------------------------------------------------------------------------------
#include <Windows.h>
#include "StopWatch.hpp"


/// <summary
/// Constructor
/// </summary>
StopWatch::StopWatch() {
    QueryPerformanceFrequency((LARGE_INTEGER*)&mClockRate);
    QueryPerformanceCounter((LARGE_INTEGER*)&mLastClock);
    mCreationTime = mLastClock;
}


/// <summary>
/// Returns the number of seconds since this StopWatch was created, or Clock was called.
/// </summary>
float StopWatch::Clock() {
    __int64 currentClock;
    float timeSinceLastCall;

    QueryPerformanceCounter((LARGE_INTEGER*)&currentClock);
    timeSinceLastCall = (currentClock - mLastClock)/(float)mClockRate;
    mLastClock = currentClock;

    return timeSinceLastCall;
}


/// <summary>
/// Returns the number of seconds since this StopWatch was created, or Clock was called.
/// </summary>
float StopWatch::GetTime() const {
    __int64 currentClock;

    QueryPerformanceCounter((LARGE_INTEGER*)&currentClock);

    return (currentClock - mLastClock)/(float)mClockRate;
}


/// <summary>
/// Returns the total number of seconds this clock has been alive.
/// </summary>
float StopWatch::GetTotalTime() const {
    __int64 currentClock;

    QueryPerformanceCounter((LARGE_INTEGER*)&currentClock);

    return (currentClock - mCreationTime)/(float)mClockRate;
}
