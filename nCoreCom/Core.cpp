/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Core.cpp
 *  The nModules Project
 *
 *  Handles communication with nCore.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../Utilities/Common.h"
#include "Core.h"
#include "CoreComHelpers.h"

using namespace nCore;

// Set to true when the core is succesfully initalized.
static bool gInitialized = false;

// Pointers to the functions in the core.
namespace nCore
{
    DECL_FUNC_VAR(GetCoreVersion);
}


/// <summary>
/// Initalizes communication with the core.
/// </summary>
/// <param name="minVersion">The minimum core version.</param>
/// <returns>S_OK if the core is succefully initalized.</returns>
HRESULT nCore::Connect(VERSION minVersion)
{
    HWND coreHWnd = FindWindow(_T("LSnCore"), nullptr);

    if (!coreHWnd)
    {
        return E_FAIL;
    }
    
    gInitialized = false;

    HMODULE hCoreInstance = (HMODULE)GetWindowLongPtr(coreHWnd, GWLP_USERDATA);
    
    INIT_FUNC(GetCoreVersion);

    // Should check the version here
    VERSION coreVersion = GetCoreVersion();
    // The major version must be the same
    if (GetMajorVersion(minVersion) != GetMajorVersion(coreVersion))
    {
        return E_FAIL;
    }
    // The minor version must be >=
    if (GetMinorVersion(minVersion) > GetMinorVersion(coreVersion))
    {
        return E_FAIL;
    }
    // If the minor version ==, the patch version must be >=
    if (GetMinorVersion(minVersion) == GetMinorVersion(coreVersion) && GetPatchVersion(minVersion) > GetPatchVersion(minVersion))
    {
        return E_FAIL;
    }
    
    HRESULT hr = S_OK;

    RETURNONFAIL(hr, System::_Init(hCoreInstance));

    gInitialized = true;

    return hr;
}


/// <summary>
/// Disconnects from the core.
/// </summary>
void nCore::Disconnect()
{
    System::_DeInit();
    FUNC_VAR_NAME(GetCoreVersion) = nullptr;
    gInitialized = false;
}


/// <summary>
/// Returns true when the core is initalized.
/// </summary>
/// <returns>True if the core is initalized.</returns>
bool nCore::Initialized()
{
    return gInitialized;
}


/// <summary>
/// Retrives the version of the loaded core.
/// </summary>
VERSION nCore::GetCoreVersion()
{
    return FUNC_VAR_NAME(GetCoreVersion)();
}
