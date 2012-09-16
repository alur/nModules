/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Factories.cpp                                                   July, 2012
 *  The nModules Project
 *
 *  Manages Direct2D/DirectWrite/... factories
 *   
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


/// <summary>
/// Releases all allocated factories.
/// </summary>
void Factories::Release() {
    SAFERELEASE(&pDWFactory);
    SAFERELEASE(&pD2DFactory);
    SAFERELEASE(&pWICFactory);
}


/// <summary>
/// Fetches a directwrite factory.
/// </summary>
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


/// <summary>
/// Fetches a direct2d factory.
/// </summary>
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


/// <summary>
/// Fetches a WIC factory.
/// </summary>
HRESULT Factories::GetWICFactory(LPVOID* ppFactory) {
    if (!pWICFactory) {
        HRESULT hr;
        if (!SUCCEEDED(hr = CoCreateInstance(
            CLSID_WICImagingFactory1,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IWICImagingFactory,
            (LPVOID*)&pWICFactory
        ))) return hr;
    }
    *ppFactory = pWICFactory;
    return S_OK;
}
