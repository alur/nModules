/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WorkArea.cpp
 *  The nModules Project
 *
 *  Functions dealing with the workarea
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "WorkArea.h"
#include "../nCoreCom/Core.h"
#include "../nShared/ErrorHandler.h"


/// <summary>
/// Sets the workarea for some monitor.
/// </summary>
/// <param name="mInfo"></param>
/// <param name="pszLine"></param>
void WorkArea::ParseLine(MonitorInfo *mInfo, LPCTSTR pszLine)
{
    TCHAR szMonitor[16], szLeft[16], szTop[16], szRight[16], szBottom[16];
    LPTSTR szTokens[] = { szMonitor, szLeft, szTop, szRight, szBottom };
    int left, top, right, bottom;

    using namespace LiteStep;

    // Parse the input string
    if (LiteStep::LCTokenize(pszLine, szTokens, 5, nullptr) == 5)
    {
        left = _wtoi(szLeft);
        top = _wtoi(szTop);
        right = _wtoi(szRight);
        bottom = _wtoi(szBottom);

        UINT monitor = ParseMonitor(szMonitor, UINT(-2));

        if (monitor == UINT(-1))
        {
            for (auto &monitor : mInfo->GetMonitors())
            {
                RECT r = { monitor.rect.left + left, monitor.rect.top + top, monitor.rect.right - right, monitor.rect.bottom - bottom };
                SystemParametersInfoW(SPI_SETWORKAREA, 1, &r, 0);
            }
            return;
        }
        else if (monitor < mInfo->GetMonitorCount())
        {
            RECT mRect = mInfo->GetMonitor(monitor).rect;
            RECT r = { mRect.left + left, mRect.top + top, mRect.right - right, mRect.bottom - bottom };
            SystemParametersInfoW(SPI_SETWORKAREA, 1, &r, 0);
            return;
        }
    }

    ErrorHandler::Error(ErrorHandler::Level::Warning, L"%s\nIs not a valid workarea declaration!", pszLine);
}


/// <summary>
/// Reads and applies all settings from the RC files.
/// </summary>
/// <param name="mInfo">A current MonitorInfo.</param>
void WorkArea::LoadSettings(MonitorInfo *mInfo, bool /* isRefresh */) {
    LiteStep::IterateOverLines(L"*nDeskWorkArea", [mInfo] (LPCTSTR line) {
        ParseLine(mInfo, line);
    });

    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETWORKAREA, 0);
}


/// <summary>
/// Resets the workareas for all monitors.
/// </summary>
/// <param name="mInfo">A current MonitorInfo.</param>
void WorkArea::ResetWorkAreas(MonitorInfo *mInfo) {
  for (auto &monitor : mInfo->GetMonitors()) {
    SystemParametersInfoW(SPI_SETWORKAREA, 1, const_cast<PRECT>(&monitor.rect), 0);
  }
  SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETWORKAREA, 0);
}
