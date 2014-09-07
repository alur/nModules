/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  MonitorInfo.hpp
 *  The nModules Project
 *
 *  Function declarations for the MonitorInfo class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <vector>
#include "../Utilities/Common.h"

using std::vector;

class MonitorInfo {
public:
    struct Monitor
    {
        RECT rect;
        RECT workArea;
        int width;
        int height;
        int workAreaWidth;
        int workAreaHeight;
    };

    MonitorInfo();

    void Update();
    UINT MonitorFromHWND(HWND hWnd) const;
    UINT MonitorFromRECT(LPRECT rect) const;

    // Every monitor
    vector<Monitor> m_monitors;

    // The virtual desktop
    Monitor m_virtualDesktop;

private:
};
