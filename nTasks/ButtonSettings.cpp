/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ButtonSettings.cpp
 *  The nModules Project
 *
 *  Contains all the settings used by button for a particular taskbar.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "ButtonSettings.hpp"
#include "TaskButton.hpp"

static const StateRender<TaskButton::State>::InitData sInitData(
  [] (StateRender<TaskButton::State>::InitData &initData)
{
  // Common defaults
  for (auto &x : initData) {
    x.defaults.brushSettings[State::BrushType::Background].color = Color::Create(0x00000000);
    x.defaults.textOffsetLeft = 36;
    x.defaults.textVerticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
  }

  using State = TaskButton::State;
  initData[State::Active].prefix = L"Active";
  initData[State::Hover].prefix = L"Hover";
  initData[State::Flashing].prefix = L"Flashing";
  initData[State::Minimized].prefix = L"Minimized";

  initData[State::ActiveHover].prefix = L"ActiveHover";
  initData[State::ActiveHover].base = State::Active;
  initData[State::ActiveHover].dependencies = { State::Active, State::Hover };
  initData[State::FlashingHover].prefix = L"FlashingHover";
  initData[State::FlashingHover].base = State::Flashing;
  initData[State::FlashingHover].dependencies = { State::Flashing, State::Hover };
  initData[State::MinimizedFlashing].prefix = L"MinimizedFlashing";
  initData[State::MinimizedFlashing].base = State::Flashing;
  initData[State::MinimizedFlashing].dependencies = { State::Flashing, State::Minimized };
  initData[State::MinimizedFlashingHover].prefix = L"MinimizedFlashingHover";
  initData[State::MinimizedFlashingHover].base = State::FlashingHover;
  initData[State::MinimizedFlashingHover].dependencies = { State::MinimizedFlashing,
    State::MinimizedHover };
  initData[State::MinimizedHover].prefix = L"MinimizedHover";
  initData[State::MinimizedHover].base = State::Hover;
  initData[State::MinimizedHover].dependencies = { State::Minimized, State::Hover };
});


/// <summary>
/// Loads these button settings from the settings file.
/// </summary>
void ButtonSettings::Load(Settings *buttonSettings) {
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

    mStateRender.Load(sInitData, buttonSettings);

    //
    mWindowSettings.Load(buttonSettings, nullptr);

    delete iconSettings;
}
