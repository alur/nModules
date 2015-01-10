/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Balloon.cpp
 *  The nModules Project
 *
 *  A general balloon popup window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Balloon.hpp"

#include "../nCoreCom/Core.h"

#include "../Utilities/Common.h"

#include <algorithm>


/// <summary>
/// Constructor
/// </summary>
Balloon::Balloon(LPCTSTR prefix, Settings* parentSettings, UINT clickedMessage, MessageHandler* callbackHandler)
  : Drawable(prefix, parentSettings)
{
  mClickedMessage = clickedMessage;
  mCallbackHandler = callbackHandler;
    
  mTitleSettings = parentSettings->CreateChild(L"Title");
  mCloseBtnSettings = parentSettings->CreateChild(L"CloseButton");

  LoadSettings();

  // Initialize the main window.
  WindowSettings windowDefaults;
  WindowSettings windowSettings;
  windowDefaults.width = 150;
  windowDefaults.height = 40;
  windowDefaults.alwaysOnTop = true;
  windowSettings.Load(mSettings, &windowDefaults);
  mWindow->Initialize(windowSettings, &mBackStates);

  // Create the a child window for the title of the balloon.
  mTitleWindow = mWindow->CreateChild(mTitleSettings, this);
  WindowSettings titleDefaults;
  WindowSettings titleSettings;
  titleDefaults.x = 4;
  titleDefaults.y = 4;
  titleSettings.Load(mTitleSettings, &titleDefaults);
  mTitleWindow->Initialize(titleSettings, &mTitleStates);
  if (!titleSettings.hidden) {
    mTitleWindow->Show();
  }

  // And another for the x button in the top right corner.
  mCloseBtnWindow = mWindow->CreateChild(mCloseBtnSettings, this);
  WindowSettings closeBtnDefaults;
  WindowSettings closeBtnSettings;
  closeBtnDefaults.width = 16;
  closeBtnDefaults.height = 16;
  closeBtnDefaults.text[0] = L'X';
  closeBtnDefaults.text[1] = L'\0';
  closeBtnSettings.Load(mCloseBtnSettings, &closeBtnDefaults);
  mCloseBtnWindow->Initialize(closeBtnSettings, &mCloseBtnStates);
  if (!closeBtnSettings.hidden) {
    mCloseBtnWindow->Show();
  }
}


/// <summary>
/// Destructor
/// </summary>
Balloon::~Balloon() {
  SAFEDELETE(mTitleWindow);
  SAFEDELETE(mCloseBtnWindow);
  SAFEDELETE(mTitleSettings);
  SAFEDELETE(mCloseBtnSettings);
}


/// <summary>
/// Handles window messages.
/// </summary>
LRESULT WINAPI Balloon::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID sender) {
  if (message == WM_LBUTTONDOWN) {
    mCallbackHandler->HandleMessage(window, mClickedMessage, sender == mCloseBtnWindow ? 1 : NULL, NULL, sender);
    return 0;
  }
  return DefWindowProc(window, message, wParam, lParam);
}


UINT Balloon::GetClickedMessage() {
  return mClickedMessage;
}


void Balloon::SetClickedMessage(UINT message) {
  mClickedMessage = message;
}


/// <summary>
/// Loads settings from the RC files
/// </summary>
void Balloon::LoadSettings() {
  StateRender<States>::InitData backInit;
  backInit[States::Base].defaults.brushSettings[State::BrushType::Background].color = Color::Create(0xCCFAFAD2);
  backInit[States::Base].defaults.brushSettings[State::BrushType::Text].color = Color::Create(0xFF000000);
  backInit[States::Base].defaults.textOffsetTop = 4;
  backInit[States::Base].defaults.textOffsetBottom = 4;
  backInit[States::Base].defaults.textOffsetRight = 4;
  backInit[States::Base].defaults.textOffsetLeft = 40;
  backInit[States::Base].defaults.brushSettings[State::BrushType::Outline].color = Color::Create(0xAA000000);
  backInit[States::Base].defaults.outlineWidth = 1.5f;
  backInit[States::Base].defaults.wordWrapping = DWRITE_WORD_WRAPPING_WRAP;
  backInit[States::Base].defaults.cornerRadiusX = 4.0f;
  backInit[States::Base].defaults.cornerRadiusY = 4.0f;
  mBackStates.Load(backInit, mSettings);

  StateRender<States>::InitData titleInit;
  titleInit[States::Base].defaults.brushSettings[State::BrushType::Text].color = Color::Create(0xFF000000);
  titleInit[States::Base].defaults.brushSettings[State::BrushType::Background].color = Color::Create(0x00000000);
  titleInit[States::Base].defaults.fontWeight = DWRITE_FONT_WEIGHT_BOLD;
  mTitleStates.Load(titleInit, mTitleSettings);
    
  StateRender<States>::InitData closeInit;
  closeInit[States::Base].defaults.brushSettings[State::BrushType::Background].color = Color::Create(0xAA77AACC);
  closeInit[States::Base].defaults.brushSettings[State::BrushType::Text].color = Color::Create(0xFF000000);
  closeInit[States::Base].defaults.brushSettings[State::BrushType::Outline].color = Color::Create(0xFF000000);
  closeInit[States::Base].defaults.outlineWidth = 1.0f;
  closeInit[States::Base].defaults.cornerRadiusX = 2.0f;
  closeInit[States::Base].defaults.cornerRadiusY = 2.0f;
  closeInit[States::Base].defaults.fontWeight = DWRITE_FONT_WEIGHT_BOLD;
  closeInit[States::Base].defaults.textAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
  closeInit[States::Base].defaults.textVerticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
  mCloseBtnStates.Load(closeInit, mCloseBtnSettings);
}


/// <summary>
/// Hides the balloon
/// </summary>
void Balloon::Hide() {
  mWindow->ClearOverlays();
  mWindow->Hide();
}


/// <summary>
/// Shows the balloon
/// </summary>
void Balloon::Show(LPCWSTR title, LPCWSTR text, HICON icon, LPSIZE iconSize, LPRECT targetPosition) {
  int offsetLeft;
  if (icon != nullptr) {
    D2D1_RECT_F f = { 4, 4, float(4 + iconSize->cx), float(4 + iconSize->cy) };
    mWindow->AddOverlay(f, icon);

    offsetLeft = iconSize->cx + 8;
    mTitleWindow->Move((float)iconSize->cx + 8, 4);
  } else {
    offsetLeft = 4;
    mTitleWindow->Move(4, 4);
  }

  mWindow->SetTextOffsets((float)offsetLeft, 20, 4, 4);

  mTitleWindow->SetText(title);
  mTitleWindow->SizeToText(400, 16, 0, 16);

  mWindow->SetText(text);
  mWindow->SizeToText(400, 400, (int)mTitleWindow->GetSize().width + offsetLeft + 24, 8 + iconSize->cy);

  mCloseBtnWindow->Move(mWindow->GetSize().width - 20, 4);

  // Show it centerd on x, 5 px above, while forcing it to stay on the virtual desktop
  const MonitorInfo::Monitor &virtualDesktop = nCore::FetchMonitorInfo().GetVirtualDesktop();
  mWindow->Move(
    std::min<float>(std::max<float>((float)virtualDesktop.rect.left, (float)targetPosition->left - mWindow->GetSize().width / 2.0f),
    (float)virtualDesktop.rect.right - mWindow->GetSize().width),
    std::min<float>(std::max<float>((float)virtualDesktop.rect.top, (float)targetPosition->top - mWindow->GetSize().height - 5.0f),
    (float)virtualDesktop.rect.bottom - mWindow->GetSize().height));

  mWindow->Show();
}
