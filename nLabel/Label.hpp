//-------------------------------------------------------------------------------------------------
// /nLabel/Label.hpp
// The nModules Project
//
// Declaration of the Label class.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "../nShared/Drawable.hpp"
#include "../nShared/StateRender.hpp"
#include "../nShared/Window.hpp"

#include <forward_list>
#include <unordered_map>

class Label : public Drawable {
public:
  explicit Label(LPCWSTR name);
  explicit Label(LPCWSTR name, Drawable * parent);
  virtual ~Label();

  // IDrawable
private:
  LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID) override;

public:
  enum class States {
    Base,
    Hover,
    Pressed,
    Count
  };

private:
  void Initalize();

private:
  StateRender<States> mStateRender;
  std::forward_list<Label> mOverlays;
  int mButtonsPressed;
};
