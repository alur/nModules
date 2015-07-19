#include "Api.h"

#include <string>
#include <unordered_map>

std::unordered_map<std::wstring, IDataProvider*> sDataProviders;


EXPORT_CDECL(void) DataChanged(int num, LPCWSTR name, ...) {

}


EXPORT_CDECL(bool) RegisterDataProvider(LPCWSTR name, IDataProvider *provider) {
  sDataProviders.emplace(name, provider);
  return true;
}


EXPORT_CDECL(void) UnregisterDataProvider(LPCWSTR name) {

}
