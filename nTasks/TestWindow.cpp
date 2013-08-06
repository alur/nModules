#include "../Utilities/Common.h"
#include "TestWindow.hpp"
#include "../nShared/LSModule.hpp"
#include <thread>


void TestWindow::CreateTestWindow() {
    std::thread worker(TestWindow::WindowWorker);
    worker.detach();
}


void TestWindow::WindowWorker() {
    TestWindow *window = new TestWindow();

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete window;
}


TestWindow::TestWindow()
{
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.lpszClassName = _T("nTaskTestClass");
    wc.style = CS_DBLCLKS;
    wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = DefWindowProc;
    MessageHandler::FixWindowClass(&wc);
    mWindowClass = RegisterClassEx(&wc);

    mWindow = CreateWindowEx(0, (LPCTSTR)mWindowClass, _T("nTaskTestWindow"), WS_OVERLAPPEDWINDOW, 100, 100, 500, 300, nullptr, nullptr, wc.hInstance, this);

    //DialogBox(wc.hInstance, (), mWindow, );











    //ShowWindow(mWindow, SW_SHOW);
}


TestWindow::~TestWindow() {
    DestroyWindow(mWindow);
    UnregisterClass((LPCTSTR)mWindowClass, GetModuleHandle(NULL));
}


LRESULT WINAPI TestWindow::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID extra) {
    switch (msg) {
    case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        break;

    case WM_PAINT:
        {

        }
        break;
    }

    return DefWindowProc(window, msg, wParam, lParam);
}
