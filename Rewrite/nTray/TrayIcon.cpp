#include "TrayIcon.hpp"

#include "../nCoreApi/Core.h"

#include "../Headers/Macros.h"

#include <shellapi.h>
//#include <strsafe.h>
//#include <Windowsx.h>


TrayIcon::TrayIcon(IPane *parent, IPainter *painter, IconData &data)
  : mData(data)
{
  mIconPainter = nCore::CreateImagePainter();
  mIconPainter->SetImage(data.icon);

  PaneInitData initData;
  ZeroMemory(&initData, sizeof(PaneInitData));
  initData.cbSize = sizeof(PaneInitData);
  initData.messageHandler = this;
  IPainter *painters[] = { painter, mIconPainter };
  initData.painters = painters;
  initData.numPainters = _countof(painters);
  mPane = parent->CreateChild(&initData);
}


TrayIcon::~TrayIcon() {
  mPane->Discard();
  mIconPainter->Discard();
}


bool TrayIcon::GetScreenRect(LPRECT rect) {
  D2D1_RECT_F r;
  if (mPane->GetScreenPosition(&r)) {
    rect->left = (LONG)r.left;
    rect->top = (LONG)r.top;
    rect->right = (LONG)r.right;
    rect->bottom = (LONG)r.bottom;
    return true;
  }
  return false;
}


void TrayIcon::Modify(LPLSNOTIFYICONDATA nid) {
  if (CHECKFLAG(nid->uFlags, NIF_ICON)) {
    mIconPainter->SetImage(nid->hIcon);
    mPane->Repaint(nullptr);
  }
}


void TrayIcon::Position(const NRECT &position) {
  mPane->Position(&position);
}


void TrayIcon::Show() {
  mPane->Show();
}


void TrayIcon::SendCallback(UINT message, WPARAM /* wParam */, LPARAM /* lParam */) {
  WPARAM wParam;
  LPARAM lParam;
  if (mData.version >= NOTIFYICON_VERSION_4) {
    D2D1_RECT_F r;
    mPane->GetScreenPosition(&r);
    wParam = MAKEWPARAM(r.left, r.top);
    lParam = MAKELPARAM(message, mData.id);
  } else {
    wParam = WPARAM(mData.id);
    lParam = LPARAM(message);
  }
  PostMessage(mData.window, mData.callbackMessage, wParam, lParam);
}


LRESULT TrayIcon::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, NPARAM) {
  if (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST) {
    AllowSetForegroundWindow(mData.processId);

    if (mData.version >= NOTIFYICON_VERSION) {
      SendCallback(message, wParam, lParam);
      if (message == WM_RBUTTONUP) {
        SendCallback(WM_CONTEXTMENU, wParam, lParam);
      } else if (message == WM_LBUTTONUP) {
        SendCallback(NIN_SELECT, wParam, lParam);
      }
    } else {
      SendCallback(message, wParam, lParam);
    }
  }
  return DefWindowProc(window, message, wParam, lParam);
};
