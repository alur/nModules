/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  MessageHandler.hpp
 *  The nModules Project
 *
 *  A class which can accept window messages.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../Utilities/Common.h"

#define GWLP_MESSAGEHANDLER 0

class MessageHandler {
public:
  // Fixes up values.
  static void FixWindowClass(LPWNDCLASSEX ex);

  // Wrapper for CreateWindowEx, ensuring that LPARAM is set to MessageHandler*.
  static HWND CreateMessageWindowEx(DWORD exStyle, LPCTSTR className, LPCTSTR windowName,
    DWORD style, int x, int y, int width, int height, HWND parent, HMENU menu, HINSTANCE instance,
    MessageHandler* messageHandler);

  // Implemented by the derived class, handles the window messages.
  virtual LRESULT WINAPI HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam,
    LPVOID extra) = 0;

  // True if this object is ready to receive messages.
  bool mInitialized;

private:
  // Forwards messages to the appropriate msghandler.
  static LRESULT WINAPI WindowProcedure(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

  // Forwards messages to the appropriate msghandler.
  static LRESULT WINAPI WindowProcedureInit(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
};
