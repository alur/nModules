/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WorkArea.cpp
 *  The nModules Project
 *
 *  Functions dealing with the workarea
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "WorkArea.h"
#include "../nCoreCom/Core.h"
#include "../nShared/Error.h"


/// <summary>
/// Sets the workarea for some monitor.
/// </summary>
/// <param name="mInfo"></param>
/// <param name="pszLine"></param>
void WorkArea::ParseLine(MonitorInfo * mInfo, LPCSTR pszLine) {
    char szMonitor[16], szLeft[16], szTop[16], szRight[16], szBottom[16];
    LPSTR szTokens[] = { szMonitor, szLeft, szTop, szRight, szBottom };
    int left, top, right, bottom;
    UINT monitor;

    using namespace nCore::InputParsing;

    // Parse the input string
    if (LCTokenize(pszLine, szTokens, 5, NULL) == 5) {
        if (ParseCoordinate(szLeft, &left) && ParseCoordinate(szTop, &top) && ParseCoordinate(szRight, &right) && ParseCoordinate(szBottom, &bottom)) {
            if (_stricmp("all", szMonitor) == 0) {
                for (std::vector<MonitorInfo::Monitor>::iterator iter = mInfo->m_monitors.begin(); iter != mInfo->m_monitors.end(); iter++) {
                    RECT r = { iter->rect.left + left, iter->rect.top + top, iter->rect.right - right, iter->rect.bottom - bottom };
                    SystemParametersInfo(SPI_SETWORKAREA, 0, &r, SPIF_SENDCHANGE);
                }
                return;
            }
            else if (ParseMonitor(szMonitor, &monitor)) {
                if (monitor < mInfo->m_monitors.size()) {
                    RECT mRect = mInfo->m_monitors[monitor].rect;
                    RECT r = { mRect.left + left, mRect.top + top, mRect.right - right, mRect.bottom - bottom };
                    SystemParametersInfo(SPI_SETWORKAREA, 0, &r, SPIF_SENDCHANGE);
                }
                return;
            }
        }
    }

    ErrorMessage(E_LVL_WARNING, "%s\nIs not a valid workarea declaration!", pszLine);
}


/// <summary>
/// Reads and applies all settings from the RC files.
/// </summary>
/// <param name="mInfo">A current MonitorInfo.</param>
void WorkArea::LoadSettings(MonitorInfo * mInfo, bool /* bIsRefresh */) {
    char szLine[MAX_LINE_LENGTH];
    LPVOID f = LCOpen(NULL);
    while (LCReadNextConfig(f, "*nDeskWorkArea", szLine, sizeof(szLine))) {
        ParseLine(mInfo, szLine+strlen("*nDeskWorkArea")+1);
    }
    LCClose(f);
}


/// <summary>
/// Resets the workareas for all monitors.
/// </summary>
/// <param name="mInfo">A current MonitorInfo.</param>
void WorkArea::ResetWorkAreas(MonitorInfo * mInfo) {
    for (std::vector<MonitorInfo::Monitor>::iterator iter = mInfo->m_monitors.begin(); iter != mInfo->m_monitors.end(); iter++) {
        SystemParametersInfo(SPI_SETWORKAREA, 0, &iter->rect, SPIF_SENDCHANGE);
    }
}
