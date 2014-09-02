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

#include <list>
#include <map>
#include <shellapi.h>
#include <Shlwapi.h>

using LiteStep::LPLSNOTIFYICONDATA;

struct Icon {
  IconData data;
  std::map<Tray*, TrayIcon*> instances;
};

extern TrayMap gTrays;

static std::list<Icon> sCurrentIcons;
typedef decltype(sCurrentIcons.begin()) IconIterator;


/// <summary>
/// Gets the screen rect of an icon.
/// </summary>
static void GetScreenRect(IconIterator icon, LPRECT rect) {
  if (icon->instances.size() > 0) {
    // TODO(Erik): Lets pick which one we return based on which tray last had focus?
    icon->instances.begin()->second->GetScreenRect(rect);
  } else {
    // We could define a rectangle for icons that arent included anywhere, instead of just zeroing.
    ZeroMemory(rect, sizeof(RECT));
  }
}


/// <summary>
/// Finds a matching icon.
/// </summary>
static IconIterator FindIcon(GUID guid) {
  for (IconIterator iter = sCurrentIcons.begin(); iter != sCurrentIcons.end(); ++iter) {
    if (iter->data.guid == guid) {
      return iter;
    }
  }
  return sCurrentIcons.end();
}


/// <summary>
/// Finds a matching icon.
/// </summary>
static IconIterator FindIcon(HWND hWnd, UINT uID) {
  for (IconIterator iter = sCurrentIcons.begin(); iter != sCurrentIcons.end(); ++iter) {
    if (iter->data.window == hWnd && iter->data.id == uID) {
      return iter;
    }
  }
  return sCurrentIcons.end();
}


