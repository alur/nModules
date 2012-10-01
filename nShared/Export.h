/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Export.h
 *  The nModules Project
 *
 *  Some defines to simplify DLL export declarations.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#define EXPORT_CDECL(type) EXTERN_C __declspec(dllexport) type __cdecl
#define EXPORT_STDCALL(type) EXTERN_C __declspec(dllexport) type __stdcall
