/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ScriptingEvents.h
 *  The nModules Project
 *
 *  Manages the LiteStep object.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

namespace Scripting {
  namespace Events {
    v8::Handle<v8::ObjectTemplate> Initialize(v8::Isolate *isolate);
  }
}
