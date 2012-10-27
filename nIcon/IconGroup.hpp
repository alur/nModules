/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IconGroup.hpp
 *  The nModules Project
 *
 *  A group of icons. Essentially an *nIcon.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Drawable.hpp"
#include "../nShared/DrawableWindow.hpp"
#include "../nShared/Settings.hpp"
#include "IIconEventHandler.hpp"
#include "Icon.hpp"
#include <ShlObj.h>
#include <map>


using std::map;


class IconGroup : public IIconEventHandler, public Drawable {
public:
    explicit IconGroup(LPCSTR prefix);
    virtual ~IconGroup();

    LRESULT WINAPI HandleMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID);

    void SetFolder(LPWSTR path);
    
private:
    //
    HRESULT GetDisplayNameOf(PCITEMID_CHILD pidl, SHGDNF flags, LPWSTR buf, UINT cchBuf);

    // 
    void AddIcon(PCITEMID_CHILD pidl, bool noRedraw = false);

    // 
    void RemoveIcon(PCITEMID_CHILD pidl);

    //
    void UpdateIcon(PCITEMID_CHILD pidl);

    void UpdateAllIcons();

    //
    void RenameIcon(PCITEMID_CHILD oldID, PCITEMID_CHILD newID);

    // 
    void PositionIcon(PCITEMID_CHILD pidl, D2D1_RECT_F* position);

    //
    vector<Icon*>::iterator FindIcon(PCITEMID_CHILD pidl);

    // All icons currently part of this group.
    vector<Icon*> icons;

    // Return value of the latest SHChangeNofityRegister call.
    ULONG changeNotifyUID;

    UINT changeNotifyMsg;

    // IShellFolder for the folder we are currently in.
    IShellFolder2* workingFolder;

    // IShellFolder for the root of the file system.
    IShellFolder2* rootFolder;
};
