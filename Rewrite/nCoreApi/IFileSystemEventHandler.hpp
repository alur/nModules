#pragma once

#include "ApiDefs.h"

#include "../Headers/Windows.h"

#include <Shobjidl.h>

/// <summary>
/// Handles events sent by the file system.
/// </summary>
class IFileSystemEventHandler {
public:
  struct Thumbnail {
    SIZE size;
    union {
      HICON icon;
      HBITMAP bitmap;
    } handle;
    enum class Type {
      HICON,
      HBITMAP
    } type;
  };

  struct Item {
    PITEMID_CHILD id;
    Thumbnail thumbnail;
  };
};
