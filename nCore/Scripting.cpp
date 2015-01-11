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
#include "ScriptingNCore.h"
#include <fstream>
#include <streambuf>
#include "ScriptingHelpers.h"
#include <strsafe.h>


using namespace v8;


//
Persistent<Context> gContext;


/// <summary>
/// Executes a piece of JavaScript code in the global context.
/// </summary>
static void RunCode(LPCWSTR code, void(*callback)(Handle<Value>), LPCWSTR fileName = nullptr) {
  Isolate * isolate = Isolate::GetCurrent();
  HandleScope handleScope(isolate);
  Context::Scope contextScope(isolate, gContext);

  TryCatch tryCatch;
  Handle<Script> script = fileName ?
    Script::Compile(String::New(CAST(code)), String::New(CAST(fileName))) :
    Script::Compile(String::New(CAST(code)));

  if (script.IsEmpty()) {
    if (tryCatch.HasCaught()) {
      Handle<Message> message = tryCatch.Message();
      String::Value exception(tryCatch.Exception());
      if (message.IsEmpty()) {
        MessageBox(nullptr, CAST(*exception), L"nScript Error", MB_OK);
      } else {
        WCHAR msg[MAX_LINE_LENGTH];

        String::Value fileName(message->GetScriptResourceName());
        StringCchPrintfW(msg, _countof(msg), L"%s\nLine %d of %s", *exception,
          message->GetLineNumber(), *fileName);

        MessageBox(nullptr, msg, L"nScript Error", MB_OK);
      }
    }
    return;
  }

  // Run the script to get the result.
  Handle<Value> result = script->Run();
  if (result.IsEmpty()) {
    if (tryCatch.HasCaught()) {
      Handle<Message> message = tryCatch.Message();
      String::Value exception(tryCatch.Exception());

      if (message.IsEmpty()) {
        MessageBox(nullptr, CAST(*exception), L"nScript Exception", MB_OK);
      } else {
        String::Value fileName(message->GetScriptResourceName());
        int lineNum = message->GetLineNumber();

        WCHAR msg[MAX_LINE_LENGTH];

        StringCchPrintfW(msg, _countof(msg), L"%ls\nLine %d of %ls", *exception, lineNum, *fileName);

        MessageBox(nullptr, msg, L"nScript Exception", MB_OK);
      }
    }
    return;
  }

  if (callback) {
    callback(result);
  }
}


/// <summary>
/// 
/// </summary>
static void InitV8() {
  Isolate * isolate = Isolate::GetCurrent();
  HandleScope handleScope(isolate);

  Handle<ObjectTemplate> global = ObjectTemplate::New();
  global->Set(String::New(CAST(L"LiteStep")),
    Scripting::LSCore::Initialize(isolate), PropertyAttribute::ReadOnly);
  global->Set(String::New(CAST(L"nCore")),
    Scripting::NCore::Initialize(isolate), PropertyAttribute::ReadOnly);

  gContext.Reset(isolate, Context::New(isolate, nullptr, global));
}


/// <summary>
/// 
/// </summary>
void Scripting::Initialize() {
  // Initalize V8
  InitV8();

  // Add bangs for running scripts
  LiteStep::AddBangCommand(L"!nAlertScript", [] (HWND, LPCTSTR code) {
    RunCode(code, [] (Handle<Value> value) {
      String::Value result(value);
      MessageBox(nullptr, CAST(*result), L"nScript", MB_OK);
    }, L"!nAlertScript");
  });
  LiteStep::AddBangCommand(L"!nExecScript", [] (HWND, LPCTSTR code) {
    RunCode(code, nullptr, L"!nExecScript");
  });

  // Load script files
  LiteStep::IterateOverLineTokens(L"*nIncludeScript", [] (LPCTSTR file) {
    std::wifstream t(file);
    std::wstring str;

    t.seekg(0, std::ios::end);
    str.reserve(std::wstring::size_type(t.tellg()));
    t.seekg(0, std::ios::beg);

    str.assign(std::istreambuf_iterator<wchar_t>(t), std::istreambuf_iterator<wchar_t>());

    RunCode(str.c_str(), nullptr, file);
  });
}


/// <summary>
/// 
/// </summary>
void Scripting::Shutdown() {
  // Remove bang commands
  LiteStep::RemoveBangCommand(L"!nAlertScript");
  LiteStep::RemoveBangCommand(L"!nExecScript");
  LSCore::Shutdown();

  // Dispose of our context, killing all JS data
  gContext.Dispose();
}
