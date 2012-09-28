/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Icon.hpp                                                   September, 2012
 *  The nModules Project
 *
 *  A single icon in an IconGroup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef ICON_HPP
#define ICON_HPP

#include "../nShared/DrawableWindow.hpp"
#include "../nShared/PaintSettings.hpp"
#include <ShlObj.h>

class Icon {
public:
    explicit Icon(PITEMID_CHILD item, IShellFolder2* shellFolder);
    virtual ~Icon();

    LRESULT WINAPI HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    
private:
    IShellFolder2* shellFolder;
    PITEMID_CHILD item;

    PaintSettings* paintSettings;
    DrawableWindow* window;
};

#endif /* ICON_HPP */
