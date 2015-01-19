//-------------------------------------------------------------------------------------------------
// /nCore/FileSystemLoaderResponseHandler.hpp
// The nModules Project
//
// Class which can receive responses from nCore's FileSystemLoader.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "FileSystemLoader.h"

#include "../Utilities/Common.h"

class FileSystemLoaderResponseHandler {
public:
  virtual LPARAM FolderLoaded(UINT64, struct LoadFolderResponse*) = 0;
  virtual LPARAM ItemLoaded(UINT64, struct LoadItemResponse*) = 0;
};
