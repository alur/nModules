/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Tooltip.hpp
 *  The nModules Project
 *
 *  A general tooltip window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Drawable.hpp"
#include "StateRender.hpp"
#include "Window.hpp"

class Tooltip : public Drawable {
public:
  enum class States {
    Base,
    Count
  };

  enum class Placement {
    Left,
    Top,
    Right,
    Bottom,
    Count
  };

public:
  Tooltip(LPCTSTR prefix, Settings * parentSettings);

public:
  void Show(LPCWSTR text, LPRECT position);
  void Hide();

  // MessageHandler
public:
  LRESULT WINAPI HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID);

private:
  int mMaxWidth;
  int mMaxHeight;
  float mOffset;
  Placement mPlacement;

  StateRender<States> mStateRender;
};
