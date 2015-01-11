//-------------------------------------------------------------------------------------------------
// /Utilities/Common.h
// The nModules Project
//
// Common header used for including windows.h
//-------------------------------------------------------------------------------------------------
#pragma once

#if defined(_WINDOWS_) || defined(_WINDOWS_H)
#  error Do not include Windows.h before this header
#endif

// Window target versions
#define _WIN32_WINNT    _WIN32_WINNT_WINBLUE
#define WINVER          _WIN32_WINNT_WINBLUE
#define _WIN32_IE       _WIN32_IE_IE100
#define NTDDI_VERSION   NTDDI_WINBLUE

// Window header options
#define NOCOMM
#define NOCRYPT
#define NOMCX
#define NOMINMAX
#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "Debugging.h"
#include "Macros.h"
