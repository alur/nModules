/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ScriptingNCore.h
 *  The nModules Project
 *
 *  Manages the nCore object.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

namespace Scripting {
  namespace NCore {
    v8::Handle<v8::ObjectTemplate> Initialize(v8::Isolate *isolate);
  }
}
