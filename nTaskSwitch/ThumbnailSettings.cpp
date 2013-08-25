/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ThumbnailSettings.cpp
 *  The nModules Project
 *
 *  Contains all the settings used by button for a particular thumbnail.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "ThumbnailSettings.hpp"


/// <summary>
/// Loads these thumbnail settings from the settings file.
/// </summary>
void ThumbnailSettings::Load(Settings *settings)
{
    //
    Settings *thumbnailSettings = settings->CreateChild(L"Task");

    //
    mThumbnailMargins = thumbnailSettings->GetOffsetRect(L"Margin", 10, 10, 10, 10);

    //
    StateRender<TaskThumbnail::State>::InitData initData;
    initData[TaskThumbnail::State::Base].defaults.backgroundBrush.color = Color::Create(0x00000000);

    initData[TaskThumbnail::State::Hover].defaults.backgroundBrush.color = Color::Create(0xCC888888);
    initData[TaskThumbnail::State::Hover].prefix = L"Hover";
    
    initData[TaskThumbnail::State::Selected].defaults.backgroundBrush.color = Color::Create(0x00000000);
    initData[TaskThumbnail::State::Selected].defaults.outlineBrush.color = Color::Create(0xFFFFFFFF);
    initData[TaskThumbnail::State::Selected].defaults.outlineWidth = 2.5f;
    initData[TaskThumbnail::State::Selected].prefix = L"Selected";
    
    initData[TaskThumbnail::State::SelectedHover].defaults.backgroundBrush.color = Color::Create(0x00000000);
    initData[TaskThumbnail::State::SelectedHover].defaults.outlineBrush.color = Color::Create(0xFFFFFFFF);
    initData[TaskThumbnail::State::SelectedHover].defaults.outlineWidth = 2.5f;
    initData[TaskThumbnail::State::SelectedHover].prefix = L"SelectedHover";
    initData[TaskThumbnail::State::SelectedHover].dependencies = { TaskThumbnail::State::Hover, TaskThumbnail::State::Selected };
    mStateRender.Load(initData, thumbnailSettings);
    
    //
    WindowSettings iconDefaults;
    iconDefaults.alwaysOnTop = true;
    mIconWindowSettings.Load(thumbnailSettings, &iconDefaults);

    //
    delete thumbnailSettings;
}