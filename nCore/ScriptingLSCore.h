/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ScriptingLSCore.h
 *  The nModules Project
 *
 *  Manages the LiteStep object.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

namespace Scripting {
  namespace LSCore {
    v8::Handle<v8::ObjectTemplate> Initialize(v8::Isolate * isolate);
    void Shutdown();
  }
}
