/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Common.h
 *  The nModules Project
 *
 *  Common header used for including windows.h
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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
#define WIN32_LEAN_AND_MEAN
#define STRICT
#define NOCRYPT
#define NOMINMAX
#define NOCOMM
#define NOMCX

#include <Windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <strsafe.h>
#include "Macros.h"
#include "Debugging.h"
#include <string>

namespace std {
#if defined(_UNICODE)
    typedef wstring tstring;
#else
    typedef string tstring;
#endif
}