/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Factories.h
 *  The nModules Project
 *
 *  Manages Direct2D/DirectWrite/WIC factories.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

namespace Factories {
    HRESULT GetDWriteFactory(LPVOID* ppFactory);
    HRESULT GetD2DFactory(LPVOID* ppFactory);
    HRESULT GetWICFactory(LPVOID* ppFactory);
    void Release();
}
