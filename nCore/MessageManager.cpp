//-------------------------------------------------------------------------------------------------
// /nCore/nCore.cpp
// The nModules Project
//
// Main .cpp file for the nCore module.
//-------------------------------------------------------------------------------------------------
#include "../Utilities/Common.h"

#include "../Utilities/Macros.h"

#include <unordered_set>

static std::unordered_multiset<UINT, HWND> sRegisteredWindows;


EXPORT_CDECL(void) RegisterForCoreMessages(HWND, UINT message[]) {
}


EXPORT_CDECL(void) UnregisterForCoreMessages(HWND, UINT message[]) {
}


void SendCoreMessage(UINT message, WPARAM, LPARAM) {
}
