/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Tooltip.cpp
 *  The nModules Project
 *
 *  A general tooltip window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "Tooltip.hpp"
#include <algorithm>


/// <summary>
/// Constructor
/// </summary>
Tooltip::Tooltip(LPCTSTR prefix, Settings* parentSettings) : Drawable(prefix, parentSettings)
{
    LoadSettings();

    WindowSettings windowDefaults;
    WindowSettings windowSettings;
    windowDefaults.width = 150;
    windowDefaults.height = 40;
    windowDefaults.alwaysOnTop = true;
    windowSettings.Load(mSettings, &windowDefaults);

    mWindow->Initialize(windowSettings, &mStateRender);
}


/// <summary>
/// Destructor
/// </summary>
Tooltip::~Tooltip()
{
}


/// <summary>
/// Shows the tooltip at the specified position.
/// </summary>
void Tooltip::Show(LPCWSTR text, LPRECT position)
{
    mWindow->SetText(text);
    mWindow->SizeToText(mMaxWidth, mMaxHeight);

    // Show it centerd on x, 5 px above, while forcing it to stay on the virtual desktop
    MonitorInfo* monInfo = mWindow->GetMonitorInformation();

    mWindow->Move(
        std::min<float>(std::max<float>(monInfo->m_virtualDesktop.rect.left, position->left + (position->right - position->left)/2.0f - mWindow->GetSize().width/2),
            monInfo->m_virtualDesktop.rect.right - mWindow->GetSize().width),
        std::min<float>(std::max<float>(monInfo->m_virtualDesktop.rect.top, position->top - mWindow->GetSize().height - 5),
            monInfo->m_virtualDesktop.rect.bottom - mWindow->GetSize().height));

    mWindow->Show();
}


/// <summary>
/// Hides the tooltip
/// </summary>
void Tooltip::Hide()
{
    mWindow->Hide();
}


/// <summary>
/// Loads settings from the RC files
/// </summary>
void Tooltip::LoadSettings()
{
    mMaxHeight = mSettings->GetInt(_T("MaxHeight"), 100);
    mMaxWidth = mSettings->GetInt(_T("MaxWidth"), 300);

    StateRender<States>::InitData stateInitData;
    stateInitData[States::Base].defaults.brushSettings[State::BrushType::Background].color = Color::Create(0xCCFAFAD2);
    stateInitData[States::Base].defaults.brushSettings[State::BrushType::Text].color = Color::Create(0xFF000000);
    stateInitData[States::Base].defaults.textOffsetTop = 2;
    stateInitData[States::Base].defaults.textOffsetBottom = 2;
    stateInitData[States::Base].defaults.textOffsetRight = 2;
    stateInitData[States::Base].defaults.textOffsetLeft = 2;
    stateInitData[States::Base].defaults.brushSettings[State::BrushType::Outline].color = Color::Create(0xFF000000);
    stateInitData[States::Base].defaults.outlineWidth = 0.75f;
    mStateRender.Load(stateInitData, mSettings);
}


/// <summary>
/// Handles Window messages
/// </summary>
LRESULT WINAPI Tooltip::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID)
{
    return DefWindowProc(window, message, wParam, lParam);
}
