/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *  ScriptingLSCore.cpp
 *  The nModules Project
 *
 *  Manages the LiteStep object.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../External/v8/include/v8.h"
#include "ScriptingLSCore.h"
#include "ScriptingHelpers.h"
#include <map>
#include <list>


using namespace v8;


// All bangs currently registered by scripts.
static std::map<std::wstring, Persistent<Function>> gScriptBangs;

extern Persistent<Context> gContext;


/// <summary>
/// Called by LiteStep in order to execute a particular bang, with a particular set of params.
/// </summary>
static void ScriptBangThump(HWND, LPCTSTR bang, LPCTSTR params) {
  auto iter = gScriptBangs.find(bang);
  if (iter != gScriptBangs.end()) {
    Isolate * isolate = Isolate::GetCurrent();
    HandleScope handle_scope(isolate);
    Handle<Value> arg = String::New(CAST(params));
    Local<Function>::New(isolate, iter->second)
        ->Call(Local<Context>::New(isolate, gContext)->Global(), 1, &arg);
  }
}


static void Execute(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() != 1)
    return;
  String::Value command(args[0]);
  LiteStep::LSExecute(nullptr, CAST(*command), 0);
}


static void Recycle(const FunctionCallbackInfo<Value> & args) {
  UNREFERENCED_PARAMETER(args);
  HWND hLiteStep = LiteStep::GetLitestepWnd();
  if (hLiteStep)
    PostMessage(hLiteStep, 9260, 0, 0);
}


static void Refresh(const FunctionCallbackInfo<Value> & args) {
  UNREFERENCED_PARAMETER(args);
  LiteStep::LSExecuteEx(nullptr, L"", L"!Refresh", L"", L"", 0);
}


static void AddBang(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() != 2)
    return;

  String::Value value(args[0]);
  LPCWSTR bang = CAST(*value);

  // Following the behavior of LS's AddBandCommand, Add overwrites previous bangs.
  if (gScriptBangs.find(bang) != gScriptBangs.end())
    gScriptBangs[bang].Dispose();

  gScriptBangs[bang].Reset(Isolate::GetCurrent(), args[1].As<Function>());

  LiteStep::AddBangCommandEx(bang, ScriptBangThump);
}


static void ExecuteBang(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() != 1 && args.Length() != 2) {
    return;
  }

  String::Value bang(args[0]);

  if (args.Length() == 2) {
    String::Value params(args[1]);
    LiteStep::LSExecuteEx(nullptr, L"", CAST(*bang), CAST(*params), L"", 0);
  } else {
    LiteStep::LSExecuteEx(nullptr, L"", CAST(*bang), L"", L"", 0);
  }
}


static void ListBangs(const FunctionCallbackInfo<Value> & args) {
  HandleScope handleScope(Isolate::GetCurrent());

  std::list<std::wstring> bangs;
  LiteStep::EnumLSData(ELD_BANGS, (FARPROC)(LiteStep::ENUMBANGSPROC) [] (LPCWSTR bang, LPARAM list) -> BOOL {
    ((std::list<std::wstring>*)list)->push_back(bang);
    return TRUE;
  }, (LPARAM)&bangs);

  Handle<Array> ret = Array::New((int)bangs.size());
  int i = 0;
  for (auto & bang : bangs) {
    ret->Set(i++, String::New(CAST(bang.c_str())));
  }

  args.GetReturnValue().Set(ret);
}


static void RemoveBang(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() != 1) {
    return;
  }

  String::Value value(args[0]);
  LPCWSTR bang = CAST(*value);

  auto iter = gScriptBangs.find(bang);
  if (iter != gScriptBangs.end()) {
    iter->second.Dispose();
    gScriptBangs.erase(iter);
    LiteStep::RemoveBangCommand(bang);
  }
}


static void GetEvar(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() != 1)
    return;

  String::Value key(args[0]);

  wchar_t buf[MAX_LINE_LENGTH];
  LiteStep::GetRCLine(CAST(*key), buf, _countof(buf), nullptr);

  args.GetReturnValue().Set(String::New(CAST(buf)));
}


