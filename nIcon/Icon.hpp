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

class Icon {
public:
    explicit Icon();
    virtual ~Icon();

    LRESULT WINAPI HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    
private:
    PaintSettings* paintSettings;
    DrawableWindow* window;
};

#endif /* ICON_HPP */
