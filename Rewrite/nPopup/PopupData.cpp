#include "PopupData.hpp"

#include "../nCoreApi/Core.h"


PopupData::PopupData(LPCWSTR prefix) {
  ISettingsReader *reader = nCore::CreateSettingsReader(prefix, nullptr);

  mConfineToMonitor = reader->GetBool(L"ConfineToMonitor", false);
  mConfineToWorkArea = reader->GetBool(L"ConfineToWorkArea", false);

  reader->Discard();
}


bool PopupData::ConfineToMonitor() const {
  return mConfineToMonitor;
}


bool PopupData::ConfineToWorkArea() const {
  return mConfineToWorkArea;
}
