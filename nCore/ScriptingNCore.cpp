/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ScriptingNCore.cpp
 *  The nModules Project
 *
 *  Manages the nCore Object.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../External/v8/include/v8.h"
#include "ScriptingNCore.h"
#include "../nShared/Window.hpp"
#include "ScriptingHelpers.h"


using namespace v8;


// 
EXPORT_CDECL(Window*) FindRegisteredWindow(LPCTSTR prefix);


/// <summary>
/// Creates the LiteStep object.
/// </summary>
Handle<ObjectTemplate> Scripting::NCore::Initialize(Isolate *isolate)
{
    // Create a stack-allocated handle scope.
    HandleScope handleScope(isolate);

    //
    // General
    //
    Handle<ObjectTemplate> nCore = ObjectTemplate::New();

    nCore->Set(String::New(CAST(L"Size")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() < 3 || args.Length() > 5)
        {
            return;
        }

        // Find the window to operate on
        if (!args[0]->IsString())
        {
            return;
        }
        String::Value windowName(args[0]);
        Window *window = FindRegisteredWindow(CAST(*windowName));

        if (window == nullptr)
        {
            return;
        }

        // Get the new width and height
        if (!args[1]->IsNumber() || !args[2]->IsNumber())
        {
            return;
        }

        int width = args[1].As<Number>()->Int32Value();
        int height = args[2].As<Number>()->Int32Value();
        
        window->Resize(width, height);
        if (window->IsChild())
        {
            window->Repaint();
        }
    }));

    return handleScope.Close(nCore);
}
