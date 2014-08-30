//-------------------------------------------------------------------------------------------------
// /nTray/TrayManager.cpp
// The nModules Project
//
// Keeps track of the system tray icons and notifies the trays of changes.
//-------------------------------------------------------------------------------------------------
#include "Tray.hpp"
#include "TrayManager.h"
#include "Types.h"

#include "../nShared/LiteStep.h"

#include "../Utilities/Process.h"

#include <shellapi.h>
#include <Shlwapi.h>
#include <vector>

using std::vector;
using TrayManager::IconIter;

extern TrayMap gTrays;

static vector<TrayManager::Icon> sCurrentIcons;


/// <summary>
/// Stops the tray manager. 
/// </summary>
void TrayManager::Stop() {
  sCurrentIcons.clear();
}


/// <summary>
/// Gets the screen rect of an icon.
/// </summary>
void TrayManager::GetScreenRect(IconIter icon, LPRECT rect) {
  if (icon->trayIcons.size() > 0) {
    icon->trayIcons[0]->GetScreenRect(rect);
  } else {
    // We could define a rectangle for icons that arent included anywhere, instead of just zeroing.
    ZeroMemory(rect, sizeof(RECT));
  }
}


/// <summary>
/// Finds a matching icon.
/// </summary>
IconIter TrayManager::FindIcon(GUID guid) {
  for (IconIter iter = sCurrentIcons.begin(); iter != sCurrentIcons.end(); ++iter) {
    if (iter->data.guidItem == guid) {
      return iter;
    }
  }
  return sCurrentIcons.end();
}


/// <summary>
/// Finds a matching icon.
/// </summary>
IconIter TrayManager::FindIcon(HWND hWnd, UINT uID) {
  for (IconIter iter = sCurrentIcons.begin(); iter != sCurrentIcons.end(); ++iter) {
    if (iter->data.hwnd == hWnd && iter->data.uID == uID) {
      return iter;
    }
  }
  return sCurrentIcons.end();
}


/// <summary>
/// Finds a matching icon.
/// </summary>
IconIter TrayManager::FindIcon(LiteStep::LPLSNOTIFYICONDATA pNID) {
  // There are 2 ways to identify an icon. Same guidItem, or same HWND and same uID.
  if ((pNID->uFlags & NIF_GUID) == NIF_GUID) {
    // uID & hWnd is ignored if guidItem is set
    return FindIcon(pNID->guidItem);
  }
  return FindIcon(pNID->hWnd, pNID->uID);
}


/// <summary>
/// Finds a matching icon in g_currentIcons.
/// </summary>
IconIter TrayManager::FindIcon(LiteStep::LPSYSTRAYINFOEVENT pSTE) {
  // There are 2 ways to identify an icon. Same guidItem, or same HWND and same uID.
  IconIter ret = FindIcon(pSTE->guidItem);
  if (ret == sCurrentIcons.end()) {
    ret = FindIcon(pSTE->hWnd, pSTE->uID);
  }
  return ret;
}


/// <summary>
/// Adds the specified icon to the trays, if it isn't already added.
/// </summary>
void TrayManager::AddIcon(LiteStep::LPLSNOTIFYICONDATA pNID) {
  if (FindIcon(pNID) == sCurrentIcons.end()) {
    sCurrentIcons.emplace_back();
    Icon &iconData = sCurrentIcons.back();
    iconData.data.hwnd = pNID->hWnd;
    iconData.data.uID = pNID->uID;
    if ((NIF_GUID & pNID->uFlags) == NIF_GUID) {
      iconData.data.guidItem = pNID->guidItem;
    } else {
      iconData.data.guidItem = GUID_NULL;
    }

    for (TrayMap::value_type &tray : gTrays) {
      TrayIcon *icon = tray.second.AddIcon(pNID);
      if (icon != nullptr) {
        iconData.trayIcons[&tray.second] = icon;
      }
    }
  }
}


/// <summary>
/// Deletes the specified icon from all trays, if it exists.
/// </summary>
void TrayManager::DeleteIcon(LiteStep::LPLSNOTIFYICONDATA pNID) {
  IconIter iconData = FindIcon(pNID);
  if (iconData != sCurrentIcons.end()) {
    for (auto icon : iconData->trayIcons) {
      icon.first->RemoveIcon(icon.second);
    }
    sCurrentIcons.erase(iconData);
  }
}


/// <summary>
/// Modifies an existing icon.
/// </summary>
void TrayManager::ModifyIcon(LiteStep::LPLSNOTIFYICONDATA pNID) {
  IconIter iconData = FindIcon(pNID);
  if (iconData != sCurrentIcons.end()) {
    for (auto icon : iconData->trayIcons) {
      icon.second->HandleModify(pNID);
    }
  } else {
    TRACE("Tried to modify non-existing icon");
  }
}


