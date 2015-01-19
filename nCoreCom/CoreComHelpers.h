//-------------------------------------------------------------------------------------------------
// /nCoreCom/CoreComHelpers.h
// The nModules Project
//
// Helper functions for nCoreCom, essentially anything that should only be used by CoreCom
// routines.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "../nShared/ResultCodes.h"

#include "../Utilities/Common.h"

// Used for initalizing a core function
#define FUNC_VAR_NAME(func) _sp ## func
#define INIT_FUNC(func) \
  FUNC_VAR_NAME(func) = decltype(FUNC_VAR_NAME(func))(GetProcAddress(hCoreInstance,#func)); \
  if (FUNC_VAR_NAME(func) == nullptr) return E_NCORECOM_FUNC_INIT;
#define DECL_FUNC_VAR(func) static decltype(func) __cdecl * FUNC_VAR_NAME(func);

namespace nCore {
  namespace System {
    HRESULT _Init(HMODULE);
    void _DeInit();
  }
}
