#include "Pane.hpp"

#include "../nUtilities/lsapi.h"
#include "../nUtilities/Macros.h"


EXPORT_CDECL(IPane*) CreatePane(const PaneInitData *initData) {
  return (IPane*)new Pane(initData, nullptr);
}


EXPORT_CDECL(IPane*) FindPane() {
  return nullptr;
}


IPane *Pane::CreateChild(const PaneInitData *initData) {
  Pane *pane = new Pane(initData, this);
  mChildren.insert(pane);
  return pane;
}


void Pane::Destroy() {
  delete this;
}


LPCWSTR Pane::GetRenderingText() const {
  return mText;
}


HWND Pane::GetWindow() const {
  return mWindow;
}


void Pane::Hide() {
  if (mVisible) {
    mVisible = false;
    if (!IsChildPane()) {
      ShowWindow(mWindow, SW_HIDE);
    } else if (mParent) {
      mParent->Repaint(mRenderingPosition, true);
    }
  }
}


void Pane::Lock() {
  ++mUpdateLock;
}


void Pane::PaintChildren(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area) const {
  for (const Pane* child : mChildren) {
    child->Paint(renderTarget, area);
  }
}


void Pane::Position(LPCNRECT position) {
  mSettings.position = *position;

  if (IsChildPane() && mParent) {
    mRenderingPosition = D2D1::RectF(
      EvaluateLength(mSettings.position.left, true) + mParent->mRenderingPosition.left,
      EvaluateLength(mSettings.position.top, false) + mParent->mRenderingPosition.top,
      EvaluateLength(mSettings.position.right, true) + mParent->mRenderingPosition.left,
      EvaluateLength(mSettings.position.bottom, false) + mParent->mRenderingPosition.top);
  } else {
    //D2D1_RECT_U oldPosition = mWindowPosition;
    //SetWindowPos(mWindow, 0, int());
  }
  mSize = D2D1::SizeF(
    mRenderingPosition.right - mRenderingPosition.left,
    mRenderingPosition.bottom - mRenderingPosition.top);
}


void Pane::Repaint(LPCNRECT area) {
  if (mVisible) {
    if (area == nullptr) {
      Repaint(mRenderingPosition, true);
    } else {
      Repaint(mRenderingPosition, true);
    }
  }
}


void Pane::SetText(LPCWSTR text) {
  if (mText) {
    free(mText);
  }
  if (text == nullptr) {
    mText = nullptr;
  } else {
    mText = _wcsdup(text);
  }
  mPainter->TextChanged(this, mPainterData, mText);
}


void Pane::Show() {
  if (!mVisible) {
    mVisible = true;
    if (!IsChildPane()) {
      ShowWindow(mWindow, SW_SHOWNOACTIVATE);
    } else {
      Repaint(true);
    }
  }
}


void Pane::Unlock() {
  if (--mUpdateLock == 0) {
    UpdateWindow(mWindow);
  }
}
