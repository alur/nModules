/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PopupSettings.hpp
 *  The nModules Project
 *
 *  Contains all the settings used by button for a particular Popup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

class PopupSettings;

#include "../nShared/Settings.hpp"
#include "../nShared/StateRender.hpp"
#include "../nShared/WindowSettings.hpp"

#include "CommandItem.hpp"
#include "FolderItem.hpp"
#include "InfoItem.hpp"
#include "SeparatorItem.hpp"

class PopupSettings
{
    // The main popup
public:

    // Command Item
public:
    WindowSettings mCommandWindowSettings;
    StateRender<CommandItem::State> mCommandStateRender;

    // Folder Item
public:
    WindowSettings mFolderWindowSettings;
    StateRender<nPopup::FolderItem::State> mFolderStateRender;

    // Info Item
public:
    WindowSettings mInfoWindowSettings;
    StateRender<InfoItem::State> mInfoStateRender;

    // Separator Item
public:
    WindowSettings mSeparatorWindowSettings;
    StateRender<SeparatorItem::State> mSeparatorStateRender;

public:
    void Load(Settings *settings);
};
