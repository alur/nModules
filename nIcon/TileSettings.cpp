/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TileSettings.cpp
 *  The nModules Project
 *
 *  Contains all the settings used by an icon tile.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "TileSettings.hpp"


/// <summary>
/// Loads these settings from the settings file.
/// </summary>
void TileSettings::Load(Settings *settings)
{
    mTileWindowSettings.Load(settings, nullptr);

    mIconSize = settings->GetInt(L"Size", 48);
    mGhostOpacity = settings->GetFloat(L"GhostOpacity", 0.6f);
    
    StateRender<IconTile::State>::InitData tileInitData;
    for (auto &stateInitData : tileInitData)
    {
        stateInitData.defaults.brushSettings[::State::BrushType::Background].color = Color::Create(0x00000000);
        stateInitData.defaults.brushSettings[::State::BrushType::TextStroke].color = Color::Create(0xBB454545);
        stateInitData.defaults.wordWrapping = DWRITE_WORD_WRAPPING_WRAP;
        stateInitData.defaults.textAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
        stateInitData.defaults.textOffsetTop = (float)mIconSize;
        stateInitData.defaults.fontStrokeWidth = 2.0f;
    }

    tileInitData[IconTile::State::Hover].prefix = L"Hover";
    tileInitData[IconTile::State::Hover].defaults.brushSettings[::State::BrushType::Background].color = Color::Create(0xAA87CEEB);
    tileInitData[IconTile::State::Hover].defaults.brushSettings[::State::BrushType::Outline].color = Color::Create(0x99FFFFFF);
    tileInitData[IconTile::State::Hover].defaults.outlineWidth = 1.5f;
    
    tileInitData[IconTile::State::Selected].prefix = L"Selected";
    tileInitData[IconTile::State::Selected].defaults.brushSettings[::State::BrushType::Background].color = Color::Create(0xCC87CEEB);
    tileInitData[IconTile::State::Selected].defaults.brushSettings[::State::BrushType::Outline].color = Color::Create(0xCCFFFFFF);
    tileInitData[IconTile::State::Selected].defaults.outlineWidth = 1.5f;
    
    tileInitData[IconTile::State::Focused].prefix = L"Focused";
    tileInitData[IconTile::State::Focused].defaults.brushSettings[::State::BrushType::Background].color = Color::Create(0xAA87CEEB);
    tileInitData[IconTile::State::Focused].defaults.brushSettings[::State::BrushType::Outline].color = Color::Create(0x99FFFFFF);
    tileInitData[IconTile::State::Focused].defaults.outlineWidth = 1.5f;

    mTileStateRender.Load(tileInitData, settings);
}
