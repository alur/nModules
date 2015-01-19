/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskButton.hpp
 *  The nModules Project
 *
 *  Declaration of the TaskButton class.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Window.hpp"
#include "../nShared/Drawable.hpp"
#include "../nShared/StateRender.hpp"
#include "../Utilities/EnumArray.hpp"
#include "ButtonSettings.hpp"

class TaskButton : public Drawable {
public:
  // All button states
  enum class State {
    Base = 0,
    Minimized,
    Flashing,
    MinimizedFlashing,
    Active,
    Hover,
    MinimizedHover,
    ActiveHover,
    FlashingHover,
    MinimizedFlashingHover,
    Count
  };

public:
  explicit TaskButton(Drawable *parent, HWND hWnd, ButtonSettings &buttonSettings);
  virtual ~TaskButton();

  TaskButton(const TaskButton &) = delete;
  TaskButton& operator=(const TaskButton &) = delete;

public:
  LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

public:
  void Reposition(FLOAT x, FLOAT y, FLOAT width, FLOAT height);

  void Activate();
  void Deactivate();

  void SetIcon(HICON icon);
  void SetOverlayIcon(HICON overlayIcon);
  void SetText(LPCWSTR title);
  void SetProgressState(TBPFLAG state);
  void SetProgressValue(USHORT progress);
  void Flash();

  void GetMinRect(LPPOINTS points);
  void ShowMenu();
  void Show();

  void ActivateState(State state);
  void ClearState(State state);
  void ToggleState(State state);

private:
  void MoveMouseToWindow();

private:
  //
  ButtonSettings &mButtonSettings;

  // The window related to this button.
  HWND mWatchedWindow;

  // The system context menu for this window.
  HMENU mMenu;

  //
  bool mMouseIsOver;

  //
  bool mIsFlashing;
  UINT_PTR mFlashTimer;

  Window::OVERLAY mIcon;
  Window::OVERLAY mIconOverlay;
};
