//-------------------------------------------------------------------------------------------------
// /nIcon/TileSettings.cpp
// The nModules Project
//
// Contains all the settings used by an icon tile.
//-------------------------------------------------------------------------------------------------
#include "Tile.hpp"
#include "TileSettings.hpp"

static const StateRender<Tile::State>::InitData sTileInitData([] (StateRender<Tile::State>::InitData &tileInitData) {
  for (auto &stateInitData : tileInitData) {
    stateInitData.defaults.brushSettings[::State::BrushType::Background].color = Color::Create(0x00000000);
    stateInitData.defaults.brushSettings[::State::BrushType::TextStroke].color = Color::Create(0xBB454545);
    stateInitData.defaults.wordWrapping = DWRITE_WORD_WRAPPING_WRAP;
    stateInitData.defaults.textAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
    stateInitData.defaults.textOffsetTop = 64;
    stateInitData.defaults.fontStrokeWidth = 2.0f;
  }

  tileInitData[Tile::State::Hover].prefix = L"Hover";
  tileInitData[Tile::State::Hover].defaults.brushSettings[::State::BrushType::Background].color = Color::Create(0xAA87CEEB);
  tileInitData[Tile::State::Hover].defaults.brushSettings[::State::BrushType::Outline].color = Color::Create(0x99FFFFFF);
  tileInitData[Tile::State::Hover].defaults.outlineWidth = 1.5f;

  tileInitData[Tile::State::Selected].prefix = L"Selected";
  tileInitData[Tile::State::Selected].defaults.brushSettings[::State::BrushType::Background].color = Color::Create(0xCC87CEEB);
  tileInitData[Tile::State::Selected].defaults.brushSettings[::State::BrushType::Outline].color = Color::Create(0xCCFFFFFF);
  tileInitData[Tile::State::Selected].defaults.outlineWidth = 1.5f;

  tileInitData[Tile::State::Focused].prefix = L"Focused";
  tileInitData[Tile::State::Focused].defaults.brushSettings[::State::BrushType::Background].color = Color::Create(0xAA87CEEB);
  tileInitData[Tile::State::Focused].defaults.brushSettings[::State::BrushType::Outline].color = Color::Create(0x99FFFFFF);
  tileInitData[Tile::State::Focused].defaults.outlineWidth = 1.5f;
});


/// <summary>
/// Loads these settings from the settings file.
/// </summary>
void TileSettings::Load(Settings *settings) {
  mTileWindowSettings.Load(settings, nullptr);
  mTileStateRender.Load(sTileInitData, settings);
  mIconSize = settings->GetInt(L"Size", 48);
  mGhostOpacity = settings->GetFloat(L"GhostOpacity", 0.6f);
}
