//-------------------------------------------------------------------------------------------------
// /nIcon/TileSettings.hpp
// The nModules Project
//
// Contains all the settings used by an icon tile.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "Tile.hpp"

#include "../nShared/Settings.hpp"
#include "../nShared/StateRender.hpp"
#include "../nShared/WindowSettings.hpp"

class TileSettings {
public:
  WindowSettings mTileWindowSettings;
  StateRender<Tile::State> mTileStateRender;
  int mIconSize;
  float mGhostOpacity;

public:
  void Load(Settings *settings);
};
