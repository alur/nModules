/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Versioning.h                                                    July, 2012
 *  The nModules Project
 *
 *  Functions for dealing with versioning.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef VERSIONING_H
#define VERSIONING_H

typedef DWORD VERSION;

unsigned __int8 GetMajorVersion(VERSION);
unsigned __int8 GetMinorVersion(VERSION);
unsigned __int8 GetPatchVersion(VERSION);
unsigned __int8 GetBuildVersion(VERSION);
HRESULT GetVersionString(VERSION, LPSTR, size_t, bool=false);

#endif /* VERSIONING_H */
