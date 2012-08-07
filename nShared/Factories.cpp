/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	Painting.cpp													July, 2012
 *	The nModules Project
 *
 *	Utility functions for painting.
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include "Factories.h"

namespace Factories {
	IDWriteFactory *pDWFactory = NULL;
	ID2D1Factory *pD2DFactory = NULL;
	IWICImagingFactory *pWICFactory = NULL;
}

void Factories::Release() {
	if (pDWFactory) { pDWFactory->Release(); pDWFactory = NULL; }
	if (pD2DFactory) { pD2DFactory->Release(); pD2DFactory = NULL; }
	if (pWICFactory) { pWICFactory->Release(); pWICFactory = NULL; }
}

HRESULT Factories::GetDWriteFactory(LPVOID* ppFactory) {
	if (!pDWFactory) {
		HRESULT hr;
		if (!SUCCEEDED(hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&pDWFactory)
		))) return hr;
	}
	*ppFactory = pDWFactory;
	return S_OK;
}

HRESULT Factories::GetD2DFactory(LPVOID* ppFactory) {
	if (!pD2DFactory) {
		HRESULT hr;
		if (!SUCCEEDED(hr = D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			&pD2DFactory
		))) return hr;
	}
	*ppFactory = pD2DFactory;
	return S_OK;
}

HRESULT Factories::GetWICFactory(LPVOID* ppFactory) {
	if (!pWICFactory) {
		HRESULT hr;
		if (!SUCCEEDED(hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			(LPVOID*)&pWICFactory
		))) return hr;
	}
	*ppFactory = pWICFactory;
	return S_OK;
}
