#include "Messages.h"
#include "TrayManager.hpp"

#include "../Headers/Macros.h"

#include <assert.h>
#include <shellapi.h>
#include <strsafe.h>


TrayManager::TrayManager() : mInitialized(false) {}


TrayManager::~TrayManager() {
  mTrays.clear();
  mIcons.clear();
}


void TrayManager::CreateTray(LPCWSTR name) {
  auto ret = mTrays.emplace(
    std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(name));

  if (ret.second) {
    Tray &tray = ret.first->second;
    for (NotifyIcon &icon : mIcons) {
      if (tray.WantsIcon(icon.data)) {
        std::list<TrayIcon>::iterator instance = tray.AddIcon(icon.data);
        icon.instances.emplace_back(&tray, instance);
      }
    }
    if (mInitialized) {
      tray.Initialized();
    }
  }
}


void TrayManager::DestroyAllTrays() {
  mTrays.clear();
  for (NotifyIcon &icon : mIcons) {
    icon.instances.clear();
  }
}


void TrayManager::UpdateIconData(IconData &data, LPLSNOTIFYICONDATA nid) {
  if (CHECKFLAG(nid->uFlags, NIF_MESSAGE)) {
    data.callbackMessage = nid->uCallbackMessage;
    data.flags |= NIF_MESSAGE;
  }
  if (CHECKFLAG(nid->uFlags, NIF_ICON)) {
    data.icon = nid->hIcon;
    data.flags |= NIF_ICON;
  }
  if (CHECKFLAG(nid->uFlags, NIF_TIP)) {
    StringCchCopy(data.tip, TRAY_MAX_TIP_LENGTH, nid->szTip);
    data.flags |= NIF_TIP;
  }
  if (!CHECKFLAG(data.flags, NIF_GUID) && CHECKFLAG(nid->uFlags, NIF_GUID)) {
    data.guid = nid->guidItem;
    data.flags |= NIF_GUID;
  }
}


void TrayManager::AddIcon(LPLSNOTIFYICONDATA nid) {
  if (FindIcon(nid) == mIcons.end()) {
    mIcons.emplace_back();
    NotifyIcon &icon = mIcons.back();
    icon.data.window = nid->hWnd;
    icon.data.id = nid->uID;
    GetWindowThreadProcessId(nid->hWnd, &icon.data.processId);
    UpdateIconData(icon.data, nid);

    for (auto &tray : mTrays) {
      if (tray.second.WantsIcon(icon.data)) {
        std::list<TrayIcon>::iterator instance = tray.second.AddIcon(icon.data);
        icon.instances.emplace_back(&tray.second, instance);
        instance->Modify(nid);
      }
    }
  }
}


void TrayManager::DeleteIcon(LPLSNOTIFYICONDATA nid) {
  auto icon = FindIcon(nid);
  if (icon != mIcons.end()) {
    for (auto instance : icon->instances) {
      instance.first->RemoveIcon(instance.second);
    }
    mIcons.erase(icon);
  }
}


void TrayManager::ModifyIcon(LPLSNOTIFYICONDATA nid) {
  auto icon = FindIcon(nid);
  if (icon != mIcons.end()) {
    UpdateIconData(icon->data, nid);
    for (auto instance : icon->instances) {
      instance.second->Modify(nid);
    }
  }
}


void TrayManager::SetFocus(LPLSNOTIFYICONDATA) {
  // TODO(Erik): Implement.
}


void TrayManager::SetVersion(LPLSNOTIFYICONDATA nid) {
  auto icon = FindIcon(nid);
  if (icon != mIcons.end()) {
    icon->data.version = nid->uVersion;
  }
}


std::list<TrayManager::NotifyIcon>::iterator TrayManager::FindIcon(GUID guid) {
  for (std::list<NotifyIcon>::iterator iter = mIcons.begin(); iter != mIcons.end(); ++iter) {
    if (iter->data.guid == guid) {
      return iter;
    }
  }
  return mIcons.end();
}


std::list<TrayManager::NotifyIcon>::iterator TrayManager::FindIcon(HWND window, UINT id) {
  for (std::list<NotifyIcon>::iterator iter = mIcons.begin(); iter != mIcons.end(); ++iter) {
    if (iter->data.window == window && iter->data.id == id) {
      return iter;
    }
  }
  return mIcons.end();
}


std::list<TrayManager::NotifyIcon>::iterator TrayManager::FindIcon(LPLSNOTIFYICONDATA nid) {
  if (CHECKFLAG(nid->uFlags, NIF_GUID)) {
    // uID & hWnd are ignored if guidItem is set
    return FindIcon(nid->guidItem);
  }
  return FindIcon(nid->hWnd, nid->uID);
}


std::list<TrayManager::NotifyIcon>::iterator TrayManager::FindIcon(LPSYSTRAYINFOEVENT event) {
  std::list<NotifyIcon>::iterator ret = FindIcon(event->guidItem);
  if (ret == mIcons.end()) {
    ret = FindIcon(event->hWnd, event->uID);
  }
  return ret;
}


LRESULT TrayManager::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
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
      assert(false); // We should handle all LM_SYSTRAY message types.
      return FALSE;
    }
    return TRUE;

  case LM_SYSTRAYINFOEVENT: {
      LPSYSTRAYINFOEVENT event = (LPSYSTRAYINFOEVENT)wParam;
      std::list<NotifyIcon>::iterator icon = FindIcon(event);
      if (icon == mIcons.end() || icon->instances.empty()) {
        return FALSE;
      }

      RECT rect;
      if (!icon->instances.front().second->GetScreenRect(&rect)) {
        return FALSE;
      }

      switch (event->dwEvent) {
      case TRAYEVENT_GETICONPOS:
        *(LRESULT*)lParam = MAKELPARAM(rect.left, rect.top);
        break;

      case TRAYEVENT_GETICONSIZE:
        *(LRESULT*)lParam = MAKELPARAM(rect.right - rect.left, rect.bottom - rect.top);
        break;

      default:
        assert(false); // We should handle all LM_SYSTRAYINFOEVENT message types.
        return FALSE;
      }
    }
    return TRUE;

  case NTRAY_GOT_INITIAL_ICONS:
    mInitialized = true;
    for (auto &tray : mTrays) {
      tray.second.Initialized();
    }
    return 0;
  }
  return DefWindowProc(window, message, wParam, lParam);
}
