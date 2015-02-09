#include "State.hpp"


State::State(ISettingsReader *settingsReader, State *base)
  : mBase(base)
{
  settingsReader->GetString(L"Font", mSettings.font, _countof(mSettings.font), L"");

  mSettings.textPadding = settingsReader->GetLTRBRect(L"TextOffset", NRECT(
    NLENGTH(0, 0, 0), NLENGTH(0, 0, 0), NLENGTH(0, 0, 0), NLENGTH(0, 0, 0)));

  DWORD color = (DWORD)settingsReader->GetInt64(L"Color", 0x55C0448F);
  mColor.a = (color >> 24) / 255.0f;
  mColor.r = (color >> 16 & 0xFF) / 255.0f;
  mColor.g = (color >> 8 & 0xFF) / 255.0f;
  mColor.b = (color & 0xFF) / 255.0f;
}


State::~State() {
}
