/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	Versioning.cpp													July, 2012
 *	The nModules Project
 *
 *	Functions for dealing with versioning.
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include <Windows.h>
#include "Versioning.h"

/// <summary>
/// Macro for making a VERSION from 4 bytes.
/// </summary>
#define MAKE_VERSION(major,minor,patch,build) ((major << 24) | (minor << 16) | (patch << 8) | build)

/// <summary>
/// Gets the major version
/// </summary>
unsigned __int8 GetMajorVersion(VERSION v) {
	return (unsigned __int8)((v & 0xFF000000) >> 24);
}

/// <summary>
/// Gets the minor version
/// </summary>
unsigned __int8 GetMinorVersion(VERSION v) {
	return (unsigned __int8)((v & 0xFF0000) >> 16);
}

/// <summary>
/// Gets the patch version
/// </summary>
unsigned __int8 GetPatchVersion(VERSION v) {
	return (unsigned __int8)((v & 0xFF00) >> 8);
}

/// <summary>
/// Gets the build version
/// </summary>
unsigned __int8 GetBuildVersion(VERSION v) {
	return (unsigned __int8)(v & 0xFF);
}

/// <summary>
/// Creates a string based on a version.
/// </summary>
HRESULT GetVersionString(VERSION v, LPSTR pszDest, size_t cchLength, bool bIncludeBuild) {
	if (bIncludeBuild) {
		return StringCchPrintf(pszDest, cchLength, "%u.%u.%u", GetMajorVersion(v), GetMinorVersion(v), GetPatchVersion(v));
	}
	else {
		return StringCchPrintf(pszDest, cchLength, "%u.%u.%u.%u", GetMajorVersion(v), GetMinorVersion(v), GetPatchVersion(v), GetBuildVersion(v));
	}
}
