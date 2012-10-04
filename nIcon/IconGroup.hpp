/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IconGroup.hpp
 *  The nModules Project
 *
 *  A group of icons. Essentially an *nIcon.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/DrawableWindow.hpp"
#include "../nShared/Settings.hpp"
#include "IIconEventHandler.hpp"
#include "Icon.hpp"
#include <ShlObj.h>
#include <map>

using std::map;

class IconGroup : public IIconEventHandler, IDrawableMessageHandler {
public:
    explicit IconGroup(LPCSTR pszPrefix);
    virtual ~IconGroup();

    LRESULT WINAPI HandleMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void SetFolder(LPWSTR path);
    
private:
    //
    HRESULT GetDisplayNameOf(PCITEMID_CHILD pidl, SHGDNF flags, LPWSTR buf, UINT cchBuf);

    // 
    void AddIcon(PCITEMID_CHILD pidl);

    // 
    void RemoveIcon(PCITEMID_CHILD pidl);

    // 
    void PositionIcon(PCITEMID_CHILD pidl, D2D1_RECT_F* position);

    //
    PCITEMID_CHILD GetLastPIDLItem(LPITEMIDLIST pidl);

    //
    vector<Icon*>::iterator FindIcon(PCITEMID_CHILD pidl);

    // Paintsettings for the whole group.
    Settings* settings;

    // The window which holds the whole group.
    DrawableWindow* window;

    // All icons currently part of this group.
    vector<Icon*> icons;

    // Return value of the latest SHChangeNofityRegister call.
    ULONG changeNotifyUID;

    // IShellFolder for the folder we are currently in.
    IShellFolder2* workingFolder;

    // IShellFolder for the root of the file system.
    IShellFolder2* rootFolder;
};
