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
#include "Icon.hpp"
#include <ShlObj.h>

class IconGroup : public IIconEventHandler {
public:
    explicit IconGroup();
    virtual ~IconGroup();

    LRESULT WINAPI HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    void SetFolder(LPWSTR path);
    
private:
    // Paintsettings for the whole group.
    PaintSettings* paintSettings;

    // The window which holds the whole group.
    DrawableWindow* window;

    // All icons currently part of this group.
    vector<Icon*> icons;

    // Return value of the latest SHChangeNofityRegister call.
    ULONG changeNotifyUID;

    // IShellFolder for the desktop; root of the filesystem.
    IShellFolder* rootFolder;
};

#endif /* ICONGROUP_HPP */
