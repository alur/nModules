/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ButtonSettings.cpp
 *  The nModules Project
 *
 *  Contains all the settings used by button for a particular taskbar.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "ButtonSettings.hpp"


/// <summary>
/// Loads these button settings from the settings file.
/// </summary>
void ButtonSettings::Load(Settings *buttonSettings)
{
    //
    mUseFlashing = buttonSettings->GetBool(_T("UseFlashing"), true);
    mFlashInterval = buttonSettings->GetInt(_T("FlashInterval"), 500);
    mNoIcons = buttonSettings->GetBool(_T("NoIcons"), false);

    //
    Settings *iconSettings = buttonSettings->CreateChild(_T("Icon"));
    float iconSize = iconSettings->GetFloat(_T("Size"), 32);
    float iconX = iconSettings->GetFloat(_T("X"), 2);
    float iconY = iconSettings->GetFloat(_T("Y"), 2);
    mIconRect = D2D1::RectF(iconX, iconY, iconX + iconSize, iconY + iconSize);

    //
    float overlayIconSize = iconSettings->GetFloat(_T("OverlaySize"), 16);
    float overlayIconOffsetX = iconSettings->GetFloat(_T("OverlayOffsetX"), 1);
    float overlayIconOffsetY = iconSettings->GetFloat(_T("OverlayOffsetY"), 1);
    mOverlayIconRect = D2D1::RectF(
        iconX + iconSize - overlayIconSize + overlayIconOffsetX,
        iconY + iconSize - overlayIconSize + overlayIconOffsetY,
        iconX + iconSize + overlayIconOffsetX,
        iconY + iconSize + overlayIconOffsetY);

    //
    StateRender<TaskButton::State>::InitData initData;

    // Common defaults
    for (auto &x : initData)
    {
        //
        x.defaults.brushSettings[State::BrushType::Background].color = Color::Create(0x00000000);
        x.defaults.textOffsetLeft = 36;
        x.defaults.textVerticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
    }

    //
    initData[TaskButton::State::Active].prefix = _T("Active");
    initData[TaskButton::State::Hover].prefix = _T("Hover");
    initData[TaskButton::State::Flashing].prefix = _T("Flashing");
    initData[TaskButton::State::Minimized].prefix = _T("Minimized");

    initData[TaskButton::State::ActiveHover].prefix = _T("ActiveHover");
    initData[TaskButton::State::ActiveHover].base = TaskButton::State::Active;
    initData[TaskButton::State::ActiveHover].dependencies = { TaskButton::State::Active, TaskButton::State::Hover };
    initData[TaskButton::State::FlashingHover].prefix = _T("FlashingHover");
    initData[TaskButton::State::FlashingHover].base = TaskButton::State::Flashing;
    initData[TaskButton::State::FlashingHover].dependencies = { TaskButton::State::Flashing, TaskButton::State::Hover };
    initData[TaskButton::State::MinimizedFlashing].prefix = _T("MinimizedFlashing");
    initData[TaskButton::State::MinimizedFlashing].base = TaskButton::State::Flashing;
    initData[TaskButton::State::MinimizedFlashing].dependencies = { TaskButton::State::Flashing, TaskButton::State::Minimized };
    initData[TaskButton::State::MinimizedFlashingHover].prefix = _T("MinimizedFlashingHover");
    initData[TaskButton::State::MinimizedFlashingHover].base = TaskButton::State::FlashingHover;
    initData[TaskButton::State::MinimizedFlashingHover].dependencies = { TaskButton::State::MinimizedFlashing, TaskButton::State::MinimizedHover };
    initData[TaskButton::State::MinimizedHover].prefix = _T("MinimizedHover");
    initData[TaskButton::State::MinimizedHover].base = TaskButton::State::Hover;
    initData[TaskButton::State::MinimizedHover].dependencies = { TaskButton::State::Minimized, TaskButton::State::Hover };

    mStateRender.Load(initData, buttonSettings);

    //
    mWindowSettings.Load(buttonSettings, nullptr);

    delete iconSettings;
}
