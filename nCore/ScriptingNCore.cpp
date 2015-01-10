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


static void MoveWindow(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() < 3 || args.Length() > 5) {
    return;
  }

  // Find the window to operate on
  if (!args[0]->IsString()) {
    return;
  }
  String::Value windowName(args[0]);
  Window * window = FindRegisteredWindow(CAST(*windowName));

  if (window == nullptr) {
    return;
  }

  // Get the new width and height
  if (!args[1]->IsNumber() || !args[2]->IsNumber())
    return;

  float x = (float)args[1].As<Number>()->Value();
  float y = (float)args[2].As<Number>()->Value();

  if (args.Length() == 3) {
    window->Move(x, y);
    if (window->IsChild()) {
      window->Repaint();
    }
  } else {
    if (!args[3]->IsNumber()) {
      return;
    }
    int duration = args[3].As<Number>()->Int32Value();

    Easing::Type easingType = Easing::Type::Linear;

    if (args.Length() == 5) {
      String::Value easingName(args[4]);
      easingType = Easing::EasingFromString(CAST(*easingName));
    }

    window->SetAnimation(x, y, window->GetDrawingSettings()->width,
      window->GetDrawingSettings()->height, duration, easingType);
  }
}


static void SizeWindow(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() < 3 || args.Length() > 5) {
    return;
  }

  // Find the window to operate on
  if (!args[0]->IsString()) {
    return;
  }
  String::Value windowName(args[0]);
  Window * window = FindRegisteredWindow(CAST(*windowName));

  if (window == nullptr) {
    return;
  }

  // Get the new width and height
  if (!args[1]->IsNumber() || !args[2]->IsNumber()) {
    return;
  }

  float width = (float)args[1].As<Number>()->Value();
  float height = (float)args[2].As<Number>()->Value();

  if (args.Length() == 3) {
    window->Resize(width, height);
    if (window->IsChild()) {
      window->Repaint();
    }
  } else {
    if (!args[3]->IsNumber()) {
      return;
    }
    int duration = args[3].As<Number>()->Int32Value();

    Easing::Type easingType = Easing::Type::Linear;

    if (args.Length() == 5) {
      String::Value easingName(args[4]);
      easingType = Easing::EasingFromString(CAST(*easingName));
    }

    window->SetAnimation(window->GetDrawingSettings()->x, window->GetDrawingSettings()->y, width,
      height, duration, easingType);
  }
}


static void PositionWindow(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() < 5 || args.Length() > 7) {
    return;
  }

  // Find the window to operate on
  if (!args[0]->IsString()) {
    return;
  }
  String::Value windowName(args[0]);
  Window * window = FindRegisteredWindow(CAST(*windowName));

  if (window == nullptr) {
    return;
  }

  // Get the new width and height
  if (!args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber() || !args[4]->IsNumber()) {
    return;
  }

  float x = (float)args[1].As<Number>()->Value();
  float y = (float)args[2].As<Number>()->Value();
  float width = (float)args[3].As<Number>()->Value();
  float height = (float)args[4].As<Number>()->Value();

  if (args.Length() == 5) {
    window->SetPosition(x, y, width, height);
    if (window->IsChild()) {
      window->Repaint();
    }
  } else {
    if (!args[5]->IsNumber()) {
      return;
    }
    int duration = args[5].As<Number>()->Int32Value();

    Easing::Type easingType = Easing::Type::Linear;

    if (args.Length() == 7) {
      String::Value easingName(args[6]);
      easingType = Easing::EasingFromString(CAST(*easingName));
    }

    window->SetAnimation(x, y, width, height, duration, easingType);
  }
}


static void HideWindow(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() != 1) {
    return;
  }

  // Find the window to operate on
  if (!args[0]->IsString()) {
    return;
  }
  String::Value windowName(args[0]);
  Window * window = FindRegisteredWindow(CAST(*windowName));

  if (window == nullptr) {
    return;
  }

  window->Hide();
}


static void ShowWindow(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() != 1) {
    return;
  }

  // Find the window to operate on
  if (!args[0]->IsString()) {
    return;
  }
  String::Value windowName(args[0]);
  Window * window = FindRegisteredWindow(CAST(*windowName));

  if (window == nullptr) {
    return;
  }

  window->Show();
  if (window->IsChild()) {
    window->Repaint();
  }
}


static void GetWindowX(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() != 1) {
    return;
  }

  // Find the window to operate on
  if (!args[0]->IsString()) {
    return;
  }
  String::Value windowName(args[0]);
  Window * window = FindRegisteredWindow(CAST(*windowName));

  if (window == nullptr) {
    return;
  }

  args.GetReturnValue().Set(window->GetPosition().x);
}


static void GetWindowY(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() != 1) {
    return;
  }

  // Find the window to operate on
  if (!args[0]->IsString()) {
    return;
  }
  String::Value windowName(args[0]);
  Window * window = FindRegisteredWindow(CAST(*windowName));

  if (window == nullptr) {
    return;
  }

  args.GetReturnValue().Set(window->GetPosition().y);
}


static void GetWindowHeight(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() != 1) {
    return;
  }

  // Find the window to operate on
  if (!args[0]->IsString()) {
    return;
  }
  String::Value windowName(args[0]);
  Window *window = FindRegisteredWindow(CAST(*windowName));

  if (window == nullptr) {
    return;
  }

  args.GetReturnValue().Set(window->GetSize().height);
}


static void GetWindowWidth(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() != 1) {
    return;
  }

  // Find the window to operate on
  if (!args[0]->IsString()) {
    return;
  }
  String::Value windowName(args[0]);
  Window *window = FindRegisteredWindow(CAST(*windowName));

  if (window == nullptr) {
    return;
  }

  args.GetReturnValue().Set(window->GetSize().width);
}


/// <summary>
/// Creates the LiteStep object.
/// </summary>
Handle<ObjectTemplate> Scripting::NCore::Initialize(Isolate *isolate) {
  HandleScope handleScope(isolate);

  Handle<ObjectTemplate> nCore = ObjectTemplate::New();

  Handle<ObjectTemplate> window = ObjectTemplate::New();
  nCore->Set(String::New(CAST(L"Window")), window, PropertyAttribute::ReadOnly);
  window->Set(String::New(CAST(L"Move")), FunctionTemplate::New(MoveWindow), PropertyAttribute::ReadOnly);
  window->Set(String::New(CAST(L"Size")), FunctionTemplate::New(SizeWindow), PropertyAttribute::ReadOnly);
  window->Set(String::New(CAST(L"Position")), FunctionTemplate::New(PositionWindow), PropertyAttribute::ReadOnly);
  window->Set(String::New(CAST(L"Hide")), FunctionTemplate::New(HideWindow), PropertyAttribute::ReadOnly);
  window->Set(String::New(CAST(L"Show")), FunctionTemplate::New(ShowWindow), PropertyAttribute::ReadOnly);
  window->Set(String::New(CAST(L"GetX")), FunctionTemplate::New(GetWindowX), PropertyAttribute::ReadOnly);
  window->Set(String::New(CAST(L"GetY")), FunctionTemplate::New(GetWindowY), PropertyAttribute::ReadOnly);
  window->Set(String::New(CAST(L"GetHeight")), FunctionTemplate::New(GetWindowHeight), PropertyAttribute::ReadOnly);
  window->Set(String::New(CAST(L"GetWidth")), FunctionTemplate::New(GetWindowWidth), PropertyAttribute::ReadOnly);

  return handleScope.Close(nCore);
}
