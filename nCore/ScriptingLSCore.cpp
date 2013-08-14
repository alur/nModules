/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ScriptingLSCore.cpp
 *  The nModules Project
 *
 *  Manages the JavaScript API.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../External/v8/include/v8.h"
#include "ScriptingLSCore.h"
#include <map>


using namespace v8;


// All bangs currently registered by scripts.
static std::map<std::string, Persistent<Function> > gScriptBangs;

//
extern Persistent<Context> gScriptingContext;


/// <summary>
/// Called by LiteStep in order to execute a particular bang, with a particular set of params.
/// </summary>
static void ScriptBangThump(HWND, LPCSTR bang, LPCSTR params)
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
        Handle<Value> arg = String::New(params);

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
    // Bangs
    //
    Handle<ObjectTemplate> bangs = ObjectTemplate::New();
    liteStep->Set(String::New("Bangs"), bangs);

    bangs->Set(String::New("Add"), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 2)
        {
            return;
        }

        String::AsciiValue bang(args[0]);

        // Following the behavior of LS's AddBandCommand, Add overwrites previous bangs.
        if (gScriptBangs.find(*bang) != gScriptBangs.end())
        {
            gScriptBangs[*bang].Dispose();
        }
        
        gScriptBangs[*bang].Reset(Isolate::GetCurrent(), args[1].As<Function>());

        LiteStep::AddBangCommandExA(*bang, ScriptBangThump);
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
        if (args.Length() != 1)
        {
            return;
        }
        String::AsciiValue bang(args[0]);

        auto iter = gScriptBangs.find(*bang);
        if (iter != gScriptBangs.end())
        {
            iter->second.Dispose();
            gScriptBangs.erase(iter);
            LiteStep::RemoveBangCommandA(*bang);
        }
    }));

    //
    // Evars
    //
    Handle<ObjectTemplate> evars = ObjectTemplate::New();
    liteStep->Set(String::New("Evars"), evars);

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
        LiteStep::RemoveBangCommandA(item.first.c_str());
    }
}
