#pragma once

const IID IID_IShellDesktopTray = {0x213E2DF9,0x9A14,0x4328,{0x99,0xB1,0x69,0x61,0xF9,0x14,0x3C,0xE9}};

class IShellDesktopTray : public IUnknown
{
public:
    STDMETHOD(QueryInterface)(THIS_ REFIID, LPVOID*) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    STDMETHOD_(ULONG,GetState)(THIS) PURE;
    STDMETHOD(GetTrayWindow)(THIS_ HWND*) PURE;
    STDMETHOD(RegisterDesktopWindow)(THIS_ HWND) PURE;
    STDMETHOD(SetVar)(THIS_ int,ULONG) PURE;
};

class TShellDesktopTray : public IShellDesktopTray
{
private:
    ULONG refCount;
    HWND desktopWnd;

public:
    TShellDesktopTray();
    ~TShellDesktopTray();

    STDMETHODIMP QueryInterface(REFIID riid, LPVOID * ppvObj);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    STDMETHODIMP_(ULONG) STDMETHODCALLTYPE GetState();
    STDMETHODIMP GetTrayWindow(HWND *o);
    STDMETHODIMP RegisterDesktopWindow(HWND d);
    STDMETHODIMP SetVar(int p1, ULONG p2);
};
