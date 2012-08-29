/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Versioning.cpp                                                  July, 2012
 *  The nModules Project
 *
 *  Functions for dealing with versioning.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include <Windows.h>
#include "Versioning.h"


/// <summary>
/// Gets the major version
/// </summary>
unsigned short GetMajorVersion(VERSION v) {
    return (unsigned short)((v & 0xFFFF000000000000) >> 48);
}


/// <summary>
/// Gets the minor version
/// </summary>
unsigned short GetMinorVersion(VERSION v) {
    return (unsigned short)((v & 0xFFFF00000000) >> 32);
}


/// <summary>
/// Gets the patch version
/// </summary>
unsigned short GetPatchVersion(VERSION v) {
    return (unsigned short)((v & 0xFFFF0000) >> 16);
}


/// <summary>
/// Gets the build version
/// </summary>
unsigned short GetBuildVersion(VERSION v) {
    return (unsigned short)(v & 0xFFFF);
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
