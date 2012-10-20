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
        RECT workArea;
        int width;
        int height;
        int workAreaWidth;
        int workAreaHeight;
    } Monitor;

    explicit MonitorInfo();
    virtual ~MonitorInfo();
    void Update();
    UINT MonitorFromHWND(HWND hWnd);
    UINT MonitorFromRECT(LPRECT rect);

    // Every monitor
    vector<Monitor> m_monitors;

    // The virtual desktop
    Monitor m_virtualDesktop;

private:
};
