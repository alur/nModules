#pragma once

#include "../nCoreApi/IPane.hpp"

#include "../nUtilities/d2d1.h"

class Pane : public IPane {
private:
  struct Settings {
    NRECT position;
    bool alwaysOnTop;
  } mSettings;

public:
  static HRESULT CreateWindowClasses(HINSTANCE);
  static void DestroyWindowClasses(HINSTANCE);


public:
  Pane &operator=(Pane&) = delete;
  Pane(const Pane&) = delete;

public:
  explicit Pane(const PaneInitData *initData, Pane *parent);
  ~Pane();

  // IMessageHandler;
public:
  LRESULT APICALL HandleMessage(HWND, UINT, WPARAM, LPARAM, NPARAM) override;

  // IPane
public:
  IPane* APICALL CreateChild(const PaneInitData *initData) override;
  void APICALL Destroy() override;
  HWND APICALL GetWindow() const override;

private:
  HRESULT ReCreateDeviceResources();
  void DiscardDeviceResources();

  float EvaluateLength(NLENGTH &length);

private:
  static LRESULT WINAPI ExternWindowProc(HWND, UINT, WPARAM, LPARAM);
  static LRESULT WINAPI InitWindowProc(HWND, UINT, WPARAM, LPARAM);

  // Top-level windows only.
private:
  HWND mWindow;
  D2D1_SIZE_U mSize;

  // All windows.
private:
  IMessageHandler *const mMessageHandler;
  IPanePainter *const mPainter;
  ID2D1HwndRenderTarget *mRenderTarget;
};
