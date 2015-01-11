/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TestWindow.cpp
 *  The nModules Project
 *
 *  A window for testing taskbar features.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../Utilities/Common.h"
#include <ShlObj.h>
#include "TestWindow.hpp"
#include "../nShared/LSModule.hpp"
#include "../Utilities/Error.h"
#include <mutex>
#include <map>
#include <set>
#include <thread>
#include "resource.h"

extern LSModule gLSModule;

// List of all currently living test dialogs.
static std::set<HWND> sTestDialogs;
static std::mutex sTestDialogsMutex;

#define PROGRESS_MAX (1 << 13)

/// <summary>
/// Constructor
/// </summary>
TestWindow::TestWindow(ITaskbarList4 *taskbarList, HMODULE module)
    : mTaskbarList(taskbarList)
    , mModule(module)
    , mProgressState(TBPF_NOPROGRESS)
{
    mTaskbarList->AddRef();
}


/// <summary>
/// Destructor
/// </summary>
TestWindow::~TestWindow()
{
    ImageList_Destroy(mOverlayImages);
    mTaskbarList->Release();
}


/// <summary>
/// Initializes the test window
/// </summary>
void TestWindow::Initialize(HWND dialogWindow)
{
    mDialogWindow = dialogWindow;

    //
    SetWindowText(mDialogWindow, L"nTaskTestWindow");
    SetDlgItemText(mDialogWindow, IDC_WINDOWTITLE, L"nTaskTestWindow");

    //
    InitOverlay();
    InitProgress();
}


/// <summary>
/// Initializes the overlay part of the window.
/// </summary>
void TestWindow::InitOverlay()
{
    mOverlayDropdown = GetDlgItem(mDialogWindow, IDC_OVERLAY_COMBO);
    mOverlayDescription = GetDlgItem(mDialogWindow, IDC_OVERLAY_DESC);

    //
    mOverlayItems.push_back(OverlayItem(L"None", nullptr));
    mOverlayItems.push_back(OverlayItem(L"Alert", LoadIcon(NULL, IDI_ASTERISK)));
    mOverlayItems.push_back(OverlayItem(L"Question", LoadIcon(NULL, IDI_QUESTION)));
    mOverlayItems.push_back(OverlayItem(L"Error", LoadIcon(NULL, IDI_ERROR)));
    mOverlayItems.push_back(OverlayItem(L"Warning", LoadIcon(NULL, IDI_WARNING)));
    mOverlayItems.push_back(OverlayItem(L"Shield", LoadIcon(NULL, IDI_SHIELD)));

    //
    mOverlayImages = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, (int)mOverlayItems.size(), 10);

    // Add items
    COMBOBOXEXITEM item;
    ZeroMemory(&item, sizeof(item));
    
    item.cchTextMax = 100;

    for (size_t i = 0, j = 0; i < mOverlayItems.size(); ++i)
    {
        if (mOverlayItems[i].icon != nullptr)
        {
            ImageList_AddIcon(mOverlayImages, mOverlayItems[i].icon);
            item.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
            item.iImage = (int)j;
            item.iSelectedImage = (int)j;
            ++j;
        }
        else
        {
            item.mask = CBEIF_TEXT;
        }
        item.iItem = i;
        item.pszText = mOverlayItems[i].name;
        SendMessage(mOverlayDropdown, CBEM_INSERTITEM, 0, LPARAM(&item));
    }

    //
    SendMessage(mOverlayDropdown, CBEM_SETIMAGELIST, 0, LPARAM(mOverlayImages));

    // Set active item
    SendMessage(mOverlayDropdown, CB_SETCURSEL, 0, 0);
}


/// <summary>
/// Initializes the progress part of the window.
/// </summary>
void TestWindow::InitProgress()
{
    mProgressSlider = GetDlgItem(mDialogWindow, IDC_PROGRESS_SLIDER);

    CheckRadioButton(mDialogWindow, IDC_PROGRESS_NONE, IDC_PROGRESS_PAUSED, IDC_PROGRESS_NONE);
    SendDlgItemMessage(mDialogWindow, IDC_PROGRESS_SLIDER, TBM_SETRANGE, TRUE, MAKELPARAM(0, PROGRESS_MAX));
}


/// <summary>
/// Sets the progress state
/// </summary>
void TestWindow::SetProgressState(TBPFLAG state)
{
    mProgressState = state;
    mTaskbarList->SetProgressState(mDialogWindow, state);
    if (state != TBPF_NOPROGRESS && state != TBPF_INDETERMINATE)
    {
        SetProgressValue(GetProgressValue());
    }
}


