/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Icon.hpp
 *  The nModules Project
 *
 *  A single icon in an IconGroup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Drawable.hpp"
#include "../nShared/DrawableWindow.hpp"
#include <ShlObj.h>

class Icon : public Drawable {
public:
    explicit Icon(Drawable* parent, PCITEMID_CHILD item, IShellFolder2* shellFolder);
    virtual ~Icon();

    void SetPosition(int x, int y);
    LRESULT WINAPI HandleMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
    HRESULT GetDisplayName(SHGDNF flags, LPWSTR buf, UINT cchBuf);

private:
    IShellFolder2* shellFolder;
    PITEMID_CHILD item;

    void SetIcon();
};