static void GetNumericEvar(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() != 1 && args.Length() != 2)
    return;

  String::Value key(args[0]);
  DOUBLE defaultValue = 0.0;

  if (args.Length() == 2) {
    defaultValue = (*args[1].As<Number>())->Value();
  }

  args.GetReturnValue().Set(Number::New(LiteStep::GetRCDouble(CAST(*key), defaultValue)));
}


static void SetEvar(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() != 2)
    return;

  String::Value key(args[0]);
  String::Value value(args[1]);

  args.GetReturnValue().Set(LiteStep::LSSetVariable(CAST(*key), CAST(*value)));
}


static void ListModules(const FunctionCallbackInfo<Value> & args) {
  HandleScope handleScope(Isolate::GetCurrent());

  std::list<std::wstring> modules;
  LiteStep::EnumLSData(ELD_MODULES, (FARPROC)(LiteStep::ENUMMODULESPROC) [] (LPCWSTR location, DWORD, LPARAM list) -> BOOL {
    ((std::list<std::wstring>*)list)->push_back(location);
    return TRUE;
  }, (LPARAM)&modules);

  Handle<Array> ret = Array::New((int)modules.size());
  int i = 0;
  for (auto & module : modules) {
    ret->Set(i++, String::New(CAST(module.c_str())));
  }

  args.GetReturnValue().Set(ret);
}


static void ReloadModule(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() != 1)
    return;

  String::Value module(args[0]);

  SendMessage(LiteStep::GetLitestepWnd(), LM_RELOADMODULE, (WPARAM)CAST(*module), LMM_PATH);
}


static void UnloadModule(const FunctionCallbackInfo<Value> & args) {
  if (args.Length() != 1)
    return;

  String::Value module(args[0]);

  SendMessage(LiteStep::GetLitestepWnd(), LM_UNLOADMODULE, (WPARAM)CAST(*module), LMM_PATH);
}


/// <summary>
/// Creates the LiteStep object.
/// </summary>
Handle<ObjectTemplate> Scripting::LSCore::Initialize(Isolate *isolate) {
  HandleScope handleScope(isolate);

  #pragma push_macro("ADD_FUNC")
  #define ADD_FUNC(object, name, func) (object)->Set(String::New(CAST(name)), \
    FunctionTemplate::New(func), PropertyAttribute::ReadOnly)

  Handle<ObjectTemplate> liteStep = ObjectTemplate::New();
  ADD_FUNC(liteStep, L"Execute", Execute);
  ADD_FUNC(liteStep, L"Recycle", Recycle);
  ADD_FUNC(liteStep, L"Refresh", Refresh);

  Handle<ObjectTemplate> bangs = ObjectTemplate::New();
  ADD_FUNC(bangs, L"Add", AddBang);
  ADD_FUNC(bangs, L"Execute", ExecuteBang);
  ADD_FUNC(bangs, L"List", ListBangs);
  ADD_FUNC(bangs, L"Remove", RemoveBang);
  liteStep->Set(String::New(CAST(L"Bangs")), bangs, PropertyAttribute::ReadOnly);

  Handle<ObjectTemplate> evars = ObjectTemplate::New();
  ADD_FUNC(evars, L"Get", GetEvar);
  ADD_FUNC(evars, L"GetNumeric", GetNumericEvar);
  ADD_FUNC(evars, L"Set", SetEvar);
  liteStep->Set(String::New(CAST(L"Evars")), evars, PropertyAttribute::ReadOnly);

  Handle<ObjectTemplate> modules = ObjectTemplate::New();
  ADD_FUNC(modules, L"List", ListModules);
  ADD_FUNC(modules, L"Reload", ReloadModule);
  ADD_FUNC(modules, L"Unload", UnloadModule);
  liteStep->Set(String::New(CAST(L"Modules")), modules, PropertyAttribute::ReadOnly);

  #pragma pop_macro("ADD_FUNC")

  return handleScope.Close(liteStep);
}


/// <summary>
/// Removes script-generated bang commands and disposes of their contexts.
/// </summary>
void Scripting::LSCore::Shutdown() {
  for (auto &item : gScriptBangs) {
    item.second.Dispose();
    LiteStep::RemoveBangCommand(item.first.c_str());
  }
}
