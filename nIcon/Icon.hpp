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
    explicit Icon(PITEMID_CHILD item, IShellFolder2* shellFolder);
    virtual ~Icon();

    LRESULT WINAPI HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    
private:
    IShellFolder2* shellFolder;
    PITEMID_CHILD item;

    Settings* settings;
    DrawableWindow* window;
};