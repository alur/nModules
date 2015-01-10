/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ScriptingHelpers.h
 *  The nModules Project
 *
 *  Utility functions used by the scripting API.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

// Can't use /Zc:wchar_t- due to linking errors :/
inline const uint16_t* CAST(LPCWSTR x) { return (uint16_t*)x; }
inline const LPCWSTR CAST(uint16_t* x) { return (LPCWSTR)x; }
