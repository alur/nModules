/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Scripting.cpp
 *  The nModules Project
 *
 *  Manages the JavaScript API.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../External/v8/include/v8.h"
#include "Scripting.h"
#include "ScriptingLSCore.h"
#include <map>
#include <fstream>
#include <streambuf>


using namespace v8;


//
Persistent<Context> gScriptingContext;


/// <summary>
/// Executes a piece of JavaScript.
/// </summary>
static void RunCode(LPCSTR code, void (*callback)(Handle<Value>))
{
    // Get the default Isolate created at startup.
    Isolate* isolate = Isolate::GetCurrent();

    // Create a stack-allocated handle scope.
    HandleScope handle_scope(isolate);

    // Enter the created context for compiling and
    // running the hello world script.
    Context::Scope context_scope(isolate, gScriptingContext);

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


/// <summary>
/// 
/// </summary>
static void InitV8()
{
    // Get the default Isolate created at startup.
    Isolate* isolate = Isolate::GetCurrent();

    // Create a stack-allocated handle scope.
    HandleScope handle_scope(isolate);

    //
    Handle<ObjectTemplate> global = ObjectTemplate::New();

    Handle<ObjectTemplate> liteStep = Scripting::LSCore::Initialize(isolate);

    //
    global->Set(String::New("LiteStep"), liteStep);

    // Create a new context.
    Handle<Context> context = Context::New(isolate, nullptr, global);

    // Create a Persistent handle to the context.
    gScriptingContext.Reset(isolate, context);
}


/// <summary>
/// 
/// </summary>
void Scripting::Initialize()
{
    // Initalize V8
    InitV8();

    // Add bangs for running scripts
    LiteStep::AddBangCommandA("!nAlertScript", [] (HWND, LPCSTR code)
    {
        RunCode(code, [] (Handle<Value> value)
        {
            String::AsciiValue result(value);
            MessageBoxA(nullptr, *result, "nScript", MB_OK);
        });
    });
    LiteStep::AddBangCommandA("!nExecScript", [] (HWND, LPCSTR code)
    {
        RunCode(code, nullptr);
    });

    // Load script files
    LiteStep::IterateOverLineTokens(_T("*nIncludeScript"), [] (LPCTSTR file)
    {
        std::ifstream t(file);
        std::string str;

        t.seekg(0, std::ios::end);   
        str.reserve(t.tellg());
        t.seekg(0, std::ios::beg);

        str.assign((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());

        RunCode(str.c_str(), nullptr);        
    });
}


/// <summary>
/// 
/// </summary>
void Scripting::Shutdown()
{
    // Remove bang commands
    LiteStep::RemoveBangCommandA("!nAlertScript");
    LiteStep::RemoveBangCommandA("!nExecScript");
    LSCore::Shutdown();
    
    // Dispose of our context, killing all JS data
    gScriptingContext.Dispose();
}
