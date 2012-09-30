/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  MonitorInfo.hpp
 *  The nModules Project
 *
 *  Function declarations for the MonitorInfo class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <vector>

using std::vector;

class MonitorInfo {
public:
    typedef struct Monitor {
        RECT rect;
        int width;
        int height;
    } Monitor;

    explicit MonitorInfo();
    virtual ~MonitorInfo();
    void Update();
    UINT MonitorFromHWND(HWND hWnd);

    // Every monitor
    vector<Monitor> m_monitors;

    // The virtual desktop
    Monitor m_virtualDesktop;

private:
};
