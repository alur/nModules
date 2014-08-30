//-------------------------------------------------------------------------------------------------
// /nTray/Types.h
// The nModules Project
//
// Shared types for nTray.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "Tray.hpp"

#include <string>
#include <unordered_map>

typedef std::unordered_map<std::wstring, Tray> TrayMap;

struct IconData {
  GUID guidItem;
  UINT uID;
  HWND hwnd;
};
