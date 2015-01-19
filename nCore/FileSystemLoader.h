//-------------------------------------------------------------------------------------------------
// /nCore/FileSystemLoader.h
// The nModules Project
//
// Loads folders and folder items.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "FileSystemLoaderResponseHandler.hpp"

#include "../Utilities/CommonD2D.h"
#include "../Utilities/StringUtils.h"

#include <Shobjidl.h>
#include <Shlwapi.h>

struct LoadFolderRequest {
  // A set of item names which should not be included in the response.
  StringKeyedSets<std::wstring>::UnorderedSet blackList;
  // The desired icon size.
  UINT targetIconWidth;
  // The folder to load from.
  IShellFolder2 *folder;
};

struct LoadItemRequest {
  // The desired icon size.
  UINT targetIconWidth;
  // The folder to load from.
  IShellFolder2 *folder;
  // The item to load.
  PITEMID_CHILD id;
};


struct LoadThumbnailResponse {
  D2D1_SIZE_F size;
  union {
    HICON icon;
    HBITMAP bitmap;
  } thumbnail;
  enum class Type {
    HICON,
    HBITMAP
  } type;
};

struct LoadItemResponse {
  LoadThumbnailResponse thumbnail;
  PITEMID_CHILD id;
};

struct LoadFolderResponse {
  std::vector<LoadItemResponse> items;
};
