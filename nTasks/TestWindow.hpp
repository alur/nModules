//--------------------------------------------------------------------------------------
// TestWindow.hpp
// The nModules Project
//
// A window used for testing taskbar features.
//
//--------------------------------------------------------------------------------------
#pragma once

#include <strsafe.h>
#include <vector>

class TestWindow
{
    // Static functions
public:
    static void Create();
    static void DestroyAll();

    // Constructor & Destructor
public:
    TestWindow(ITaskbarList4 *taskbarList, HMODULE module);
    ~TestWindow();

    // Variables
private:
    ITaskbarList4 *mTaskbarList;
    HMODULE mModule;

    // Window Procs
public:
    INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK ExternDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

    // General
private:
    void Initialize(HWND dialogWindow);

    HWND mDialogWindow;

    // Overlay icon
private:
    void InitOverlay();

    HWND mOverlayDropdown;
    HWND mOverlayDescription;
    HIMAGELIST mOverlayImages;

    struct OverlayItem
    {
        OverlayItem(LPCTSTR name, HICON icon)
        {
            StringCchCopy(this->name, _countof(this->name), name);
            this->icon = icon;
        }

        TCHAR name[MAX_PATH];
        HICON icon;
    };

    std::vector<OverlayItem> mOverlayItems;

    // Progress bar
private:
    void InitProgress();
    void SetProgressState(TBPFLAG);
    void SetProgressValue(ULONGLONG value);
    ULONGLONG GetProgressValue();

    TBPFLAG mProgressState;
    HWND mProgressSlider;
};
