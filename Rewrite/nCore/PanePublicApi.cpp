#include "Displays.hpp"
#include "Pane.hpp"

#include "../nUtilities/lsapi.h"
#include "../nUtilities/Macros.h"

extern Displays gDisplays;


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


float Pane::EvaluateLength(const NLENGTH &length, bool horizontal) const {
  return length.Evaluate(horizontal ? mSize.width : mSize.height, horizontal ? mDpi.x : mDpi.y);
}


LPVOID Pane::GetPainterData() const {
  return mPainterData;
}


const D2D1_RECT_F *Pane::GetRenderingPosition() const {
  return &mRenderingPosition;
}


LPCWSTR Pane::GetRenderingText() const {
  return mText;
}


bool Pane::GetScreenPosition(D2D1_RECT_F *rect) const {
  const Pane *topMost = this;
  for (; topMost && topMost->IsChildPane(); topMost = topMost->mParent);
  if (topMost) {
    RECT r;
    if (GetWindowRect(topMost->mWindow, &r)) {
      *rect = mRenderingPosition;
      rect->left += (float)r.left;
      rect->right += (float)r.left;
      rect->top += (float)r.top;
      rect->bottom += (float)r.top;
      return true;
    }
  }
  return false;
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
    D2D1_RECT_F newPosition = D2D1::RectF(
      EvaluateLengthParent(mSettings.position.left, true) + mParent->mRenderingPosition.left,
      EvaluateLengthParent(mSettings.position.top, false) + mParent->mRenderingPosition.top,
      EvaluateLengthParent(mSettings.position.right, true) + mParent->mRenderingPosition.left,
      EvaluateLengthParent(mSettings.position.bottom, false) + mParent->mRenderingPosition.top);

    D2D1_SIZE_F newSize = D2D1::SizeF(
      newPosition.right - newPosition.left,
      newPosition.bottom - newPosition.top);

    bool isMove = newPosition.left != mRenderingPosition.left
      || newPosition.top != mRenderingPosition.top;
    bool isSize = newSize.width != mSize.width || newSize.height != mSize.height;

    if (!isMove && !isSize) {
      return;
    }

    Repaint(false); // Invalidate where we used to be
    mRenderingPosition = newPosition;
    mSize = newSize;
    mPainter->PositionChanged(this, mPainterData, mRenderingPosition, isMove, isSize);
    Repaint(true); // And where we are now
  } else {
    //D2D1_RECT_U oldPosition = mWindowPosition;
    //SetWindowPos(mWindow, 0, int());
    mSize = D2D1::SizeF(
      mRenderingPosition.right - mRenderingPosition.left,
      mRenderingPosition.bottom - mRenderingPosition.top);
  }
}


void Pane::Repaint(LPCNRECT area) {
  if (mVisible) {
    if (area == nullptr) {
      Repaint(mRenderingPosition, true);
    } else {
      D2D1_RECT_F position = D2D1::RectF(
        EvaluateLength(area->left, true) + mRenderingPosition.left,
        EvaluateLength(area->top, false) + mRenderingPosition.top,
        EvaluateLength(area->right, true) + mRenderingPosition.left,
        EvaluateLength(area->bottom, false) + mRenderingPosition.top);
      Repaint(position, true);
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
  Repaint(true);
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
    RepaintInvalidated();
  }
}
