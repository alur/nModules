/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nScript.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nCalendar module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"
#include "../External/v8/include/v8.h"
#include "nScript.h"
#include <map>
#include "Version.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Winmm.lib")

using namespace v8;

// The LSModule class
LSModule gLSModule(_T(MODULE_NAME), _T(MODULE_AUTHOR), MakeVersion(MODULE_VERSION));

// The messages we want from the core
const UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, 0 };

//
Persistent<Context> gContext;


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

    //
    InitV8();

    //
    LiteStep::AddBangCommandA("!nScriptAlert", BangAlert);
    LiteStep::AddBangCommandA("!nScriptExec", BangExec);

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* instance */)
{
    LiteStep::RemoveBangCommandA("!nScriptAlert");
    LiteStep::RemoveBangCommandA("!nScriptExec");
    CleanupV8();
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


void GetString(const FunctionCallbackInfo<Value> &args)
{
}


void InitV8()
{
    // Get the default Isolate created at startup.
    Isolate* isolate = Isolate::GetCurrent();

    // Create a stack-allocated handle scope.
    HandleScope handle_scope(isolate);

    //
    Handle<ObjectTemplate> global = ObjectTemplate::New();
    Handle<ObjectTemplate> liteStep = ObjectTemplate::New();
    Handle<ObjectTemplate> bangs = ObjectTemplate::New();
    Handle<ObjectTemplate> evars = ObjectTemplate::New();

    global->Set(String::New("LiteStep"), liteStep);

    liteStep->Set(String::New("Bangs"), bangs);

    //
    bangs->Set(String::New("Add"), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
    }));

    //
    bangs->Set(String::New("Execute"), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 1 && args.Length() != 2)
        {
            return;
        }

        String::AsciiValue bang(args[0]);
        String::AsciiValue params(args[1]);

        LiteStep::LSExecuteExA(nullptr, "", *bang, *params, "", 0);
    }));

    //
    bangs->Set(String::New("List"), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
    }));
    
    //
    bangs->Set(String::New("Remove"), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
    }));

    //
    evars->Set(String::New("Set"), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 2)
        {
            return;
        }

        String::AsciiValue key(args[0]);
        String::AsciiValue value(args[1]);

        args.GetReturnValue().Set(LiteStep::LSSetVariableA(*key, *value));
    }));

    //
    liteStep->Set(String::New("Execute"), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 1)
        {
            return;
        }

        String::AsciiValue command(args[0]);
        LiteStep::LSExecuteA(nullptr, *command, 0);
    }));

    //
    evars->Set(String::New("Get"), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 1)
        {
            return;
        }

        String::AsciiValue key(args[0]);

        char buf[MAX_LINE_LENGTH];
        LiteStep::GetRCLineA(*key, buf, MAX_LINE_LENGTH, nullptr);

        args.GetReturnValue().Set(String::New(buf));
    }));

    //
    evars->Set(String::New("GetNumeric"), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 1 && args.Length() != 2)
        {
            return;
        }

        String::AsciiValue key(args[0]);
        DOUBLE defaultValue = 0.0;

        if (args.Length() == 2)
        {
            defaultValue = (*args[1].As<Number>())->Value();
        }

        args.GetReturnValue().Set(Number::New(LiteStep::GetRCDoubleA(*key, defaultValue)));
    }));

    //
    liteStep->Set(String::New("ScriptVersion"), String::New(MODULE_VERSION_STR));

    // Create a new context.
    Handle<Context> context = Context::New(isolate, NULL, global);

    // Create a Persistent handle to the context.
    gContext.Reset(isolate, context);
}


void CleanupV8()
{
    // The persistent handle needs to be eventually disposed.
    gContext.Dispose();
}


void BangExec(HWND, LPCSTR code)
{
    RunCode(code, nullptr);
}


void BangAlert(HWND, LPCSTR code)
{
    RunCode(code, [] (Handle<Value> value)
    {
        String::AsciiValue result(value);
        MessageBoxA(nullptr, *result, "nScript", MB_OK);
    });
}


void RunCode(LPCSTR code, void (*callback)(Handle<Value>))
{
    // Get the default Isolate created at startup.
    Isolate* isolate = Isolate::GetCurrent();

    // Create a stack-allocated handle scope.
    HandleScope handle_scope(isolate);

    // Enter the created context for compiling and
    // running the hello world script.
    Context::Scope context_scope(isolate, gContext);

    // Create a string containing the JavaScript source code.
    Handle<String> source = String::New(code);

    // Compile the source code.
    TryCatch tryCatch;
    Handle<Script> script = Script::Compile(source);
    if (script.IsEmpty())
    {
        if (tryCatch.HasCaught())
        {
            String::AsciiValue exception(tryCatch.Exception());
            MessageBoxA(nullptr, *exception, "nScript Error", MB_OK);
        }
        return;
    }
  
    // Run the script to get the result.
    Handle<Value> result = script->Run();
    if (result.IsEmpty())
    {
        if (tryCatch.HasCaught())
        {
            String::AsciiValue exception(tryCatch.Exception());
            MessageBoxA(nullptr, *exception, "nScript Exception", MB_OK);
        }
        return;
    } 
    
    if (callback)
    {
        callback(result);
    }
}
