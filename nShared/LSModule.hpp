/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LSModule.hpp
 *  The nModules Project
 *
 *  Deals with all basic LiteStep module functionality.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Macros.h"
#include "Versioning.h"
#include <map>
#include <string>
#include "Drawable.hpp"


using namespace std;


// Functions which all modules must implement.
LRESULT WINAPI LSMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
EXPORT_CDECL(int) initModuleEx(HWND parent, HINSTANCE instance, LPCSTR path);
EXPORT_CDECL(void) quitModule(HINSTANCE instance);


class LSModule : public MessageHandler {
public:
    explicit LSModule(HWND parent, LPCSTR moduleName, LPCSTR author, VERSION version, HINSTANCE instance);
    virtual ~LSModule();

    // Registers window classes and creates the message handler window.
    bool Initialize(PWNDCLASSEX customMessageClass = NULL, PWNDCLASSEX customDrawableClass = NULL);
    
    // Connects to nCore.
    bool ConnectToCore(VERSION minimumCoreVersion);

    // Adds a top-level drawable.
    void AddDrawable(LPCSTR name, Drawable* drawable);

    // Creates a top-level drawable window.
    DrawableWindow* CreateDrawableWindow(Settings* settings);

    // Registers the message handler to recieve these messages from the core.
    void RegisterForMessages(UINT messages[]);

    //
    LRESULT WINAPI HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

private:
    // The window class used by the message handler.
    ATOM messageHandlerClass;

    // The window class used for drawable windows.
    ATOM drawableClass;

    // The name of this module.
    LPCSTR moduleName;

    // The author of this module.
    LPCSTR author;

    // The version of this module.
    VERSION version;

    // The LiteStep message handler window.
    HWND messageHandler;

    // The parent window given to initModuleEx. Should be used as the parent for all top-level windows.
    HWND parent;

    // The instance of this module.
    HINSTANCE instance;

    // All top-level drawables.
    map<string, Drawable*> drawables;
};
