/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ButtonSettings.hpp
 *  The nModules Project
 *
 *  Contains all the settings used by button for a particular taskbar.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "TaskButton.hpp"
#include "../Utilities/EnumArray.hpp"

class ButtonSettings
{
public:
    explicit ButtonSettings();
    virtual ~ButtonSettings();

    // Button Specific settings
public:
    bool mUseFlashing;
    int mFlashInterval;
    bool mNoIcons;

    float mIconSize;
    float mIconX;
    float mIconY;

    float mOverlayIconSize;
    float mOverlayIconOffsetX;
    float mOverlayIconOffsetY;

    // Window settings
public:
    WindowSettings mWindowSettings;
    WindowSettings mWindowDefaults;
    
    // State settings
public:
    //EnumArray<TaskButton::State, StateSettings> mStateSettings;
    //EnumArray<TaskButton::State, StateSettings> mStateDefaults;

    //
public:
    void Load(Settings *settings);
};
