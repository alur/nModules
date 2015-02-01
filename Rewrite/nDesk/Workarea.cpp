#include "Workarea.h"

#include "../nCoreApi/Core.h"

#include "../nUtilities/lsapi.h"

#include <algorithm>

extern ILogger *gLogger;


void LoadWorkareas() {
  nCore::EnumRCLines(L"*nDeskWorkArea", [] (LPCWSTR line, LPARAM) -> void {
    wchar_t monitorTok[16], leftTok[16], topTok[16], rightTok[16], bottomTok[16];
    LPWSTR tokens[] = { monitorTok, leftTok, topTok, rightTok, bottomTok };

    if (LCTokenize(line, tokens, 5, nullptr) == 5) {
      NLENGTH left(0, 0, 0), top(0, 0, 0), right(0, 0, 0), bottom(0, 0, 0);
      UINT monitor = 0;

      if (!nCore::ParseMonitor(monitorTok, &monitor)) {
        gLogger->Warning(L"Invalid monitor %s in *nDeskWorkArea %s", monitorTok, line);
        return;
      }
      if (!nCore::ParseLength(leftTok, &left)) {
        gLogger->Warning(L"Invalid left padding %s in *nDeskWorkArea %s", leftTok, line);
        return;
      }
      if (!nCore::ParseLength(topTok, &top)) {
        gLogger->Warning(L"Invalid top padding %s in *nDeskWorkArea %s", topTok, line);
        return;
      }
      if (!nCore::ParseLength(rightTok, &right)) {
        gLogger->Warning(L"Invalid right padding %s in *nDeskWorkArea %s", rightTok, line);
        return;
      }
      if (!nCore::ParseLength(bottomTok, &bottom)) {
        gLogger->Warning(L"Invalid bottom padding %s in *nDeskWorkArea %s", bottomTok, line);
        return;
      }

      if (monitor == 0xFFFFFFFF) {
        for (UINT i = 0; i < nCore::GetDisplays()->Count(); ++i) {
          const Display &display = nCore::GetDisplays()->GetDisplay(i);
          RECT workArea = {
            display.rect.left + (LONG)left.Evaluate((float)display.width, display.dpi.x),
            display.rect.top + (LONG)top.Evaluate((float)display.height, display.dpi.y),
            display.rect.right - (LONG)right.Evaluate((float)display.width, display.dpi.x),
            display.rect.bottom - (LONG)bottom.Evaluate((float)display.height, display.dpi.y)
          };
          SystemParametersInfoW(SPI_SETWORKAREA, 1, &workArea, 0);
        }
      } else if (monitor < nCore::GetDisplays()->Count()) {
        const Display &display = nCore::GetDisplays()->GetDisplay(monitor);
        RECT workArea = {
          display.rect.left + (LONG)left.Evaluate((float)display.width, display.dpi.x),
          display.rect.top + (LONG)top.Evaluate((float)display.height, display.dpi.y),
          display.rect.right - (LONG)right.Evaluate((float)display.width, display.dpi.x),
          display.rect.bottom - (LONG)bottom.Evaluate((float)display.height, display.dpi.y)
        };
        SystemParametersInfoW(SPI_SETWORKAREA, 1, &workArea, 0);
      } else {
        gLogger->Warning(L"Invalid monitor %s in *nDeskWorkArea %s", monitorTok, line);
      }
    } else {
      gLogger->Warning(L"Malformatted workarea declaration %s", monitorTok, line);
    }
  }, 0);
  SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETWORKAREA, 0);
}


void ReloadWorkareas() {
  for (UINT i = 0; i < nCore::GetDisplays()->Count(); ++i) {
    nCore::GetDisplays()->GetDisplay(i).rect;
    SystemParametersInfoW(SPI_SETWORKAREA, 1,
      const_cast<PRECT>(&nCore::GetDisplays()->GetDisplay(i).rect), 0);
  }
  LoadWorkareas();
}


void ClearWorkareas() {
  for (UINT i = 0; i < nCore::GetDisplays()->Count(); ++i) {
    nCore::GetDisplays()->GetDisplay(i).rect;
    SystemParametersInfoW(SPI_SETWORKAREA, 1,
      const_cast<PRECT>(&nCore::GetDisplays()->GetDisplay(i).rect), 0);
  }
  SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETWORKAREA, 0);
}
