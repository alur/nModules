/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ScriptingEvents.cpp
 *  The nModules Project
 *
 *  Manages the nCore Object.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../Utilities/Common.h"
#include "../External/v8/include/v8.h"
#include "ScriptingEvents.h"
#include "ScriptingHelpers.h"


using namespace v8;


/// <summary>
/// Creates the LiteStep object.
/// </summary>
Handle<ObjectTemplate> Scripting::Events::Initialize(Isolate *isolate) {
  // Create a stack-allocated handle scope.
  HandleScope handleScope(isolate);

  //
  // General
  //
  Handle<ObjectTemplate> events = ObjectTemplate::New();

  //events->Set(,,PropertyAttribute::DontEnum)

  ////
  //events->Set(String::New(CAST(L"AddHandler")), FunctionTemplate::New([] (const FunctionCallbackInfo<Value> &args) -> void
  //{
  //    // Check argument types
  //    if (args.Length() != 3 || !args[0]->IsString() || !args[1]->IsString() || !args[2]->IsFunction())
  //    {
  //        return;
  //    }

  //    //

  //}));


  return handleScope.Close(events);
}
