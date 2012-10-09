#include <Windows.h>
#include "ShellDesktopTray.hpp"

namespace ExplorerTest {
    void Init();
    DWORD WINAPI Thread(LPVOID param);
}

void ExplorerTest::Init() {
    DWORD dw;
    CreateThread(NULL, 0, Thread, NULL, 0, &dw);
}

DWORD WINAPI ExplorerTest::Thread(LPVOID param) {
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
