/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  MessageHandler.hpp
 *  The nModules Project
 *
 *  A class which can accept window messages.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "MessageHandler.hpp"

#include "../Utilities/Common.h"


/// <summary>
/// Fixes up a WNDCLASSEX struct to be used as a message handler.
/// </summary>
void MessageHandler::FixWindowClass(LPWNDCLASSEX ex) {
  ex->cbWndExtra += sizeof(MessageHandler*);
  ex->lpfnWndProc = &MessageHandler::WindowProcedureInit;
}


/// <summary>
/// Wrapper for CreateWindowEx making sure that lParam is a MessageHandler*.
/// </summary>
HWND MessageHandler::CreateMessageWindowEx(DWORD exStyle, LPCTSTR className, LPCTSTR windowName,
  DWORD style, int x, int y, int width, int height, HWND parent, HMENU menu, HINSTANCE instance,
  MessageHandler *messageHandler) {
  return CreateWindowEx(exStyle, className, windowName, style, x, y, width, height, parent, menu, instance, messageHandler);
}


/// <summary>
/// Wrapper since we can't point to a member function directly.
/// </summary>
LRESULT WINAPI MessageHandler::WindowProcedure(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
  return ((MessageHandler*)GetWindowLongPtr(window, 0))->HandleMessage(window, msg, wParam, lParam, nullptr);
}


/// <summary>
/// Sets the windowproc and to MessageHandler::WindowProcedure and GWLP_MESSAGEHANDLER. This is done this
/// way in order to reduce the amount of code in the window procedure.
/// </summary>
LRESULT WINAPI MessageHandler::WindowProcedureInit(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (msg == WM_CREATE) {
    MessageHandler *msgHandler = (MessageHandler*)((LPCREATESTRUCT)lParam)->lpCreateParams;
    SetWindowLongPtr(window, GWLP_MESSAGEHANDLER, (LONG_PTR)msgHandler);
    SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)&MessageHandler::WindowProcedure);
    return msgHandler->HandleMessage(window, msg, wParam, lParam, nullptr);
  }
  return DefWindowProc(window, msg, wParam, lParam);
}
