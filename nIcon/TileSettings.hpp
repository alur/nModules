/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TileSettings.hpp
 *  The nModules Project
 *
 *  Contains all the settings used by an icon tile.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

class TileSettings;

#include "../nShared/Settings.hpp"
#include "../nShared/StateRender.hpp"
#include "../nShared/WindowSettings.hpp"

#include "IconTile.hpp"

class TileSettings
{
public:
    WindowSettings mTileWindowSettings;
    StateRender<IconTile::State> mTileStateRender;

    int mIconSize;
    float mGhostOpacity;

public:
    void Load(Settings *settings);
};
