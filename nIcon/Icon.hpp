/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Icon.hpp
 *  The nModules Project
 *
 *  A single icon in an IconGroup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/DrawableWindow.hpp"
#include <ShlObj.h>

class Icon {
public:
    explicit Icon(PCITEMID_CHILD item, IShellFolder2* shellFolder, DrawableWindow* parentWindow, Settings* parentSettings);
    virtual ~Icon();

    void SetPosition(int x, int y);
    LRESULT WINAPI HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    HRESULT GetDisplayName(SHGDNF flags, LPWSTR buf, UINT cchBuf);

private:
    IShellFolder2* shellFolder;
    PITEMID_CHILD item;

    void SetIcon();

    Settings* settings;
    DrawableWindow* window;
};