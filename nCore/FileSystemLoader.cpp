//-------------------------------------------------------------------------------------------------
// /nCore/FileSystemLoader.cpp
// The nModules Project
//
// Loads folders and folder items.
//
// Exports the following functions:
//   - void CancelLoad(UINT64 id)
//   - UINT64 LoadFolder(LoadFolderRequest&, FileSystemLoaderResponseHandler*)
//-------------------------------------------------------------------------------------------------
#include "CoreMessages.h"
#include "FileSystemLoader.h"

#include "../Utilities/Macros.h"

#include <CommonControls.h>
#include <shellapi.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#include <thread>
#include <Thumbcache.h>
#include <unordered_map>
#include <vector>

extern HWND ghWndMsgHandler;

struct RequestData {
  RequestData(FileSystemLoaderResponseHandler *handler) {
    this->handler = handler;
    abort = new bool;
    *abort = false;
  }

  FileSystemLoaderResponseHandler *handler;
  bool *abort;
};

static UINT64 sNextRequestId = 0;
static std::unordered_map<UINT64, RequestData> sOutstandingRequests;


/// <summary>
/// Tries to load the icon using IThumbnailProvider.
/// </summary>
static HRESULT LoadIconUsingThumbnailProvider(LoadThumbnailResponse &response, int iconSize, IShellFolder2 *shellFolder, LPCITEMIDLIST *item) {
  IThumbnailProvider *thumbnailProvider;
  HRESULT hr = shellFolder->GetUIObjectOf(nullptr, 1, item, IID_IThumbnailProvider, nullptr,
    reinterpret_cast<LPVOID*>(&thumbnailProvider));
  if (SUCCEEDED(hr)) {
    WTS_ALPHATYPE alphaType;
    hr = thumbnailProvider->GetThumbnail(iconSize, &response.thumbnail.bitmap, &alphaType);
    if (SUCCEEDED(hr)) {
      BITMAP bmp;
      GetObjectW(response.thumbnail.bitmap, sizeof(BITMAP), &bmp);
      response.size.height = (FLOAT)bmp.bmHeight;
      response.size.width = (FLOAT)bmp.bmWidth;
      response.type = LoadThumbnailResponse::Type::HBITMAP;
    }
    thumbnailProvider->Release();
  }
  return hr;
}


/// <summary>
/// Tries to load the icon using IExtractImage.
/// </summary>
static HRESULT LoadIconUsingExtractImage(LoadThumbnailResponse &response, int iconSize, IShellFolder2 *shellFolder, LPCITEMIDLIST *item) {
  IExtractImage *extractImage;
  HRESULT hr = shellFolder->GetUIObjectOf(nullptr, 1, item, IID_IExtractImage, nullptr,
    reinterpret_cast<LPVOID*>(&extractImage));
  if (SUCCEEDED(hr)) {
    WCHAR location[MAX_PATH];
    SIZE size = { iconSize, iconSize };
    DWORD flags = 0;
    DWORD priority = IEIT_PRIORITY_NORMAL;

    hr = extractImage->GetLocation(location, _countof(location), &priority, &size, 0, &flags);

    if (SUCCEEDED(hr)) {
      hr = extractImage->Extract(&response.thumbnail.bitmap);
    }

    if (SUCCEEDED(hr)) {
      response.type = LoadThumbnailResponse::Type::HBITMAP;
    }
    extractImage->Release();
  }
  return hr;
}


/// <summary>
/// Tries to load the icon using IExtractIcon.
/// </summary>
static HRESULT LoadIconUsingExtractIcon(LoadThumbnailResponse &response, int iconSize, IShellFolder2 *shellFolder, LPCITEMIDLIST *item) {
  int iconIndex = 0;
  WCHAR iconFile[MAX_PATH];
  UINT flags;
  IExtractIconW *extractIcon;
  HRESULT hr = shellFolder->GetUIObjectOf(nullptr, 1, item, IID_IExtractIconW, nullptr,
    reinterpret_cast<LPVOID*>(&extractIcon));

  // Get the location of the file containing the appropriate icon, and the index of the icon.
  if (SUCCEEDED(hr)) {
    hr = extractIcon->GetIconLocation(GIL_FORSHELL, iconFile, MAX_PATH, &iconIndex, &flags);

    // Extract the icon.
    if (SUCCEEDED(hr)) {
      if (wcscmp(iconFile, L"*") == 0) { // * always leads to bogus icons (32x32) :/
        IImageList *imageList;
        hr = SHGetImageList(iconSize > 48 ? SHIL_JUMBO : (iconSize > 32 ? SHIL_EXTRALARGE : (iconSize > 16 ? SHIL_LARGE : SHIL_SMALL)),
          IID_IImageList, reinterpret_cast<LPVOID*>(&imageList));
        if (SUCCEEDED(hr)) {
          hr = imageList->GetIcon(iconIndex, ILD_TRANSPARENT, &response.thumbnail.icon);
          imageList->Release();
        }
      } else {
        hr = extractIcon->Extract(iconFile, iconIndex, &response.thumbnail.icon, nullptr,
          MAKELONG(iconSize, 0));
      }
    }

    // If the extraction failed, fall back to a 32x32 icon.
    if (hr == S_FALSE) {
      hr = extractIcon->Extract(iconFile, iconIndex, &response.thumbnail.icon, nullptr,
        MAKELONG(32, 0));
    }

    // Add it as an overlay.
    if (hr == S_OK) {
      response.type = LoadThumbnailResponse::Type::HICON;
    }
    extractIcon->Release();
  }
  return hr;
}


