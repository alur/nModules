#pragma once

#include "../nCoreApi/ILogger.hpp"

#include "../nUtilities/Version.h"
#include "../nUtilities/Windows.h"

class NModule {
public:
  NModule(LPCWSTR name, VERSION version, VERSION coreVersion);

public:
  NModule(const NModule&) = delete;
  NModule &operator=(NModule&) = delete;

public:
  const LPCWSTR name;
  const VERSION version;
  const VERSION coreVersion;

  HINSTANCE instance;
  HWND window;
  ILogger *log;
};
