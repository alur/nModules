#pragma once

#include "../nCoreApi/IPane.hpp"

#include "../nUtilities/d2d1.h"

#include <unordered_set>

class Pane : public IPane {
private:
  struct Settings {
    // The position of the pane, relative to the parent.
    NRECT position;
    wchar_t parent[MAX_PREFIX];
    bool alwaysOnTop;
    // The monitor to use for computing relative sizes. Top-level pane only.
    UINT monitor;
  } mSettings;

public:
  static HRESULT CreateWindowClasses(HINSTANCE);
  static void DestroyWindowClasses(HINSTANCE);

  static void FullscreenActivated(HMONITOR, HWND);
  static void FullscreenDeactivated(HMONITOR);

public:
  Pane &operator=(Pane&) = delete;
  Pane(const Pane&) = delete;

public:
  Pane(const PaneInitData *initData, Pane *parent);
  ~Pane();

  // IMessageHandler;
public:
  LRESULT APICALL HandleMessage(HWND, UINT, WPARAM, LPARAM, NPARAM) override;

  // IPane (PublicApi)
public:
  IPane* APICALL CreateChild(const PaneInitData*) override;
  void APICALL Destroy() override;
  float APICALL EvaluateLength(const NLENGTH &length, bool horizontal) const override;
  D2D1_RECT_F APICALL EvaluateRect(const NRECT &rect) const override;
  LPVOID APICALL GetPainterData() const override;
  const D2D1_RECT_F &APICALL GetRenderingPosition() const override;
  const D2D1_SIZE_F &APICALL GetRenderingSize() const override;
  LPCWSTR APICALL GetRenderingText() const override;
  bool APICALL GetScreenPosition(D2D1_RECT_F *rect) const override;
  HWND APICALL GetWindow() const override;
  void APICALL Hide() override;
  void APICALL Lock() override;
  void APICALL PaintChildren(ID2D1RenderTarget*, const D2D1_RECT_F *area) const override;
  void APICALL Position(LPCNRECT) override;
  void APICALL Repaint(LPCNRECT) override;
  void APICALL SetText(LPCWSTR) override;
  void APICALL Show() override;
  void APICALL Unlock() override;

private:
  void DiscardDeviceResources();
  HRESULT ReCreateDeviceResources();

  void OnFullscreenActivated(HMONITOR, HWND);
  void OnFullscreenDeactivated(HMONITOR);

  inline bool IsChildPane() const;
  // Evaluates a length in the context of the parent.
  float EvaluateLengthParent(const NLENGTH &length, bool horizontal) const;
  void Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area) const;
  void ParentPositionChanged();

  // Invalidates the entire pane.
  void Repaint(bool update);

  // Repaints the invalidated area of the window.
  void RepaintInvalidated() const;

  // Invalidates the given area of the pane.
  void Repaint(const D2D1_RECT_F &area, bool update);

private:
  static LRESULT WINAPI ExternWindowProc(HWND, UINT, WPARAM, LPARAM);
  static LRESULT WINAPI InitWindowProc(HWND, UINT, WPARAM, LPARAM);

  // Top-level panes only.
private:
  HWND mWindow;
  D2D1_RECT_U mWindowPosition;

  // Child specific.
private:
  Pane *mParent;

  // All panes.
private:
  IMessageHandler *const mMessageHandler;
  IPanePainter *const mPainter;
  std::unordered_set<Pane*> mChildren;
  // The absolute position of this element within the window.
  D2D1_RECT_F mRenderingPosition;
  // The size of the window, in pixels.
  D2D1_SIZE_F mSize;
  D2D1_POINT_2F mDpi;
  // When this is not 0, we won't repaint the pane.
  int mUpdateLock;
  bool mVisible;
  LPWSTR mText;
  // A pointer retrieved from the painter, which is passed back when we paint. Allows the painter
  // to store per-window data.
  LPVOID mPainterData;
  // The name of the pane, if it's named.
  wchar_t mName[MAX_PREFIX];

  bool mCoveredByFullscreenWindow;

  // Set on all panes, but created & destroyed by the top-level.
private:
  ID2D1HwndRenderTarget *mRenderTarget;
};
