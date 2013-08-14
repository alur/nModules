/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ScriptingLSCore.cpp
 *  The nModules Project
 *
 *  Manages the LiteStep object.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../External/v8/include/v8.h"
#include "ScriptingLSCore.h"
#include "ScriptingHelpers.h"
#include <map>


using namespace v8;


// All bangs currently registered by scripts.
static std::map<std::wstring, Persistent<Function> > gScriptBangs;

//
extern Persistent<Context> gScriptingContext;

//typedef wchar_t uint16_t;

/// <summary>
/// Called by LiteStep in order to execute a particular bang, with a particular set of params.
/// </summary>
static void ScriptBangThump(HWND, LPCTSTR bang, LPCTSTR params)
{
    auto iter = gScriptBangs.find(bang);
    if (iter != gScriptBangs.end())
    {
        // Get the default Isolate created at startup.
        Isolate* isolate = Isolate::GetCurrent();

        // Create a stack-allocated handle scope.
        HandleScope handle_scope(isolate);

        // Enter the created context for compiling and
        // running the hello world script.
        Context::Scope context_scope(isolate, gScriptingContext);

        Handle<Function> func = Local<Function>::New(isolate, iter->second);
        Handle<Context> context = Local<Context>::New(isolate, gScriptingContext);
        Handle<Value> arg = String::New(CAST(params));

        func->Call(context->Global(), 1, &arg);
    }
}


/// <summary>
/// Creates the LiteStep object.
/// </summary>
Handle<ObjectTemplate> Scripting::LSCore::Initialize(Isolate *isolate)
{
    //
    // LiteStep
    // - AboutDialog()
    // - Bangs
    // - - Add(bang, func)
    // - - Execute(bang, args)
    // - - List()
    // - - Remove(bang)
    // - Evars
    // - - Get(key)
    // - - GetNumeric(key, default)
    // - - Set(key, value)
    // - Execute(command)
    // - Modules
    // - - List()
    // - - ReLoad(path)
    // - - UnLoad(path)
    // - Recycle()
    // - Refresh()
    // 

    // Create a stack-allocated handle scope.
    HandleScope handleScope(isolate);

    //
    // General
    //
    Handle<ObjectTemplate> liteStep = ObjectTemplate::New();
    liteStep->Set(String::New(CAST(L"Execute")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 1)
        {
            return;
        }

        String::Value command(args[0]);
        LiteStep::LSExecute(nullptr, CAST(*command), 0);
    }));

    //
    // Bangs
    //
    Handle<ObjectTemplate> bangs = ObjectTemplate::New();
    liteStep->Set(String::New(CAST(L"Bangs")), bangs);

    bangs->Set(String::New(CAST(L"Add")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 2)
        {
            return;
        }

        String::Value value(args[0]);
        LPCWSTR bang = CAST(*value);

        // Following the behavior of LS's AddBandCommand, Add overwrites previous bangs.
        if (gScriptBangs.find(bang) != gScriptBangs.end())
        {
            gScriptBangs[bang].Dispose();
        }
        
        gScriptBangs[bang].Reset(Isolate::GetCurrent(), args[1].As<Function>());

        LiteStep::AddBangCommandEx(bang, ScriptBangThump);
    }));

    //
    bangs->Set(String::New(CAST(L"Execute")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 1 && args.Length() != 2)
        {
            return;
        }

        String::Value bang(args[0]);
        String::Value params(args[1]);

        LiteStep::LSExecuteEx(nullptr, L"", CAST(*bang), CAST(*params), L"", 0);
    }));

    //
    bangs->Set(String::New(CAST(L"List")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {

    }));
    
    //
    bangs->Set(String::New(CAST(L"Remove")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {   
        if (args.Length() != 1)
        {
            return;
        }
        String::Value value(args[0]);
        LPCWSTR bang = CAST(*value);

        auto iter = gScriptBangs.find(bang);
        if (iter != gScriptBangs.end())
        {
            iter->second.Dispose();
            gScriptBangs.erase(iter);
            LiteStep::RemoveBangCommand(bang);
        }
    }));

    //
    // Evars
    //
    Handle<ObjectTemplate> evars = ObjectTemplate::New();
    liteStep->Set(String::New(CAST(L"Evars")), evars);

    evars->Set(String::New(CAST(L"Set")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 2)
        {
            return;
        }

        String::Value key(args[0]);
        String::Value value(args[1]);

        args.GetReturnValue().Set(LiteStep::LSSetVariable(CAST(*key), CAST(*value)));
    }));

    //
    evars->Set(String::New(CAST(L"Get")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 1)
        {
            return;
        }

        String::Value key(args[0]);

        wchar_t buf[MAX_LINE_LENGTH];
        LiteStep::GetRCLine(CAST(*key), buf, _countof(buf), nullptr);

        args.GetReturnValue().Set(String::New(CAST(buf)));
    }));

    //
    evars->Set(String::New(CAST(L"GetNumeric")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 1 && args.Length() != 2)
        {
            return;
        }

        String::Value key(args[0]);
        DOUBLE defaultValue = 0.0;

        if (args.Length() == 2)
        {
            defaultValue = (*args[1].As<Number>())->Value();
        }

        args.GetReturnValue().Set(Number::New(LiteStep::GetRCDouble(CAST(*key), defaultValue)));
    }));

    return handleScope.Close(liteStep);
}


/// <summary>
/// Removes script-generated bang commands and disposes of their contexts.
/// </summary>
void Scripting::LSCore::Shutdown()
{
    for (auto &item : gScriptBangs)
    {
        item.second.Dispose();
        LiteStep::RemoveBangCommand(item.first.c_str());
    }
}
