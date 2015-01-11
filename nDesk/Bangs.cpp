/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Bangs.cpp
 *  The nModules Project
 *
 *  Handles bang commands
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "Bangs.h"
#include "WorkArea.h"
#include "ClickHandler.hpp"
#include "DesktopPainter.hpp"
#include "Settings.h"
#include "../nCoreCom/Core.h"
#include <algorithm>

extern ClickHandler *g_pClickHandler;
extern DesktopPainter *g_pDesktopPainter;

struct BangItem {
  BangItem(LPCTSTR name, LiteStep::BANGCOMMANDPROC command) {
    this->name = name;
    this->command = command;
  }

  LPCTSTR name;
  LiteStep::BANGCOMMANDPROC command;
};

static const BangItem bangMap[] = {
  // Sets the work area.
  BangItem(L"SetWorkArea", [] (HWND, LPCTSTR args) {
    WorkArea::ParseLine(&nCore::FetchMonitorInfo(), args);
    SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETWORKAREA, 0);
  }),

  // Adds a click handler.
  BangItem(L"On", [] (HWND, LPCTSTR args) {
    g_pClickHandler->AddHandler(args);
  }),

  // Removes click handlers.
  BangItem(L"Off", [] (HWND, LPCTSTR args) {
    g_pClickHandler->RemoveHandlers(args);
  }),

  // Sets the transition duration.
  BangItem(L"SetTransitionDuration", [] (HWND, LPCTSTR args) {
    g_pDesktopPainter->SetTransitionTime(_wtoi(args));
  }),

  // Sets the transition square size. Force it to be >= 2.
  BangItem(L"SetTransitionSquareSize", [] (HWND, LPCTSTR args) {
    g_pDesktopPainter->SetSquareSize(std::max(2, _wtoi(args)));
  }),

  // Sets the transition effect.
  BangItem(L"SetTransitionEffect", [] (HWND, LPCTSTR args) {
    g_pDesktopPainter->SetTransitionType(nDesk::Settings::TransitionTypeFromString(args));
  }),

  // Sets or clears the invalid all on update flag.
  BangItem(L"SetInvalidateAllOnUpdate", [] (HWND, LPCTSTR args) {
    g_pDesktopPainter->SetInvalidateAllOnUpdate(LiteStep::ParseBool(args));
  })
};


/// <summary>
/// Registers bangs.
/// </summary>
void Bangs::_Register() {
  wchar_t bangName[64];
  for (auto &bang : bangMap) {
    StringCchPrintf(bangName, _countof(bangName), L"!nDesk%s", bang.name);
    LiteStep::AddBangCommand(bangName, bang.command);
  }
}


/// <summary>
/// Unregisters bangs.
/// </summary>
void Bangs::_Unregister() {
  wchar_t bangName[64];
  for (auto &bang : bangMap) {
    StringCchPrintf(bangName, _countof(bangName), L"!nDesk%s", bang.name);
    LiteStep::RemoveBangCommand(bangName);
  }
}
