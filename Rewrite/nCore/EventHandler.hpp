#pragma once

#include "../nCoreApi/IEventHandler.hpp"
#include "../nCoreApi/IEventProcessor.hpp"
#include "../nCoreApi/ISettingsReader.hpp"

class EventHandler : public IEventHandler {
public:
  EventHandler(const ISettingsReader *settingsReader);

public:
  EventHandler(const EventHandler&) = delete;
  EventHandler &operator=(const EventHandler&) = delete;

  // IMessageHandler
public:
  LRESULT APICALL HandleMessage(HWND, UINT message, WPARAM, LPARAM, IEventProcessor*) override;

  // IEventHandler
public:
  void APICALL Destroy() override;
};