/// <summary>
/// Sets the progress value
/// </summary>
void TestWindow::SetProgressValue(ULONGLONG value)
{
    if (mProgressState != TBPF_NOPROGRESS && mProgressState != TBPF_INDETERMINATE)
    {
        mTaskbarList->SetProgressValue(mDialogWindow, value, PROGRESS_MAX);
    }
}


/// <summary>
/// Gets the progress value
/// </summary>
ULONGLONG TestWindow::GetProgressValue()
{
    return SendMessage(mProgressSlider, TBM_GETPOS, 0, 0);
}


/// <summary>
/// The test windows dialog process.
/// </summary>
INT_PTR CALLBACK TestWindow::DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            switch (wParam)
            {
            case MAKEWPARAM(IDOK, BN_CLICKED):
                {
                    DestroyWindow(hwndDlg);
                    PostQuitMessage(0);
                }
                return TRUE;

            case MAKEWPARAM(IDC_PROGRESS_NONE, BN_CLICKED):
                {
                    SetProgressState(TBPF_NOPROGRESS);
                }
                return TRUE;

            case MAKEWPARAM(IDC_PROGRESS_INDETERMINATE, BN_CLICKED):
                {
                    SetProgressState(TBPF_INDETERMINATE);
                }
                return TRUE;

            case MAKEWPARAM(IDC_PROGRESS_NORMAL, BN_CLICKED):
                {
                    SetProgressState(TBPF_NORMAL);
                }
                return TRUE;

            case MAKEWPARAM(IDC_PROGRESS_ERROR, BN_CLICKED):
                {
                    SetProgressState(TBPF_ERROR);
                }
                return TRUE;

            case MAKEWPARAM(IDC_PROGRESS_PAUSED, BN_CLICKED):
                {
                    SetProgressState(TBPF_PAUSED);
                }
                return TRUE;

            case MAKEWPARAM(IDC_FLASH, BN_CLICKED):
                {
                    SetForegroundWindow(FindWindow(L"DesktopBackgroundClass", nullptr));
                    FlashWindow(mDialogWindow, TRUE);
                }
                return TRUE;

            case MAKEWPARAM(IDC_WINDOWTITLE, EN_CHANGE):
                {
                    TCHAR windowTitle[256];
                    GetWindowText((HWND) lParam, windowTitle, _countof(windowTitle));
                    SetWindowText(mDialogWindow, windowTitle);
                }
                return TRUE;

            case MAKEWPARAM(IDC_OVERLAY_DESC, EN_CHANGE):
                {
                    TCHAR overlayTitle[256];
                    GetWindowText(mOverlayDescription, overlayTitle, _countof(overlayTitle));
                    mTaskbarList->SetOverlayIcon(mDialogWindow, mOverlayItems[SendMessage(mOverlayDropdown, CB_GETCURSEL, 0, 0)].icon, overlayTitle);
                }
                return TRUE;

            case MAKEWPARAM(IDC_OVERLAY_COMBO, CBN_SELCHANGE):
                {
                    TCHAR overlayTitle[256];
                    GetWindowText(mOverlayDescription, overlayTitle, _countof(overlayTitle));
                    mTaskbarList->SetOverlayIcon(mDialogWindow, mOverlayItems[SendMessage(mOverlayDropdown, CB_GETCURSEL, 0, 0)].icon, overlayTitle);
                }
                return TRUE;
            }
        }
        break;

    case WM_HSCROLL:
        {
            if ((HWND)lParam == GetDlgItem(hwndDlg, IDC_PROGRESS_SLIDER))
            {
                SetProgressValue(SendMessage((HWND)lParam, TBM_GETPOS, 0, 0));
            }
        }
        break;

    case WM_INITDIALOG:
        {
            Initialize(hwndDlg);
            sTestDialogsMutex.lock();
            sTestDialogs.insert(hwndDlg);
            sTestDialogsMutex.unlock();
        }
        return TRUE;

    case WM_DESTROY:
        {
            sTestDialogsMutex.lock();
            sTestDialogs.erase(hwndDlg);
            sTestDialogsMutex.unlock();
        }
        return TRUE;

    case WM_SYSCOMMAND:
        {
            switch (wParam & 0xFFF0)
            {
            case SC_CLOSE:
                {
                    DestroyWindow(hwndDlg);
                    PostQuitMessage(0);
                }
                return TRUE;
            }
        }
        break;
    }
    return FALSE;
}


