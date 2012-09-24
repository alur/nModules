/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IconGroup.hpp                                                 August, 2012
 *  The nModules Project
 *
 *  A group of icons. Essentially an *nIcon.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef ICONGROUP_HPP
#define ICONGROUP_HPP

#include "../nShared/DrawableWindow.hpp"
#include "../nShared/PaintSettings.hpp"

#include "IIconEventHandler.hpp"
#include "DirectoryManager.hpp"
#include "Icon.hpp"

class IconGroup : public IIconEventHandler {
public:
    explicit IconGroup();
    virtual ~IconGroup();

    LRESULT WINAPI HandleMessage(UINT, WPARAM, LPARAM);
    
private:
    // Paintsettings for the whole group.
    PaintSettings* m_pPaintSettings;

    // The window which holds the whole group.
    DrawableWindow* m_pWindow;

    // The directory manager used by this group.
    DirectoryManager* m_pDirectoryManager;

    // All icons currently part of this group.
    vector<Icon*> m_icons;
};

#endif /* ICONGROUP_HPP */
