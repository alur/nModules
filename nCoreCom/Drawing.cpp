/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	Drawing.cpp														July, 2012
 *	The nModules Project
 *
 *	Provides calls to the drawing functions of the core.
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <Windows.h>
#include "Core.h"

// Pointers to the functions in the core.
HRESULT (__cdecl * _pGetD2DFactory)(LPVOID *);
HRESULT (__cdecl * _pGetDWriteFactory)(LPVOID *);
HRESULT (__cdecl * _pGetWICFactory)(LPVOID *);

/// <summary>
/// Initalizes the core communications.
/// </summary>
/// <returns>True if the core is succefully initalized.</returns>
HRESULT nCore::Drawing::Init(HMODULE hCoreInstance) {
	INIT_FUNC(_pGetD2DFactory,HRESULT (__cdecl *)(LPVOID *),"GetD2DFactory")
	INIT_FUNC(_pGetDWriteFactory,HRESULT (__cdecl *)(LPVOID *),"GetDWriteFactory")
	INIT_FUNC(_pGetWICFactory,HRESULT (__cdecl *)(LPVOID *),"GetWICFactory")

	return S_OK;
}


HRESULT nCore::Drawing::GetD2DFactory(LPVOID *ppFactory) {
	return _pGetD2DFactory(ppFactory);
}

HRESULT nCore::Drawing::GetDWriteFactory(LPVOID *ppFactory) {
	return _pGetDWriteFactory(ppFactory);
}

HRESULT nCore::Drawing::GetWICFactory(LPVOID *ppFactory) {
	return _pGetWICFactory(ppFactory);
}