#include "Messages.h"
#include "Timers.h"

#include "../nShared/UIDGenerator.hpp"

#include "../nCoreApi/IMessageHandler.hpp"

#include "../Headers/Macros.h"

#include <assert.h>
#include <unordered_map>

static UIDGenerator<UINT_PTR> sIdGenerator(NCORE_LEASEABLE_TIMERS_START);
static std::unordered_map<UINT_PTR, IMessageHandler*> sActiveTimers;

extern HWND gWindow;


EXPORT_CDECL(void) ClearInterval(UINT_PTR id) {
  KillTimer(gWindow, id);
  sActiveTimers.erase(id);
  sIdGenerator.ReleaseId(id);
}


EXPORT_CDECL(UINT_PTR) SetInterval(UINT delay, IMessageHandler *handler) {
  UINT_PTR id = sIdGenerator.GetNewId();
  sActiveTimers[id] = handler;
  SetTimer(gWindow, id, delay, nullptr);
  return id;
}


void Timers::Handle(WPARAM id, LPARAM lParam) {
  sActiveTimers[id]->HandleMessage(gWindow, WM_TIMER, id, lParam, 0);
}