/// <summary>
/// Finds a matching icon.
/// </summary>
static IconIterator FindIcon(LPLSNOTIFYICONDATA pNID) {
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
static IconIterator FindIcon(LiteStep::LPSYSTRAYINFOEVENT pSTE) {
  // There are 2 ways to identify an icon. Same guidItem, or same HWND and same uID.
  IconIterator ret = FindIcon(pSTE->guidItem);
  if (ret == sCurrentIcons.end()) {
    ret = FindIcon(pSTE->hWnd, pSTE->uID);
  }
  return ret;
}


static void UpdateIconData(IconData &iconData, LPLSNOTIFYICONDATA pNID) {
  if ((pNID->uFlags & NIF_MESSAGE) == NIF_MESSAGE) {
    iconData.callbackMessage = pNID->uCallbackMessage;
    iconData.flags &= NIF_MESSAGE;
  }
  if ((pNID->uFlags & NIF_ICON) == NIF_ICON) {
    iconData.icon = pNID->hIcon;
    iconData.flags &= NIF_ICON;
  }
  if ((pNID->uFlags & NIF_TIP) == NIF_TIP) {
    StringCchCopy(iconData.tip, TRAY_MAX_TIP_LENGTH, pNID->szTip);
    iconData.flags &= NIF_TIP;
  }
  if ((iconData.flags & NIF_GUID) != NIF_GUID && (pNID->uFlags & NIF_GUID) == NIF_GUID) {
    iconData.guid = pNID->guidItem;
    iconData.flags &= NIF_GUID;
  }
}


/// <summary>
/// Adds the specified icon to the trays, if it isn't already added.
/// </summary>
static void AddIcon(LPLSNOTIFYICONDATA pNID) {
  if (FindIcon(pNID) == sCurrentIcons.end()) {
    sCurrentIcons.emplace_back();
    Icon &icon = sCurrentIcons.back();
    icon.data.window = pNID->hWnd;
    icon.data.id = pNID->uID;
    GetWindowThreadProcessId(pNID->hWnd, &icon.data.processId);
    UpdateIconData(icon.data, pNID);

    for (TrayMap::value_type &tray : gTrays) {
      TrayIcon *instance = tray.second.AddIcon(icon.data);
      if (instance != nullptr) {
        icon.instances[&tray.second] = instance;
        instance->HandleModify(pNID);
      }
    }
  }
}


/// <summary>
/// Deletes the specified icon from all trays, if it exists.
/// </summary>
static void DeleteIcon(LPLSNOTIFYICONDATA pNID) {
  IconIterator icon = FindIcon(pNID);
  if (icon != sCurrentIcons.end()) {
    for (auto instance : icon->instances) {
      instance.first->RemoveIcon(instance.second);
    }
    sCurrentIcons.erase(icon);
  }
}


/// <summary>
/// Modifies an existing icon.
/// </summary>
static void ModifyIcon(LPLSNOTIFYICONDATA pNID) {
  IconIterator icon = FindIcon(pNID);
  if (icon != sCurrentIcons.end()) {
    UpdateIconData(icon->data, pNID);
    for (auto instance : icon->instances) {
      instance.second->HandleModify(pNID);
    }
  } else {
    TRACE("Tried to modify non-existing icon");
    // TODO(Erik): Figure out what explorer does in this case.
  }
}


/// <summary>
/// Returns the focus to one of the trays.
/// </summary>
static void SetFocus(LPLSNOTIFYICONDATA /* pNID */) {
}


/// <summary>
/// Changes the version of an existing tray icon.
/// </summary>
static void SetVersion(LPLSNOTIFYICONDATA pNID) {
  IconIterator icon = FindIcon(pNID);
  if (icon != sCurrentIcons.end()) {
    icon->data.version = pNID->uVersion;
  }
}


/// <summary>
/// Stops the tray manager. 
/// </summary>
void TrayManager::Stop() {
  sCurrentIcons.clear();
}


/// <summary>
/// Handles LiteStep shell messages.
/// <summary>
LRESULT TrayManager::ShellMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
  case LM_SYSTRAY:
    switch ((DWORD)wParam) {
    case NIM_ADD:
      AddIcon((LPLSNOTIFYICONDATA)lParam);
      break;

    case NIM_DELETE:
      DeleteIcon((LPLSNOTIFYICONDATA)lParam);
      break;

    case NIM_MODIFY:
      ModifyIcon(LPLSNOTIFYICONDATA(lParam));
      break;

    case NIM_SETFOCUS:
      SetFocus((LPLSNOTIFYICONDATA)lParam);
      break;

    case NIM_SETVERSION:
      SetVersion((LPLSNOTIFYICONDATA)lParam);
      break;

    default:
      TRACE("TrayManager::Unknown LM_SYSTRAY wParam: %u", wParam);
      return FALSE;
    }
    return TRUE;

  case LM_SYSTRAYINFOEVENT: {
      LiteStep::LPSYSTRAYINFOEVENT lpSTE = (LiteStep::LPSYSTRAYINFOEVENT)wParam;
      IconIterator icon = FindIcon(lpSTE);
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

    if (SUCCEEDED(GetProcessName(iconData.data.window, false, buffer, _countof(buffer)))) {
      StringCchCatW(iconIDs, _countof(iconIDs), L"Process Name: \t");
      StringCchCatW(iconIDs, _countof(iconIDs), buffer);
    }

    if (GetClassNameW(iconData.data.window, buffer, _countof(buffer)) != 0) {
      StringCchCatW(iconIDs, _countof(iconIDs), L"\nWindow Class: \t");
      StringCchCatW(iconIDs, _countof(iconIDs), buffer);
    }

    if (GetWindowTextW(iconData.data.window, buffer, _countof(buffer)) != 0) {
      StringCchCatW(iconIDs, _countof(iconIDs), L"\nWindow Text: \t");
      StringCchCatW(iconIDs, _countof(iconIDs), buffer);
    }

    if (iconData.data.guid != GUID_NULL) {
      StringFromGUID2(iconData.data.guid, buffer, _countof(buffer));
      StringCchCatW(iconIDs, _countof(iconIDs), L"\nGUID: \t\t");
      StringCchCatW(iconIDs, _countof(iconIDs), buffer);
    }

    StringCchCatW(iconIDs, _countof(iconIDs),
      L"\n------------------------------------------------------------------------\n");
  }
  MessageBoxW(nullptr, iconIDs, L"List of tray icons", MB_OK | MB_ICONINFORMATION);
}