/// <summary>
/// External test window dialog proc, forwarding to the internal class proc.
/// </summary>
INT_PTR CALLBACK TestWindow::ExternDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_INITDIALOG)
    {
        SetWindowLongPtr(hwndDlg, GWLP_USERDATA, lParam);
    }
    
    TestWindow *testWindow = (TestWindow *)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
    if (testWindow)
    {
        return testWindow->DialogProc(hwndDlg, message, wParam, lParam);
    }

    return FALSE;
}


/// <summary>
/// Handles HRESULT errors.
/// </summary>
static void ErrorHandler(LPCTSTR description, HRESULT error)
{
    TCHAR errorMessage[MAX_LINE_LENGTH];
    TCHAR hrDescription[MAX_LINE_LENGTH];

    DescriptionFromHR(error, hrDescription, _countof(hrDescription));
    StringCchPrintf(errorMessage, _countof(errorMessage), L"Error creating test window. %s\n\n%s", description, hrDescription);

    MessageBox(nullptr, errorMessage, L"nTask Error", MB_OK | MB_ICONERROR);
}


/// <summary>
/// The test windows process.
/// </summary>
static void WindowWorker(HINSTANCE instance)
{
    HRESULT hr;
    if (SUCCEEDED(hr = CoInitialize(nullptr)))
    {
        ITaskbarList4 *taskbarList;
        hr = CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER,
            IID_ITaskbarList4, reinterpret_cast<void**>(&taskbarList));
        if (SUCCEEDED(hr))
        {
            hr = taskbarList->HrInit();
            if (SUCCEEDED(hr))
            {
                TestWindow window(taskbarList, instance);

                HWND parent = CreateWindowEx(WS_EX_APPWINDOW, L"Static", L"", WS_OVERLAPPEDWINDOW, 0, 0, 50, 50, nullptr, nullptr, instance, 0);

                if (parent)
                {
                    if (DialogBoxParam(instance, MAKEINTRESOURCE(IDD_TEST_WINDOW), parent, TestWindow::ExternDialogProc, (LPARAM)&window) == -1)
                    {
                        ErrorHandler(L"DialogBoxParam failed.", HRESULT_FROM_WIN32(GetLastError()));
                    }

                    DestroyWindow(parent);
                }
                else
                {
                    ErrorHandler(L"CreateWindowEx failed.", HRESULT_FROM_WIN32(GetLastError()));
                }
            }
            else
            {
                ErrorHandler(L"ITaskbarList::HrInit failed.", hr);
            }

            taskbarList->Release();
        }
        else
        {
            ErrorHandler(L"CoCreateInstance(IID_ITaskbarList4) failed.", hr);
        }

        CoUninitialize();
    }
    else
    {
        ErrorHandler(L"Failed to get module handle.", hr);
    }
}


/// <summary>
/// Creates a test window.
/// </summary>
void TestWindow::Create()
{
    std::thread(WindowWorker, gLSModule.GetInstance()).detach();
}


/// <summary>
/// Creates a test window.
/// </summary>
void TestWindow::DestroyAll()
{
    std::vector<HANDLE> windowHandles;
    sTestDialogsMutex.lock();
    for (HWND hwnd : sTestDialogs)
    {
        windowHandles.push_back(OpenThread(SYNCHRONIZE, FALSE, GetWindowThreadProcessId(hwnd, nullptr)));
        PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), 0);
    }
    sTestDialogsMutex.unlock();
    if (!windowHandles.empty())
    {
        WaitForMultipleObjects((DWORD)windowHandles.size(), &windowHandles[0], TRUE, INFINITE);
    }
}


/// <summary>
/// Export version of CreateTestWindow, so that you can create the test window without loading nDesk.
/// (e.g. while using another task module, or while running explorer)
/// </summary>
EXPORT_STDCALL(void) CreateTestWindow(HWND, HINSTANCE, LPTSTR, int)
{
    HMODULE module;
    if (GetModuleHandleEx(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCTSTR)&WindowWorker,
        &module) != FALSE)
    {
        WindowWorker(module);
    }
    else
    {
        MessageBox(nullptr, L"Error creating test window. Failed to get module handle :/", L"nTask Error", MB_OK | MB_ICONERROR);
    }
}
