//-------------------------------------------------------------------------------------------------
// /Utilities/Versioning.h
// The nModules Project
//
// Functions for dealing with versioning.
//-------------------------------------------------------------------------------------------------
#pragma once

typedef unsigned __int64 VERSION;
typedef unsigned __int16 VERSION_PART;

/// <summary>
/// Creates a version from its constituent parts.
/// </summary>
VERSION MakeVersion(VERSION_PART major, VERSION_PART minor, VERSION_PART patch, VERSION_PART build);

/// <summary>
/// Gets the major version
/// </summary>
VERSION_PART GetMajorVersion(VERSION);

/// <summary>
/// Gets the minor version
/// </summary>
VERSION_PART GetMinorVersion(VERSION);

/// <summary>
/// Gets the patch version
/// </summary>
VERSION_PART GetPatchVersion(VERSION);

/// <summary>
/// Gets the build version
/// </summary>
VERSION_PART GetBuildVersion(VERSION);

/// <summary>
/// Creates a string based on a version.
/// </summary>
HRESULT GetVersionString(VERSION, LPTSTR, size_t, bool = false);
