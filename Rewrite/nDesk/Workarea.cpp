#include "Workarea.h"

#include "../nModuleBase/nModule.hpp"

#include "../nCoreApi/Core.h"

#include "../Headers/lsapi.h"

#include <algorithm>
#include <vector>

extern NModule gModule;


void LoadWorkareas() {
  std::vector<RECT> newWorkAreas(nCore::GetDisplays()->Count());
  for (UINT i = 0; i < nCore::GetDisplays()->Count(); ++i) {
    newWorkAreas[i] = nCore::GetDisplays()->GetDisplay(i).rect;
  }

  nCore::EnumRCLines(L"*nDeskWorkArea", [] (LPCWSTR line, LPARAM pNewWorkAreas) -> void {
    LPRECT newWorkAreas = (LPRECT)pNewWorkAreas;
    wchar_t monitorTok[16], leftTok[16], topTok[16], rightTok[16], bottomTok[16];
    LPWSTR tokens[] = { monitorTok, leftTok, topTok, rightTok, bottomTok };

    if (LCTokenize(line, tokens, 5, nullptr) == 5) {
      NLENGTH left(0, 0, 0), top(0, 0, 0), right(0, 0, 0), bottom(0, 0, 0);
      UINT monitor = 0;

      if (!nCore::ParseMonitor(monitorTok, &monitor)) {
        gModule.log->Warning(L"Invalid monitor %s in *nDeskWorkArea %s", monitorTok, line);
        return;
      }
      if (!nCore::ParseLength(leftTok, &left)) {
        gModule.log->Warning(L"Invalid left padding %s in *nDeskWorkArea %s", leftTok, line);
        return;
      }
      if (!nCore::ParseLength(topTok, &top)) {
        gModule.log->Warning(L"Invalid top padding %s in *nDeskWorkArea %s", topTok, line);
        return;
      }
      if (!nCore::ParseLength(rightTok, &right)) {
        gModule.log->Warning(L"Invalid right padding %s in *nDeskWorkArea %s", rightTok, line);
        return;
      }
      if (!nCore::ParseLength(bottomTok, &bottom)) {
        gModule.log->Warning(L"Invalid bottom padding %s in *nDeskWorkArea %s", bottomTok, line);
        return;
      }

      if (monitor == 0xFFFFFFFF) {
        for (UINT i = 0; i < nCore::GetDisplays()->Count(); ++i) {
          const Display &display = nCore::GetDisplays()->GetDisplay(i);
          newWorkAreas[i] = display.rect;
          newWorkAreas[i].left += (LONG)left.Evaluate((float)display.width, display.dpi.x);
          newWorkAreas[i].top += (LONG)top.Evaluate((float)display.height, display.dpi.y);
          newWorkAreas[i].right -= (LONG)right.Evaluate((float)display.width, display.dpi.x);
          newWorkAreas[i].bottom -= (LONG)bottom.Evaluate((float)display.height, display.dpi.y);
        }
      } else if (monitor < nCore::GetDisplays()->Count()) {
        const Display &display = nCore::GetDisplays()->GetDisplay(monitor);
        newWorkAreas[monitor] = display.rect;
        newWorkAreas[monitor].left += (LONG)left.Evaluate((float)display.width, display.dpi.x);
        newWorkAreas[monitor].top += (LONG)top.Evaluate((float)display.height, display.dpi.y);
        newWorkAreas[monitor].right -= (LONG)right.Evaluate((float)display.width, display.dpi.x);
        newWorkAreas[monitor].bottom -= (LONG)bottom.Evaluate((float)display.height,
          display.dpi.y);
      } else {
        gModule.log->Warning(L"Invalid monitor %s in *nDeskWorkArea %s", monitorTok, line);
      }
    } else {
      gModule.log->Warning(L"Malformatted workarea declaration %s", line);
    }
  }, (LPARAM)&newWorkAreas[0]);


  bool changesMade = false;
  for (UINT i = 0; i < nCore::GetDisplays()->Count(); ++i) {
    if (!EqualRect(&nCore::GetDisplays()->GetDisplay(i).workArea, &newWorkAreas[i])) {
      SystemParametersInfoW(SPI_SETWORKAREA, 1, const_cast<PRECT>(&newWorkAreas[i]), 0);
      changesMade = true;
    }
  }
  if (changesMade) {
    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETWORKAREA, 0);
  }
}


void ClearWorkareas() {
  bool changesMade = false;
  for (UINT i = 0; i < nCore::GetDisplays()->Count(); ++i) {
    const Display &display = nCore::GetDisplays()->GetDisplay(i);
    if (!EqualRect(&display.rect, &display.workArea)) {
      SystemParametersInfoW(SPI_SETWORKAREA, 1, const_cast<PRECT>(&display.rect), 0);
      changesMade = true;
    }
  }
  if (changesMade) {
    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETWORKAREA, 0);
  }
}
