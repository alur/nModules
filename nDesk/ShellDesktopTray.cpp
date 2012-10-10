#include <Windows.h>
#include "ShellDesktopTray.h"


TShellDesktopTray::TShellDesktopTray()
{
    refCount = 0;
}


TShellDesktopTray::~TShellDesktopTray()
{

}


HRESULT TShellDesktopTray::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
    if (!ppvObj)
    {
        return E_POINTER;
    }

    if (riid == IID_IUnknown || riid == IID_IShellDesktopTray)
    {
        *ppvObj = this;
    }
    else
    {
        *ppvObj = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}


ULONG TShellDesktopTray::AddRef()
{
  ++refCount;

  return refCount;
}


ULONG TShellDesktopTray::Release()
{
    if (refCount > 0)
    {
        --refCount;
    }

    return refCount;
}


ULONG TShellDesktopTray::GetState()
{
    return 2;
}


HRESULT TShellDesktopTray::GetTrayWindow(HWND *hTrayWnd)
{
    // Prevent Explorer from closing the tray window when shutting down
    *hTrayWnd = NULL;
    return S_OK;
}


HRESULT TShellDesktopTray::RegisterDesktopWindow(HWND d)
{
    return S_OK;
}


HRESULT TShellDesktopTray::SetVar(int p1, ULONG p2)
{
    return S_OK;
}