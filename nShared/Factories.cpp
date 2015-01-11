/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Factories.cpp
 *  The nModules Project
 *
 *  Manages Direct2D/DirectWrite/... factories
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Factories.h"

#include "../Utilities/CommonD2D.h"

#include <dwrite.h>
#include <wincodec.h>


static IDWriteFactory *sDWFactory = nullptr;
static ID2D1Factory *sD2DFactory = nullptr;
static IWICImagingFactory *sWICFactory = nullptr;


/// <summary>
/// Releases all allocated factories.
/// </summary>
void Factories::Release() {
  SAFERELEASE(sDWFactory);
  SAFERELEASE(sD2DFactory);
  SAFERELEASE(sWICFactory);
}


/// <summary>
/// Fetches a DirectWrite factory.
/// </summary>
HRESULT Factories::GetDWriteFactory(LPVOID *factory) {
  if (sDWFactory == nullptr) {
    HRESULT hr;
    RETURNONFAIL(hr, DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
      reinterpret_cast<IUnknown**>(&sDWFactory)));
  }
  *factory = sDWFactory;
  return S_OK;
}


/// <summary>
/// Fetches a Direct2D factory.
/// </summary>
HRESULT Factories::GetD2DFactory(LPVOID *factory) {
  if (sD2DFactory == nullptr) {
    D2D1_FACTORY_OPTIONS d2dFactoryOptions;
    ZeroMemory(&d2dFactoryOptions, sizeof(D2D1_FACTORY_OPTIONS));
#if defined(_DEBUG)
    // If the project is in a debug build, enable Direct2D debugging via SDK Layers.
    d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

    HRESULT hr;
    RETURNONFAIL(hr, D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory),
      &d2dFactoryOptions, reinterpret_cast<LPVOID*>(&sD2DFactory)));
  }
  *factory = sD2DFactory;
  return S_OK;
}


/// <summary>
/// Fetches a WIC factory.
/// </summary>
HRESULT Factories::GetWICFactory(LPVOID *factory) {
  if (sWICFactory == nullptr) {
    HRESULT hr;
    RETURNONFAIL(hr, CoCreateInstance(CLSID_WICImagingFactory1, nullptr, CLSCTX_INPROC_SERVER,
      IID_IWICImagingFactory, reinterpret_cast<LPVOID*>(&sWICFactory)));
  }
  *factory = sWICFactory;
  return S_OK;
}
