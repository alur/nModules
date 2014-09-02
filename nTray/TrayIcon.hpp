//-------------------------------------------------------------------------------------------------
// /nTray/TrayIcon.hpp
// The nModules Project
//
// An icon inside a tray.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "Types.h"

#include "../nShared/Drawable.hpp"
#include "../nShared/StateRender.hpp"
#include "../nShared/Window.hpp"

class TrayIcon : public Drawable {
public:
  enum class States {
    Base = 0,
    Count
  };

public:
  TrayIcon(Drawable *parent, IconData&, WindowSettings&, StateRender<States>*);

public:
  void Reposition(RECT);
  void Reposition(UINT x, UINT y, UINT width, UINT height);
  void Show();

  void SendCallback(UINT message, WPARAM wParam, LPARAM lParam);
  void GetScreenRect(LPRECT rect);

  void HandleModify(LiteStep::LPLSNOTIFYICONDATA);

private:
  void UpdateIcon();

public:
  LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

  // Settings
private:
  int mIconSize;
  IconData &mIconData;

private:
  bool mShowingTip;
  Window::OVERLAY mIconOverlay;
};