/// <summary>
/// Returns the focus to one of the trays.
/// </summary>
void TrayManager::SetFocus(LiteStep::LPLSNOTIFYICONDATA /* pNID */) {
}


/// <summary>
/// Changes the version of an existing tray icon.
/// </summary>
void TrayManager::SetVersion(LiteStep::LPLSNOTIFYICONDATA pNID) {
  IconIter iconData = FindIcon(pNID);
  if (iconData != sCurrentIcons.end()) {
    for (auto icon : iconData->trayIcons) {
      icon.second->HandleSetVersion(pNID);
    }
  }
}


/// <summary>
/// Handles LiteStep shell messages.
/// <summary>
LRESULT TrayManager::ShellMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
  case LM_SYSTRAY:
    switch ((DWORD)wParam)
    {
    case NIM_ADD:
      AddIcon((LiteStep::LPLSNOTIFYICONDATA)lParam);
      break;
            
    case NIM_DELETE:
      DeleteIcon((LiteStep::LPLSNOTIFYICONDATA)lParam);
      break;
            
    case NIM_MODIFY:
      ModifyIcon((LiteStep::LPLSNOTIFYICONDATA)lParam);
      break;
            
    case NIM_SETFOCUS:
      SetFocus((LiteStep::LPLSNOTIFYICONDATA)lParam);
      break;
            
    case NIM_SETVERSION:
      SetVersion((LiteStep::LPLSNOTIFYICONDATA)lParam);
      break;

    default:
      TRACE("TrayManager::Unknown LM_SYSTRAY wParam: %u", wParam);
      return FALSE;
    }
    return TRUE;

  case LM_SYSTRAYINFOEVENT: {
      LiteStep::LPSYSTRAYINFOEVENT lpSTE = (LiteStep::LPSYSTRAYINFOEVENT)wParam;
      IconIter icon = FindIcon(lpSTE);
      if (icon == sCurrentIcons.end()) {
        return FALSE;
      }

      RECT r;
      GetScreenRect(icon, &r);

      switch (lpSTE->dwEvent) {
      case TRAYEVENT_GETICONPOS:
        *(LRESULT*)lParam = MAKELPARAM(r.left, r.top);
        break;

      case TRAYEVENT_GETICONSIZE:
        *(LRESULT*)lParam = MAKELPARAM(r.right - r.left, r.bottom - r.top);
        break;

      default:
        TRACE("TrayManager::Unknown LM_SYSTRAYINFOEVENT: %u", lpSTE->dwEvent);
        return FALSE;
      }
    }
    return TRUE;
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// <summary>
/// Called when the init phase has ended.
/// <summary>
void TrayManager::InitCompleted() {
  for (auto &tray : gTrays) {
    tray.second.InitCompleted();
  }
}


/// <summary>
/// Lists identifying information about all tray icons
/// <summary>
void TrayManager::ListIconIDS() {
  WCHAR iconIDs[32768];
  iconIDs[0] = L'\0';
  for (auto &iconData : sCurrentIcons) {
    WCHAR buffer[MAX_PATH];

    if (SUCCEEDED(GetProcessName(iconData.data.hwnd, false, buffer, _countof(buffer)))) {
      StringCchCatW(iconIDs, _countof(iconIDs), L"Process Name: \t");
      StringCchCatW(iconIDs, _countof(iconIDs), buffer);
    }

    if (GetClassNameW(iconData.data.hwnd, buffer, _countof(buffer)) != 0) {
      StringCchCatW(iconIDs, _countof(iconIDs), L"\nWindow Class: \t");
      StringCchCatW(iconIDs, _countof(iconIDs), buffer);
    }

    if (GetWindowTextW(iconData.data.hwnd, buffer, _countof(buffer)) != 0) {
      StringCchCatW(iconIDs, _countof(iconIDs), L"\nWindow Text: \t");
      StringCchCatW(iconIDs, _countof(iconIDs), buffer);
    }

    if (iconData.data.guidItem != GUID_NULL) {
      StringFromGUID2(iconData.data.guidItem, buffer, _countof(buffer));
      StringCchCatW(iconIDs, _countof(iconIDs), L"\nGUID: \t\t");
      StringCchCatW(iconIDs, _countof(iconIDs), buffer);
    }

    StringCchCatW(iconIDs, _countof(iconIDs),
      L"\n------------------------------------------------------------------------\n");
  }
  MessageBoxW(nullptr, iconIDs, L"List of tray icons", MB_OK | MB_ICONINFORMATION);
}