static void LoadThumbnail(LoadThumbnailResponse &response, int iconSize, IShellFolder2 *folder, LPCITEMIDLIST *item) {
  response.size.height = (FLOAT)iconSize;
  response.size.width = (FLOAT)iconSize;
  HRESULT hr = LoadIconUsingThumbnailProvider(response, iconSize, folder, item);
  if (hr != S_OK) {
    hr = LoadIconUsingExtractImage(response, iconSize, folder, item);
  }
  if (hr != S_OK) {
    hr = LoadIconUsingExtractIcon(response, iconSize, folder, item);
  }
  if (hr != S_OK) {
    response.thumbnail.icon = LoadIcon(nullptr, IDI_ERROR);
    response.type = LoadThumbnailResponse::Type::HICON;
  }
}


static void LoadFolderItemThread(LoadItemRequest request, UINT64 requestId, volatile bool *abort, HWND hwnd) {
  CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

  LoadItemResponse item;
  item.id = request.id;
  LoadThumbnail(item.thumbnail, request.targetIconWidth, request.folder, (LPCITEMIDLIST*)&request.id);

  if (!*abort) {
    SendMessage(hwnd, NCORE_FILE_SYSTEM_ITEM_LOAD_COMPLETE, (WPARAM)requestId, (LPARAM)&item);
  }
  delete abort;

  ILFree(item.id);
  if (item.thumbnail.type == LoadThumbnailResponse::Type::HBITMAP) {
    DeleteObject(item.thumbnail.thumbnail.bitmap);
  } else if (item.thumbnail.type == LoadThumbnailResponse::Type::HICON) {
    DestroyIcon(item.thumbnail.thumbnail.icon);
  } else {
    ASSERT(false);
  }

  CoUninitialize();
}


static void LoadFolderThread(LoadFolderRequest request, UINT64 requestId, volatile bool *abort, HWND hwnd) {
  LoadFolderResponse response;

  CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

  IEnumIDList *enumIdList;
  if (SUCCEEDED(request.folder->EnumObjects(nullptr, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &enumIdList))) {
    PIDLIST_RELATIVE idNext;
    while (!*abort && enumIdList->Next(1, &idNext, nullptr) != S_FALSE) {
      STRRET ret;
      if (SUCCEEDED(request.folder->GetDisplayNameOf(idNext, SHGDN_FORPARSING, &ret))) {
        WCHAR buffer[MAX_PATH];
        StrRetToBufW(&ret, idNext, buffer, _countof(buffer));
        if (request.blackList.count(buffer) == 0) {
          response.items.emplace_back();
          LoadItemResponse &item = response.items.back();
          item.id = idNext;
          LoadThumbnail(item.thumbnail, request.targetIconWidth, request.folder, (LPCITEMIDLIST*)&idNext);
        } else {
          CoTaskMemFree(idNext);
        }
      }
    }
    enumIdList->Release();
  }

  if (!*abort) {
    SendMessage(hwnd, NCORE_FILE_SYSTEM_LOAD_COMPLETE, (WPARAM)requestId, (LPARAM)&response);
  }

  delete abort;

  for (LoadItemResponse &item : response.items) {
    CoTaskMemFree(item.id);
    if (item.thumbnail.type == LoadThumbnailResponse::Type::HBITMAP) {
      DeleteObject(item.thumbnail.thumbnail.bitmap);
    } else if (item.thumbnail.type == LoadThumbnailResponse::Type::HICON) {
      DestroyIcon(item.thumbnail.thumbnail.icon);
    } else {
      ASSERT(false);
    }
  }

  request.folder->Release();

  CoUninitialize();
}


/// <summary>
/// Asynchronously loads the contents of a folder.
/// </summary>
EXPORT_CDECL(UINT64) LoadFolder(LoadFolderRequest &request, FileSystemLoaderResponseHandler *handler) {
  UINT64 requestId = ++sNextRequestId;
  auto requestData = sOutstandingRequests.emplace(
    std::piecewise_construct,
    std::forward_as_tuple(requestId),
    std::forward_as_tuple(handler)).first;

  request.folder->AddRef();

  std::thread(LoadFolderThread, request, requestId, requestData->second.abort,
    ghWndMsgHandler).detach();

  return requestId;
}


/// <summary>
/// Asynchronously loads a folder item.
/// </summary>
EXPORT_CDECL(UINT64) LoadFolderItem(LoadItemRequest &request, FileSystemLoaderResponseHandler *handler) {
  UINT64 requestId = ++sNextRequestId;
  auto requestData = sOutstandingRequests.emplace(
    std::piecewise_construct,
    std::forward_as_tuple(requestId),
    std::forward_as_tuple(handler)).first;

  request.folder->AddRef();

  std::thread(LoadFolderItemThread, request, requestId, requestData->second.abort,
    ghWndMsgHandler).detach();

  return requestId;
}


/// <summary>
/// Cancels an outstanding request.
/// </summary>
EXPORT_CDECL(void) CancelLoad(UINT64 id) {
  auto request = sOutstandingRequests.find(id);
  ASSERT(request != sOutstandingRequests.end());
  *request->second.abort = true;
  sOutstandingRequests.erase(request);
}


/// <summary>
/// Called by nCores window procedure when it receives a message from the thread.
/// </summary>
void LoadCompleted(UINT64 id, LPVOID result) {
  auto request = sOutstandingRequests.find(id);
  if (request != sOutstandingRequests.end()) {
    request->second.handler->FolderLoaded(id, (LoadFolderResponse*)result);
  }
}


/// <summary>
/// Called by nCores window procedure when it receives a message from the thread.
/// </summary>
void LoadItemCompleted(UINT64 id, LPVOID result) {
  auto request = sOutstandingRequests.find(id);
  if (request != sOutstandingRequests.end()) {
    request->second.handler->ItemLoaded(id, (LoadItemResponse*)result);
  }
}
