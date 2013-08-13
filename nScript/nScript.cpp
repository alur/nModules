/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nScript.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nCalendar module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"
#include "nScript.h"
#include <map>
#include "Version.h"
#include "v8\v8.h"


#ifdef _DEBUG
#   pragma comment(lib, "v8_base.debug.lib")
#else
#   pragma comment(lib, "v8_base.lib")
#endif
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Winmm.lib")


// The LSModule class
LSModule gLSModule(_T(MODULE_NAME), _T(MODULE_AUTHOR), MakeVersion(MODULE_VERSION));

// The messages we want from the core
const UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, 0 };


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
EXPORT_CDECL(int) initModuleW(HWND parent, HINSTANCE instance, LPCWSTR /* path */)
{
    if (!gLSModule.Initialize(parent, instance))
    {
        return 1;
    }

    if (!gLSModule.ConnectToCore(MakeVersion(CORE_VERSION)))
    {
        return 1;
    }

    // Load settings
    LoadSettings();

    using namespace v8;

    // Get the default Isolate created at startup.
    Isolate* isolate = Isolate::GetCurrent();

    // Create a stack-allocated handle scope.
    HandleScope handle_scope(isolate);

    // Create a new context.
    Handle<Context> context = Context::New(isolate);

    // Here's how you could create a Persistent handle to the context, if needed.
    Persistent<Context> persistent_context(isolate, context);
  
    // Enter the created context for compiling and
    // running the hello world script. 
    Context::Scope context_scope(context);

    // Create a string containing the JavaScript source code.
    Handle<String> source = String::New("function funcA(z) { return z + 'this is funcA'; }");

    // Compile the source code.
    Handle<Script> script = Script::Compile(source);
  
    // Run the script to get the result.
    Handle<Value> result = script->Run();

    // Create a string containing the JavaScript source code.
    Handle<String> source2 = String::New("funcA('this is source2');");

    // Compile the source code.
    Handle<Script> script2 = Script::Compile(source2);
  
    // Run the script to get the result.
    Handle<Value> result2 = script2->Run();
  
    // The persistent handle needs to be eventually disposed.
    persistent_context.Dispose();

    // Convert the result to an ASCII string and print it.
    String::AsciiValue ascii(result2);
    TRACE("%s\n", *ascii);

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* instance */)
{
    gLSModule.DeInitalize();
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="window">The window the message is for.</param>
/// <param name="message">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LSMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_CREATE:
        {
            SendMessage(LiteStep::GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
        }
        return 0;

    case WM_DESTROY:
        {
            SendMessage(LiteStep::GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
        }
        return 0;

    case LM_REFRESH:
        {
        }
        return 0;
    }
    return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// Reads through the .rc files and creates calendars.
/// </summary>
void LoadSettings()
{
}
