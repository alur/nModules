/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Versioning.h                                                    July, 2012
 *  The nModules Project
 *
 *  Functions for dealing with versioning.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef VERSIONING_H
#define VERSIONING_H

typedef DWORDLONG VERSION;

/// <summary>
/// Macro for making a VERSION from 4 shorts.
/// </summary>
#define MAKE_VERSION(major,minor,patch,build) (((VERSION)major << 48) | ((VERSION)minor << 32) | ((VERSION)patch << 16) | (VERSION)build)

unsigned short GetMajorVersion(VERSION);
unsigned short GetMinorVersion(VERSION);
unsigned short GetPatchVersion(VERSION);
unsigned short GetBuildVersion(VERSION);
HRESULT GetVersionString(VERSION, LPSTR, size_t, bool=false);

#endif /* VERSIONING_H */
