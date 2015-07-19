#include "Factories.h"

#include "../Headers/d2d1.h"
#include "../Headers/Macros.h"

#include <dwrite.h>
#include <wincodec.h>

ID2D1Factory *gD2DFactory = nullptr;
IDWriteFactory *gDWriteFactory = nullptr;
IWICImagingFactory *gWICFactory = nullptr;


HRESULT Factories::Create() {
  HRESULT hr = S_OK;

  hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
    reinterpret_cast<IUnknown**>(&gDWriteFactory));

  if (SUCCEEDED(hr)) {
    D2D1_FACTORY_OPTIONS d2dFactoryOptions;
    ZeroMemory(&d2dFactoryOptions, sizeof(D2D1_FACTORY_OPTIONS));
#if defined(_DEBUG)
    d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory),
      &d2dFactoryOptions, reinterpret_cast<void**>(&gD2DFactory));
  }

  if (SUCCEEDED(hr)) {
    hr = CoCreateInstance(CLSID_WICImagingFactory1, nullptr, CLSCTX_INPROC_SERVER,
      IID_IWICImagingFactory, reinterpret_cast<LPVOID*>(&gWICFactory));
  }

  if (FAILED(hr)) {
    Destroy();
  }

  return hr;
}


void Factories::Destroy() {
  SAFERELEASE(gD2DFactory);
  SAFERELEASE(gDWriteFactory);
  SAFERELEASE(gWICFactory);
}


EXPORT_CDECL(ID2D1Factory*) GetD2DFactory() {
  return gD2DFactory;
}


EXPORT_CDECL(IDWriteFactory*) GetDWriteFactory() {
  return gDWriteFactory;
}


EXPORT_CDECL(IWICImagingFactory*) GetWICFactory() {
  return gWICFactory;
}
