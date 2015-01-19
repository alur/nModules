/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LSModule.hpp
 *  The nModules Project
 *
 *  Deals with all basic LiteStep module functionality.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../Utilities/Common.h"
#include "../Utilities/Versioning.h"
#include <map>
#include <string>
#include "Drawable.hpp"

// Functions which all modules must implement.
LRESULT WINAPI LSMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
EXPORT_CDECL(int) initModuleW(HWND parent, HINSTANCE instance, LPCWSTR path);
EXPORT_CDECL(void) quitModule(HINSTANCE instance);

class LSModule : public MessageHandler {
public:
  LSModule(LPCTSTR moduleName, LPCTSTR author, VERSION version);

  // Registers window classes and creates the message handler window.
  bool Initialize(HWND parent, HINSTANCE instance, PWNDCLASSEX customMessageClass = nullptr, PWNDCLASSEX customDrawableClass = nullptr);
  void DeInitalize();

  // Connects to nCore.
  bool ConnectToCore(VERSION minimumCoreVersion);

  // Creates a top-level drawable window.
  Window *CreateDrawableWindow(Settings *settings, MessageHandler *msgHandler);

  // Registers the message handler to recieve these messages from the core.
  void RegisterForMessages(UINT messages[]);

  //
  HWND GetMessageWindow();

  //
  HINSTANCE GetInstance();

  //
  LRESULT WINAPI HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID extra) override;

private:
  // The window class used by the message handler.
  ATOM messageHandlerClass;

  // The window class used for drawable windows.
  ATOM drawableClass;

  // The name of this module.
  LPCTSTR moduleName;

  // The author of this module.
  LPCTSTR author;

  // The version of this module.
  VERSION version;

  // The LiteStep message handler window.
  HWND messageHandler;

  // The parent window given to initModuleEx. Should be used as the parent for all top-level windows.
  HWND parent;

  // The instance of this module.
  HINSTANCE instance;
};
