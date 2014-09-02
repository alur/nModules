//-------------------------------------------------------------------------------------------------
// /nTray/TrayIcon.cpp
// The nModules Project
//
// Implementation of the TrayIcon class. A single icon in the tray.
//-------------------------------------------------------------------------------------------------
#include "Tray.hpp"
#include "TrayIcon.hpp"

#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"

#include <shellapi.h>
#include <strsafe.h>
#include <Windowsx.h>

extern LSModule gLSModule;


/// <summary>
/// Constructor
/// </summary>
TrayIcon::TrayIcon(Drawable *parent, IconData &iconData, WindowSettings &wndSettings,
  StateRender<States> *stateRender)
  : Drawable(parent, L"Icon")
  , mIconData(iconData)
  , mIconSize(mSettings->GetInt(L"Size", 16))
  , mShowingTip(false)
{
  mWindow->Initialize(wndSettings, stateRender);
}


/// <summary>
/// Shows the trayicon.
/// </summary>
void TrayIcon::Show() {
  mWindow->Show();
}


/// <summary>
/// Handles NIM_MODIFY.
/// </summary>
void TrayIcon::HandleModify(LiteStep::LPLSNOTIFYICONDATA pNID) {
  if ((pNID->uFlags & NIF_ICON) == NIF_ICON) {
    UpdateIcon();
  }
  if ((pNID->uFlags & NIF_TIP) == NIF_TIP) {
    if (mShowingTip) {
      if (mIconData.tip[0] != L'\0') {
        RECT r;
        mWindow->GetScreenRect(&r);
        ((Tray*)mParent)->ShowTip(mIconData.tip, &r);
      } else {
        ((Tray*)mParent)->HideTip();
      }
    }
  }

  // TODO::NIF_STATE

  if ((pNID->uFlags & NIF_INFO) == NIF_INFO) {
    // uTimeout is only valid on 2000 and XP, so we can safely ignore it.
    if (*pNID->szInfo != L'\0' || *pNID->szInfoTitle != L'\0') {
      ((Tray*)mParent)->EnqueueBalloon(this, pNID->szInfoTitle, pNID->szInfo, pNID->dwInfoFlags,
        pNID->hBalloonIcon, (pNID->uFlags & NIF_REALTIME) == NIF_REALTIME);
    }
  }
}


/// <summary>
/// Updates the icon.
/// </summary>
void TrayIcon::UpdateIcon() {
  mWindow->ClearOverlays();
  D2D1_RECT_F f = D2D1::RectF(0.0f, 0.0f, (float)mIconSize, (float)mIconSize);
  mIconOverlay = mWindow->AddOverlay(f, mIconData.icon);
  mWindow->Repaint();
}


/// <summary>
/// Repositions the icon.
/// </summary>
void TrayIcon::Reposition(RECT rect) {
  mWindow->SetPosition(rect);
}


/// <summary>
/// Repositions the icon.
/// </summary>
void TrayIcon::Reposition(UINT x, UINT y, UINT width, UINT height) {
  mWindow->SetPosition((float)x, (float)y, (float)width, (float)height);
}


/// <summary>
/// Gets the screen coordinate rect of this tray icon.
/// </summary>
void TrayIcon::GetScreenRect(LPRECT rect) {
  mWindow->GetScreenRect(rect);
}


/// <summary>
/// Sends a message to the owner of the icon.
/// </summary>
void TrayIcon::SendCallback(UINT message, WPARAM /* wParam */, LPARAM /* lParam */) {
  WPARAM wParam;
  LPARAM lParam;
  if (mIconData.version >= NOTIFYICON_VERSION_4) {
    RECT r;
    mWindow->GetScreenRect(&r);
    wParam = MAKEWPARAM(r.left, r.top);
    lParam = MAKELPARAM(message, mIconData.id);
  } else {
    wParam = WPARAM(mIconData.id);
    lParam = LPARAM(message);
  }
  PostMessage(mIconData.window, mIconData.callbackMessage, wParam, lParam);
}


/// <summary>
/// Handles window messages for the icon's window.
/// </summary>
LRESULT WINAPI TrayIcon::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID) {
  if (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST) {
    if (message == WM_MOUSEMOVE && !mShowingTip) {
      if (!IsWindow(mIconData.window)) {
        // The icon went away...
        LiteStep::LSNOTIFYICONDATA lsNID;
        lsNID.cbSize = sizeof(lsNID);
        lsNID.hWnd = mIconData.window;
        lsNID.uID = mIconData.id;
        lsNID.guidItem = mIconData.guid;
        lsNID.uFlags = mIconData.flags;
        SendMessage(gLSModule.GetMessageWindow(), LM_SYSTRAY, NIM_DELETE, (LPARAM)&lsNID);
        return 0;
      }

      mShowingTip = true;
      if (mIconData.tip[0] != L'\0') {
        RECT r;
        mWindow->GetScreenRect(&r);
        ((Tray*)mParent)->ShowTip(mIconData.tip, &r);
      }
    }

    if (message == WM_RBUTTONDOWN || message == WM_LBUTTONDOWN) {
      ((Tray*)mParent)->HideTip();
    }

    AllowSetForegroundWindow(mIconData.processId);

    if (mIconData.version >= NOTIFYICON_VERSION_4) {
      if (message == WM_RBUTTONUP) {
        SendCallback(WM_CONTEXTMENU, wParam, lParam);
      } else if (message == WM_LBUTTONUP) {
        SendCallback(NIN_SELECT, wParam, lParam);
      } else {
        SendCallback(message, wParam, lParam);
      }
    } else {
      SendCallback(message, wParam, lParam);
    }
  } else if (message == WM_MOUSELEAVE) {
    ((Tray*)mParent)->HideTip();
    mShowingTip = false;
  } else {
    return DefWindowProc(window, message, wParam, lParam);
  }
  return 0;
}
