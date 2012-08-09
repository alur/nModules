/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Core.cpp                                                        July, 2012
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
int (__stdcall * _pGetVersion)();


/// <summary>
/// Initalizes the core communications.
/// </summary>
/// <param name="minVersion">The minimum required core version.</param>
/// <returns>True if the core is succefully initalized.</returns>
HRESULT nCore::Init(VERSION minVersion) {
    HWND coreHWnd = FindWindow("LSnCore", NULL);
    if (!coreHWnd)
        return E_FAIL;

    initialized = false;

    HMODULE hCoreInstance = (HMODULE)GetWindowLongPtr(coreHWnd, GWLP_USERDATA);
    
    INIT_FUNC(_pGetVersion,int (__stdcall *)(),"GetCoreVersion")

    // Should check the version here
    VERSION coreVersion = GetVersion();
    if (GetMajorVersion(minVersion) != GetMajorVersion(coreVersion))
        return E_FAIL;

    HRESULT hr;
    if (!SUCCEEDED(hr = System::Init(hCoreInstance))) return hr;
    if (!SUCCEEDED(hr = InputParsing::Init(hCoreInstance))) return hr;

    initialized = true;

    return S_OK;
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
