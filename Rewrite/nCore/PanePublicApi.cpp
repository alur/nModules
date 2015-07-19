#include "Displays.hpp"
#include "Pane.hpp"

#include "../Headers/lsapi.h"
#include "../Headers/Macros.h"

extern Displays gDisplays;


EXPORT_CDECL(IPane*) CreatePane(const PaneInitData *initData) {
  return (IPane*)new Pane(initData, nullptr);
}


EXPORT_CDECL(IPane*) FindPane() {
  return nullptr;
}


void Pane::ActivateState(BYTE state) {
  if (!mActiveStates[state]) {
    mActiveStates[state] = true;
    for (BYTE dependent : mStateDependents[state]) {
      bool activate = true;
      for (BYTE dependency : mStateDependencies[dependent]) {
        if (!mActiveStates[dependency]) {
          activate = false;
          break;
        }
      }
      if (activate) {
        ActivateState(dependent);
      }
    }
    if (state > mCurrentState) {
      mCurrentState = state;
      Repaint(true);
    }
  }
}


void Pane::ClearState(BYTE state) {
  if (mActiveStates[state]) {
    mActiveStates[state] = false;
    if (mCurrentState == state) {
      for (; state > 0 && !mActiveStates[state]; --state);
      mCurrentState = state;
      Repaint(true);
    }
    for (BYTE dependent : mStateDependents[state]) {
      ClearState(dependent);
    }
  }
}


IPane *Pane::CreateChild(const PaneInitData *initData) {
  Pane *pane = new Pane(initData, this);
  mChildren.insert(pane);
  return pane;
}


void Pane::Discard() {
  delete this;
}


float Pane::EvaluateLength(const NLENGTH &length, bool horizontal) const {
  return length.Evaluate(horizontal ? mSize.width : mSize.height, horizontal ? mDpi.x : mDpi.y);
}


D2D1_RECT_F Pane::EvaluateRect(const NRECT &rect) const {
  return D2D1::RectF(
    rect.left.Evaluate(mSize.width, mDpi.x),
    rect.top.Evaluate(mSize.height, mDpi.y),
    rect.right.Evaluate(mSize.width, mDpi.x),
    rect.bottom.Evaluate(mSize.height, mDpi.y));
}


LPVOID Pane::GetPainterData(const IPainter *painter) const {
  for (int i = 0; i < mPainters.size(); ++i) {
    if (mPainters[i] == painter) {
      return mPainterData[i];
    }
  }
  return nullptr;
}


const D2D1_RECT_F &Pane::GetRenderingPosition() const {
  return mRenderingPosition;
}


const D2D1_SIZE_F &Pane::GetRenderingSize() const {
  return mSize;
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
  const Pane *pane = this;
  for (; pane->mParent; pane = pane->mParent);
  return pane->mWindow;
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


void Pane::Move(const NPOINT &point) {
  mSettings.position.bottom -= mSettings.position.top - point.y;
  mSettings.position.top = point.y;
  mSettings.position.right -= mSettings.position.left - point.x;
  mSettings.position.left = point.x;

  if (!IsChildPane()) {
    MoveWindow(mWindow, (int)EvaluateLengthParent(mSettings.position.left, true),
      (int)EvaluateLengthParent(mSettings.position.top, false), (int)mSize.width,
      (int)mSize.height, FALSE);
  } else if (mParent) {
    Repaint(false); // Invalidate where we used to be
    mRenderingPosition = D2D1::RectF(
      EvaluateLengthParent(mSettings.position.left, true) + mParent->mRenderingPosition.left,
      EvaluateLengthParent(mSettings.position.top, false) + mParent->mRenderingPosition.top,
      EvaluateLengthParent(mSettings.position.right, true) + mParent->mRenderingPosition.left,
      EvaluateLengthParent(mSettings.position.bottom, false) + mParent->mRenderingPosition.top);
    for (int i = 0; i < mPainters.size(); ++i) {
      mPainters[i]->PositionChanged(this, mPainterData[i], mRenderingPosition, true, false);
    }
    Repaint(true); // And where we are now
  }
}


void Pane::Position(LPCNRECT position) {
  mSettings.position = *position;

  if (!IsChildPane()) {
    //D2D1_RECT_U oldPosition = mWindowPosition;
    mSize = D2D1::SizeF(
      mRenderingPosition.right - mRenderingPosition.left,
      mRenderingPosition.bottom - mRenderingPosition.top);

  } else if (mParent) {
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
    for (int i = 0; i < mPainters.size(); ++i) {
      mPainters[i]->PositionChanged(this, mPainterData[i], mRenderingPosition, isMove, isSize);
    }
    Repaint(true); // And where we are now
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
  for (int i = 0; i < mPainters.size(); ++i) {
    mPainters[i]->TextChanged(this, mPainterData[i], mText);
  }
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


void Pane::ToggleState(BYTE state) {
  if (mActiveStates[state]) {
    ClearState(state);
  } else {
    ActivateState(state);
  }
}


void Pane::Unlock() {
  if (--mUpdateLock == 0) {
    RepaintInvalidated();
  }
}
