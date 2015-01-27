#include "Workarea.h"

#include "../nCoreApi/Core.h"

#include "../nUtilities/lsapi.h"

#include <algorithm>


void LoadWorkareas() {
  nCore::EnumRCLines(L"*nDeskWorkArea", [] (LPCWSTR line, LPARAM) -> void {
    wchar_t monitorTok[16], leftTok[16], topTok[16], rightTok[16], bottomTok[16];
    LPWSTR tokens[] = { monitorTok, leftTok, topTok, rightTok, bottomTok };

    if (LCTokenize(line, tokens, 5, nullptr) == 5) {
      int left = std::max(0, _wtoi(leftTok)),
          top = std::max(0, _wtoi(topTok)),
          right = std::max(0, _wtoi(rightTok)),
          bottom = std::max(0, _wtoi(bottomTok));
      // TODO(Erik):ParseMonitor
      UINT monitor = std::max(-1, _wtoi(monitorTok));

      if (monitor == 0xFFFFFFFF) {
        for (UINT i = 0; i < nCore::GetDisplays()->Count(); ++i) {
          RECT monitorRect = nCore::GetDisplays()->GetDisplay(i).rect;
          RECT workArea = { monitorRect.left + left, monitorRect.top + top,
            monitorRect.right - right, monitorRect.bottom - bottom };
          SystemParametersInfoW(SPI_SETWORKAREA, 1, &workArea, 0);
        }
      } else if (monitor < nCore::GetDisplays()->Count()) {
        RECT monitorRect = nCore::GetDisplays()->GetDisplay(monitor).rect;
        RECT workArea = { monitorRect.left + left, monitorRect.top + top,
          monitorRect.right - right, monitorRect.bottom - bottom };
        SystemParametersInfoW(SPI_SETWORKAREA, 1, &workArea, 0);
      }
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
