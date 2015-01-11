//-------------------------------------------------------------------------------------------------
// /nCoreCom/Core.cpp
// The nModules Project
//
// Handles communication with nCore.
//-------------------------------------------------------------------------------------------------
#include "Core.h"
#include "CoreComHelpers.h"

#include "../nShared/LiteStep.h"
#include "../nShared/ResultCodes.h"

#include "../Utilities/Common.h"

// Set to true when the core is succesfully initalized.
static bool sInitialized = false;

// Pointers to the functions in the core.
namespace nCore {
  DECL_FUNC_VAR(GetCoreVersion);
  DECL_FUNC_VAR(RegisterForCoreMessages);
  DECL_FUNC_VAR(UnregisterForCoreMessages);
}

/// <summary>
/// Initalizes communication with the core.
/// </summary>
/// <param name="minVersion">The minimum core version.</param>
/// <returns>S_OK if the core is succefully initalized.</returns>
HRESULT nCore::Connect(VERSION minVersion) {
  HWND coreWindow = FindWindowEx(nullptr, nullptr, L"LSnCore", nullptr);

  ASSERT(!sInitialized);

  if (coreWindow == nullptr) {
    return E_NCORECOM_CORE_NOT_FOUND;
  }

  HMODULE hCoreInstance = (HMODULE)GetWindowLongPtr(coreWindow, GWLP_USERDATA);

  INIT_FUNC(GetCoreVersion);

  VERSION coreVersion = FUNC_VAR_NAME(GetCoreVersion)();
  // The major version must be the same
  if (GetMajorVersion(minVersion) != GetMajorVersion(coreVersion)) {
    return E_NCORECOM_MAJOR_VERSION;
  }
  // The minor version must be >=
  if (GetMinorVersion(minVersion) > GetMinorVersion(coreVersion)) {
    return E_NCORECOM_MINOR_VERSION;
  }
  // If the minor version ==, the patch version must be >=
  if (GetMinorVersion(minVersion) == GetMinorVersion(coreVersion) &&
    GetPatchVersion(minVersion) > GetPatchVersion(minVersion)) {
    return E_NCORECOM_PATCH_VERSION;
  }

  INIT_FUNC(RegisterForCoreMessages);
  INIT_FUNC(UnregisterForCoreMessages);

  HRESULT hr = S_OK;

  RETURNONFAIL(hr, System::_Init(hCoreInstance));

  sInitialized = true;

  return hr;
}

/// <summary>
/// Disconnects from the core.
/// </summary>
void nCore::Disconnect() {
  System::_DeInit();
  FUNC_VAR_NAME(GetCoreVersion) = nullptr;
  sInitialized = false;
}

/// <summary>
/// Returns true when the core is initalized.
/// </summary>
/// <returns>True if the core is initalized.</returns>
bool nCore::Initialized() {
  return sInitialized;
}

/// <summary>
/// Retrives the version of the loaded core.
/// </summary>
VERSION nCore::GetCoreVersion() {
  ASSERT(nCore::Initialized());
  return FUNC_VAR_NAME(GetCoreVersion)();
}

void nCore::RegisterForCoreMessages(HWND hwnd, const UINT messages[]) {
  ASSERT(nCore::Initialized());
  FUNC_VAR_NAME(RegisterForCoreMessages)(hwnd, messages);
}

void nCore::UnregisterForCoreMessages(HWND hwnd, const UINT messages[]) {
  ASSERT(nCore::Initialized());
  FUNC_VAR_NAME(UnregisterForCoreMessages)(hwnd, messages);
}
