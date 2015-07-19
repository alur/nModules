#include "../Headers/Windows.h"


BOOL APIENTRY DllMain(HANDLE module, DWORD reasonForCall, LPVOID /* reserved */) {
  if (reasonForCall == DLL_PROCESS_ATTACH) {
#ifdef _DLL
    DisableThreadLibraryCalls((HINSTANCE)module);
#endif
  }
  return TRUE;
}
