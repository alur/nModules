//--------------------------------------------------------------------------------------
// TestWindow.hpp
// The nModules Project
//
// A window used for testing taskbar features.
//
//--------------------------------------------------------------------------------------
#pragma once

#include "../nShared/MessageHandler.hpp"

class TestWindow : MessageHandler {
public:
    static void CreateTestWindow();

private:
    static void WindowWorker();

private:
    TestWindow();
    ~TestWindow();

private:
    ATOM mWindowClass;
    HWND mWindow;

    // MessageHandler implementation
public:
    LRESULT WINAPI HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID extra) override;

};
