#pragma once

#include "../nUtilities/Version.h"
#include "../nUtilities/Windows.h"

class LsModule {
public:
  LsModule(const LPCWSTR name, VERSION);

public:
  LsModule(LsModule&) = delete;
  LsModule&operator=(LsModule&) = delete;

public:
  HRESULT CreateMessageHandler(HINSTANCE coreInstance, WNDPROC);
  void DestroyMessageHandler();

  LRESULT HandleGetRevId(LPARAM lParam) const;
  HWND GetMessageWindow() const;

private:
  HWND mMessageWindow;
  const LPCWSTR mName;
  const VERSION mVersion;
};
