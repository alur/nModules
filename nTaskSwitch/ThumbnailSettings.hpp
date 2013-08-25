/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ThumbnailSettings.hpp
 *  The nModules Project
 *
 *  Contains all the settings used by button for a particular thumbnail.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

class ThumbnailSettings;

#include "TaskThumbnail.hpp"

class ThumbnailSettings
{
public:
    RECT mThumbnailMargins;
    
    // Window settings
public:
    WindowSettings mWindowSettings;
    StateRender<TaskThumbnail::State> mStateRender;

    WindowSettings mIconWindowSettings;

public:
    void Load(Settings *settings);
};
