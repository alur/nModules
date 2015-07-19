#include "Label.hpp"

#include "../nCoreApi/Core.h"

#include "../Headers/lsapi.h"

const StateDefinition sStates[] = {
  { L"Hover",   0, 0 }, // 1
  { L"Pressed", 0, 0 }, // 2
};


Label::Label(LPCWSTR name) : mButtonsPressed(0) {
  ISettingsReader *reader = nCore::CreateSettingsReader(name, nullptr);

  mEventHandler = nCore::CreateEventHandler(reader);
  mTextPainter = nCore::CreateTextPainter(reader, sStates, _countof(sStates));
  mBackgroundPainter = nCore::CreateBackgroundPainter(reader, sStates, _countof(sStates));

  PaneInitData paneInitData;
  ZeroMemory(&paneInitData, sizeof(PaneInitData));
  paneInitData.cbSize = sizeof(PaneInitData);
  paneInitData.messageHandler = this;
  paneInitData.name = name;
  IPainter *painters[] = { mBackgroundPainter, mTextPainter, nCore::GetChildPainter() };
  paneInitData.painters = painters;
  paneInitData.numPainters = _countof(painters);
  paneInitData.settingsReader = reader;
  paneInitData.flags = PaneInitData::DynamicText;
  paneInitData.states = sStates;
  paneInitData.numStates = _countof(sStates);
  mPane = nCore::CreatePane(&paneInitData);

  wchar_t buffer[MAX_LINE_LENGTH];
  if (reader->GetString(L"Text", buffer, _countof(buffer), L"")) {
    mPane->SetText(buffer);
  }

  reader->Discard();

  mPane->Show();
}


Label::~Label() {
  mPane->Discard();
  mTextPainter->Discard();
  mBackgroundPainter->Discard();
  mEventHandler->Discard();
}


LRESULT Label::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, NPARAM) {
  switch (message) {
  case WM_MOUSEMOVE:
    mPane->ActivateState(State::Hover);
    break;

  case WM_MOUSELEAVE:
    mButtonsPressed = 0;
    mPane->ClearState(State::Hover);
    mPane->ClearState(State::Pressed);
    break;

  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
  case WM_MBUTTONDOWN:
  case WM_XBUTTONDOWN:
    ++mButtonsPressed;
    mPane->ActivateState(State::Pressed);
    break;

  case WM_LBUTTONUP:
  case WM_RBUTTONUP:
  case WM_MBUTTONUP:
  case WM_XBUTTONUP:
    if (--mButtonsPressed <= 0) {
      mPane->ClearState(State::Pressed);
    }
    break;
  }
  return mEventHandler->HandleMessage(window, message, wParam, lParam, nullptr);
}
