/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IconGroup.cpp                                                 August, 2012
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "IconGroup.hpp"
#include "../nShared/Macros.h"
#include "../nShared/Debugging.h"
// 
extern HINSTANCE g_hInstance;
extern LPCSTR g_szGroupHandler;


#define WM_SHCHANGE_NOTIFY WM_USER

/// <summary>
/// Constructor
/// </summary>
IconGroup::IconGroup() {
    WCHAR path[MAX_PATH];

    // Initalize all variables.
    this->changeNotifyUID = 0;

    this->paintSettings = new PaintSettings("DesktopIcons");
    this->paintSettings->position.bottom = 1100;
    this->paintSettings->position.top = 50;
    this->paintSettings->position.left = 1970;
    this->paintSettings->position.right = 3790;
    this->paintSettings->setText(L"");
    this->paintSettings->backColor = this->paintSettings->ARGBToD2DColor(0x6600FF00);

    this->paintSettings->GetSettings()->GetString("Folder", path, sizeof(path), "C:\\test");

    this->window = new DrawableWindow(FindWindow("DesktopBackgroundClass", ""), g_szGroupHandler, this->paintSettings, g_hInstance);
    SetWindowLongPtr(this->window->getWindow(), 0, (LONG_PTR)this);
    this->window->Show();

    // Get the root ISHellFolder
    SHGetDesktopFolder(&this->rootFolder);

    SetFolder(path);

    //LPWSTR wszLoc = NULL;
    //SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &wszLoc);
    //m_pDirectoryManager = new DirectoryManager(wszLoc, m_pWindow->getWindow(), a);
    //CoTaskMemFree(wszLoc);
}


/// <summary>
/// Destructor
/// </summary>
IconGroup::~IconGroup() {
    if (this->changeNotifyUID != 0) {
        SHChangeNotifyDeregister(this->changeNotifyUID);
    }
    SAFERELEASE(this->rootFolder);
    SAFEDELETE(this->window);
    SAFEDELETE(this->paintSettings);
}


/// <summary>
/// Destructor
/// </summary>
void IconGroup::SetFolder(LPWSTR folder) {
    IShellFolder* workingFolder;

    PIDLIST_RELATIVE idList;
    PIDLIST_RELATIVE idNext;
    IEnumIDList* enumIDList;

    STRRET ret;
    
    // Just in case we are switching folders, deregister for old notifications
    if (this->changeNotifyUID != 0) {
        SHChangeNotifyDeregister(this->changeNotifyUID);
    }

    // Get the folder we are interested in
    this->rootFolder->ParseDisplayName(NULL, NULL, folder, NULL, &idList, NULL);
    this->rootFolder->BindToObject(idList, NULL, IID_IShellFolder, reinterpret_cast<LPVOID*>(&workingFolder));

    // Enumerate the contents of this folder
    workingFolder->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &enumIDList);
    while (enumIDList->Next(1, &idNext, NULL) != S_FALSE) {
        workingFolder->GetDisplayNameOf(idNext, SHGDN_NORMAL, &ret);
    }
    enumIDList->Release();

    // Register for change notifications
    SHChangeNotifyEntry watchEntries[] = { idList, TRUE };
    this->changeNotifyUID = SHChangeNotifyRegister(
        this->window->getWindow(),
        SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery,
        SHCNE_CREATE | SHCNE_DELETE,
        WM_SHCHANGE_NOTIFY,
        1,
        watchEntries);

    // Let go fo the PIDLists
    CoTaskMemFree(idList);
    CoTaskMemFree(idNext);

	// Let go of the IShellFolder interfaces
    workingFolder->Release();
}


/// <summary>
/// 
/// </summary>
LRESULT WINAPI IconGroup::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_SHCHANGE_NOTIFY:
        {
            long event;
            PIDLIST_ABSOLUTE* idList;
            STRRET ret;
            HANDLE notifyLock = SHChangeNotification_Lock((HANDLE)wParam, (DWORD)lParam, &idList, &event);

            if (notifyLock) {
                this->rootFolder->GetDisplayNameOf(*idList, SHGDN_NORMAL, &ret);


                switch (event) {
                case SHCNE_CREATE:
                    TRACEW(L"File created: %s", ret.pOleStr);
                    break;
                case SHCNE_DELETE:
                    TRACEW(L"File deleted: %s", ret.pOleStr);
                    break;
                }
                SHChangeNotification_Unlock(notifyLock);
            }
        }
        return 0;

    default:
        return this->window->HandleMessage(msg, wParam, lParam);
    }
}
