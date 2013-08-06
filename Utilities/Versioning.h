/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Versioning.h
 *  The nModules Project
 *
 *  Functions for dealing with versioning.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

typedef unsigned __int64 VERSION;
typedef unsigned __int16 VERSION_PART;

VERSION MakeVersion(VERSION_PART major, VERSION_PART minor, VERSION_PART patch, VERSION_PART build);
VERSION_PART GetMajorVersion(VERSION);
VERSION_PART GetMinorVersion(VERSION);
VERSION_PART GetPatchVersion(VERSION);
VERSION_PART GetBuildVersion(VERSION);
HRESULT GetVersionString(VERSION, LPTSTR, size_t, bool=false);
