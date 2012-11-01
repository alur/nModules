/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Core.cpp
 *  The nModules Project
 *
 *  Handles communication with nCore.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <Windows.h>
#include "Core.h"

// Set to true when the core is succesfully initalized.
bool initialized = false;

// Pointers to the functions in the core.
VERSION (__stdcall * _pGetVersion)();


/// <summary>
/// Initalizes communication with the core.
/// </summary>
/// <param name="minVersion">The minimum core version.</param>
/// <returns>S_OK if the core is succefully initalized.</returns>
HRESULT nCore::Connect(VERSION minVersion) {
    HWND coreHWnd = FindWindow("LSnCore", NULL);

    if (!coreHWnd) {
        return E_FAIL;
    }
    
    initialized = false;

    HMODULE hCoreInstance = (HMODULE)GetWindowLongPtr(coreHWnd, GWLP_USERDATA);
    
    INIT_FUNC(_pGetVersion,VERSION (__stdcall *)(),"GetCoreVersion");

    // Should check the version here
    VERSION coreVersion = GetVersion();
    // The major version must be the same
    if (GetMajorVersion(minVersion) != GetMajorVersion(coreVersion)) {
        return E_FAIL;
    }
    // The minor version must be >=
    if (GetMinorVersion(minVersion) > GetMinorVersion(coreVersion)) {
        return E_FAIL;
    }
    // If the minor version ==, the patch version must be >=
    if (GetMinorVersion(minVersion) == GetMinorVersion(coreVersion) && GetPatchVersion(minVersion) > GetPatchVersion(minVersion)) {
        return E_FAIL;
    }
    
    HRESULT hr = S_OK;

    if (!SUCCEEDED(hr = System::Init(hCoreInstance))) return hr;
    if (!SUCCEEDED(hr = InputParsing::Init(hCoreInstance))) return hr;

    initialized = true;

    return S_OK;
}


/// <summary>
/// Disconnects from the core.
/// </summary>
void nCore::Disconnect() {
}


/// <summary>
/// Returns true when the core is initalized.
/// </summary>
/// <returns>True if the core is initalized.</returns>
bool nCore::Initalized() {
    return initialized;
}


/// <summary>
/// Retrives the version of the loaded core.
/// </summary>
VERSION nCore::GetVersion() {
    return _pGetVersion();
}
