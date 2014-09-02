/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Balloon.hpp
 *  The nModules Project
 *
 *  A general balloon popup window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Drawable.hpp"
#include "StateRender.hpp"
#include "Window.hpp"

class Balloon : public Drawable {
public:
  enum class States {
    Base,
    Count
  };
public:
  Balloon(LPCTSTR prefix, Settings* parentSettings, UINT clickedMessage, MessageHandler* callbackHandler);
  ~Balloon();

  void Show(LPCWSTR title, LPCWSTR text, HICON icon, LPSIZE iconSize, LPRECT targetPosition);
  void Hide();

  UINT GetClickedMessage();
  void SetClickedMessage(UINT);

private:
    void LoadSettings();

public:
  LRESULT WINAPI HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID Window);

private:
  UINT mClickedMessage;
  MessageHandler *mCallbackHandler;

  StateRender<States> mBackStates;
  StateRender<States> mTitleStates;
  StateRender<States> mCloseBtnStates;

  Window *mTitleWindow;
  Window *mCloseBtnWindow;

  Settings *mTitleSettings;
  Settings *mCloseBtnSettings;
};
