/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Factories.cpp
 *  The nModules Project
 *
 *  Manages Direct2D/DirectWrite/... factories
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include "Factories.h"
#include "Macros.h"

namespace Factories {
    IDWriteFactory *pDWFactory = nullptr;
    ID2D1Factory *pD2DFactory = nullptr;
    IWICImagingFactory *pWICFactory = nullptr;
}


/// <summary>
/// Releases all allocated factories.
/// </summary>
void Factories::Release() {
    SAFERELEASE(pDWFactory);
    SAFERELEASE(pD2DFactory);
    SAFERELEASE(pWICFactory);
}


/// <summary>
/// Fetches a directwrite factory.
/// </summary>
HRESULT Factories::GetDWriteFactory(LPVOID* ppFactory) {
    if (pDWFactory == nullptr) {
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
    if (pD2DFactory == nullptr) {
        D2D1_FACTORY_OPTIONS d2dFactoryOptions;
        HRESULT hr;
        ZeroMemory(&d2dFactoryOptions, sizeof(D2D1_FACTORY_OPTIONS));
        #if defined(_DEBUG)
            // If the project is in a debug build, enable Direct2D debugging via SDK Layers.
            d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
        #endif

        if (!SUCCEEDED(hr = D2D1CreateFactory(
            D2D1_FACTORY_TYPE_MULTI_THREADED,
            __uuidof(ID2D1Factory),
            &d2dFactoryOptions,
            reinterpret_cast<LPVOID*>(&pD2DFactory)
        ))) return hr;
    }
    *ppFactory = pD2DFactory;
    return S_OK;
}


/// <summary>
/// Fetches a WIC factory.
/// </summary>
HRESULT Factories::GetWICFactory(LPVOID* ppFactory) {
    if (pWICFactory == nullptr) {
        HRESULT hr;

        if (!SUCCEEDED(hr = CoCreateInstance(
            CLSID_WICImagingFactory1,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_IWICImagingFactory,
            (LPVOID*)&pWICFactory
        ))) return hr;
    }
    *ppFactory = pWICFactory;
    return S_OK;
}
