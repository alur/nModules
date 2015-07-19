#include "Pane.hpp"

#include "../nCoreApi/Messages.h"

#include "../Headers/Windows.h"

#include <assert.h>
#include <windowsx.h>


LRESULT WINAPI Pane::ExternWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  return ((Pane*)GetWindowLongPtr(window, 0))->HandleMessage(window, message, wParam, lParam, 0);
}


LRESULT WINAPI Pane::InitWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  if (message == WM_CREATE) {
    SetWindowLongPtr(window, 0, (LONG_PTR)((LPCREATESTRUCT)lParam)->lpCreateParams);
    SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)&Pane::ExternWindowProc);
    return ExternWindowProc(window, message, wParam, lParam);
  }
  return DefWindowProc(window, message, wParam, lParam);
}


LRESULT Pane::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, NPARAM) {
  // Forward mouse messages to the lowest level child window which the mouse is over.
  if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) {
    int xPos = GET_X_LPARAM(lParam);
    int yPos = GET_Y_LPARAM(lParam);

    IMessageHandler *handler = nullptr; // TODO(Erik): Mouse capture

    if (handler == nullptr) {
      for (Pane *child : mChildren) {
        if (child->mVisible && !child->mSettings.clickThrough) { // TODO(Erik): ClickThrough
          D2D1_RECT_F pos = child->mRenderingPosition;
          if (xPos >= pos.left && xPos <= pos.right && yPos >= pos.top && yPos <= pos.bottom) {
            handler = child;
            break;
          }
        }
      }
    }

    if (msg == WM_MOUSEMOVE && !mIsTrackingMouse && mWindow) {
      mIsTrackingMouse = true;
      TrackMouseEvent(&mTrackMouseEvent);
      mMessageHandler->HandleMessage(window, NCORE_WM_MOUSEENTER, wParam, lParam, 0);
    }

    if (handler != mActiveChild) {
      if (mActiveChild) {
        mActiveChild->HandleMessage(window, WM_MOUSELEAVE, 0, 0, 0);
      } else {
        mMessageHandler->HandleMessage(window, WM_MOUSEMOVE, wParam, lParam, 0);
      }
      mActiveChild = (Pane*)handler;
      if (handler) {
        handler->HandleMessage(window, NCORE_WM_MOUSEENTER, wParam, lParam, 0);
      }
    }

    // Just let our message handler deal with it.
    if (handler == nullptr) {
      handler = mMessageHandler;
    }

    return handler->HandleMessage(window, msg, wParam, lParam, 0);
  }

  switch (msg) {
  case WM_MOUSELEAVE:
    mIsTrackingMouse = false;
    if (mActiveChild) {
      mActiveChild->HandleMessage(window, WM_MOUSELEAVE, 0, 0, 0);
      mActiveChild = nullptr;
    }
    break;

  case WM_ERASEBKGND:
    return 1;

  case WM_NCPAINT:
    return 0;

  case WM_PAINT:
    {
      RECT updateRect;
      // The mVisible check is here because we continiously receieve WM_PAINT message for hidden
      // windows. Needs to be investigated.
      if (mVisible && GetUpdateRect(window, &updateRect, FALSE) != FALSE) {
        if (ReCreateDeviceResources() == S_OK) {
          D2D1_RECT_F d2dUpdateRect = D2D1::RectF((FLOAT)updateRect.left, (FLOAT)updateRect.top,
            (FLOAT)updateRect.right, (FLOAT)updateRect.bottom);

          mRenderTarget->BeginDraw();
          mRenderTarget->PushAxisAlignedClip(&d2dUpdateRect, D2D1_ANTIALIAS_MODE_ALIASED);
          mRenderTarget->Clear();
          Paint(mRenderTarget, &d2dUpdateRect);
          mRenderTarget->PopAxisAlignedClip();

          HRESULT hr = mRenderTarget->EndDraw();
          if (hr == D2DERR_RECREATE_TARGET) {
            DiscardDeviceResources();
            InvalidateRect(mWindow, nullptr, FALSE);
            UpdateWindow(mWindow);
          } else if (SUCCEEDED(hr)) {
            ValidateRect(window, &updateRect);
          } else {
            // TODO(Erik): Deal with this.
            assert(false);
          }
        }
      }
    }
    return 0;

  case WM_WINDOWPOSCHANGING:
    {
      if (mSettings.alwaysOnTop && !mCoveredByFullscreenWindow) {
        LPWINDOWPOS windowPos = LPWINDOWPOS(lParam);
        // TODO(Erik): Handle covered by fullscreen.
        windowPos->hwndInsertAfter = HWND_TOPMOST;
      }
    }
    break;
  }

  return mMessageHandler->HandleMessage(window, msg, wParam, lParam, (NPARAM)(IPane*)this);
}
