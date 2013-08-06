/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Factories.h
 *  The nModules Project
 *
 *  Manages Direct2D/DirectWrite/WIC factories.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

namespace Factories
{
    HRESULT GetDWriteFactory(LPVOID *factory);
    HRESULT GetD2DFactory(LPVOID *factory);
    HRESULT GetWICFactory(LPVOID *factory);
    void Release();
}
