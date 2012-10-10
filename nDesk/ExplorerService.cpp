#include <Windows.h>
#include "ExplorerService.h"
#include "ShellDesktopTray.h"


DWORD WINAPI ExplorerThread(LPVOID);

ExplorerService::ExplorerService() {
    this->m_dwThread = 0;
}

ExplorerService::~ExplorerService() {

}


void ExplorerService::Stop() {

}


void ExplorerService::Start() {
    CreateThread(NULL, 0, ExplorerThread, NULL, 0, &this->m_dwThread);
}


DWORD WINAPI ExplorerThread(LPVOID) {
    typedef void *(WINAPI *SHCREATEDESKTOP)(void *);
    typedef bool (WINAPI *SHDESKTOPMESSAGELOOP)(void *);
    TShellDesktopTray *explorerTray = NULL;
    LPVOID lpVoid;
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    HMODULE library = LoadLibrary("shell32.dll");
    SHCREATEDESKTOP SHCreateDesktop = (SHCREATEDESKTOP)GetProcAddress(library, (LPCSTR)0xC8);
    SHDESKTOPMESSAGELOOP SHDesktopMessageLoop = (SHDESKTOPMESSAGELOOP)GetProcAddress(library, (LPCSTR)0xC9);


    explorerTray = new TShellDesktopTray();
    explorerTray->QueryInterface(IID_IShellDesktopTray, &lpVoid);
    IShellDesktopTray *iTray = reinterpret_cast <IShellDesktopTray*> (lpVoid);


    HANDLE hDesktop = SHCreateDesktop(iTray);
    ShowWindow(FindWindowW(L"Progman", NULL), SW_HIDE);
    if (hDesktop) {
        SHDesktopMessageLoop(hDesktop);
    }

    iTray->Release();
    explorerTray->Release();
    CoUninitialize();

    FreeLibrary(library);
    return 0;
}
