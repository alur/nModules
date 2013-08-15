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

    nCore->Set(String::New(CAST(L"Move")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
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

        int x = args[1].As<Number>()->Int32Value();
        int y = args[2].As<Number>()->Int32Value();
        
        if (args.Length() == 3)
        {
            window->Move(x, y);
            if (window->IsChild())
            {
                window->Repaint();
            }
        }
        else
        {
            if (!args[3]->IsNumber())
            {
                return;
            }
            int duration = args[3].As<Number>()->Int32Value();

            Easing::Type easingType = Easing::Type::Linear;

            if (args.Length() == 5)
            {
                String::Value easingName(args[4]);
                easingType = Easing::EasingFromString(CAST(*easingName));
            }
            
            window->SetAnimation(x, y, window->GetDrawingSettings()->width, window->GetDrawingSettings()->height,
                duration, easingType);
        }
    }));

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
        
        if (args.Length() == 3)
        {
            window->Resize(width, height);
            if (window->IsChild())
            {
                window->Repaint();
            }
        }
        else
        {
            if (!args[3]->IsNumber())
            {
                return;
            }
            int duration = args[3].As<Number>()->Int32Value();

            Easing::Type easingType = Easing::Type::Linear;

            if (args.Length() == 5)
            {
                String::Value easingName(args[4]);
                easingType = Easing::EasingFromString(CAST(*easingName));
            }
            
            window->SetAnimation(window->GetDrawingSettings()->x, window->GetDrawingSettings()->y,
                width, height, duration, easingType);
        }
    }));

    nCore->Set(String::New(CAST(L"Position")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() < 5 || args.Length() > 7)
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
        if (!args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber() || !args[4]->IsNumber())
        {
            return;
        }
        
        int x = args[1].As<Number>()->Int32Value();
        int y = args[2].As<Number>()->Int32Value();
        int width = args[3].As<Number>()->Int32Value();
        int height = args[4].As<Number>()->Int32Value();
        
        if (args.Length() == 5)
        {
            window->SetPosition(x, y, width, height);
            if (window->IsChild())
            {
                window->Repaint();
            }
        }
        else
        {
            if (!args[5]->IsNumber())
            {
                return;
            }
            int duration = args[5].As<Number>()->Int32Value();

            Easing::Type easingType = Easing::Type::Linear;

            if (args.Length() == 7)
            {
                String::Value easingName(args[6]);
                easingType = Easing::EasingFromString(CAST(*easingName));
            }
            
            window->SetAnimation(x, y, width, height, duration, easingType);
        }
    }));

    //
    nCore->Set(String::New(CAST(L"Hide")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 1)
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

        window->Hide();
    }));

    //
    nCore->Set(String::New(CAST(L"Show")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 1)
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

        window->Show();
        if (window->IsChild())
        {
            window->Repaint();
        }
    }));

    //
    nCore->Set(String::New(CAST(L"GetX")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 1)
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

        args.GetReturnValue().Set(window->GetDrawingSettings()->x);
    }));

    //
    nCore->Set(String::New(CAST(L"GetY")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 1)
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

        args.GetReturnValue().Set(window->GetDrawingSettings()->y);
    }));

    //
    nCore->Set(String::New(CAST(L"GetHeight")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 1)
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

        args.GetReturnValue().Set(window->GetDrawingSettings()->height);
    }));

    //
    nCore->Set(String::New(CAST(L"GetWidth")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
    {
        if (args.Length() != 1)
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

        args.GetReturnValue().Set(window->GetDrawingSettings()->width);
    }));

    return handleScope.Close(nCore);
}
