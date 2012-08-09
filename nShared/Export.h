/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Export.h                                                        July, 2012
 *  The nModules Project
 *
 *  Some defines to simplify DLL export declarations.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef EXPORT_H
#define EXPORT_H

#define EXPORT_CDECL(type) EXTERN_C __declspec(dllexport) type __cdecl
#define EXPORT_STDCALL(type) EXTERN_C __declspec(dllexport) type __stdcall

#endif /* EXPORT_H */
