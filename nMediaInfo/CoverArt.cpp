/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  CoverArt.hpp
*  The nModules Project
*
*  Renders cover art.
*   
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/LiteStep.h"
#include "CoverArt.hpp"
#define ID3LIB_LINKOPTION 1
#include "..\External\id3lib\id3\tag.h"
#include <wincodec.h>
#include "../nShared/Factories.h"
#include "../nShared/Macros.h"
#include <Shlwapi.h>
#include <strsafe.h>

#define IPC_GETLISTPOS 125
#define IPC_GETPLAYLISTFILEW 214


/// <summary>
/// Constructor.
/// </summary>
/// <param name="name">The settings prefix to use.</param>
CoverArt::CoverArt(LPCSTR name) : Drawable(name) {
    DrawableSettings defaults;
    defaults.width = 200;
    defaults.height = 200;
    this->window->Initialize(&defaults);

    D2D1_RECT_F pos = {
        0, 0,
        (float)this->window->GetDrawingSettings()->width,
        (float)this->window->GetDrawingSettings()->height
    };
    this->coverArt = this->window->AddOverlay(pos, (IWICBitmap*)NULL);

    this->folderCanidates.push_back(L"*.jpg");
    this->folderCanidates.push_back(L"*.png");

    this->settings->GetString("DefaultCoverArt", this->defaultCoverArt, MAX_PATH, "");

    Update();

    this->window->Show();
}


/// <summary>
/// Destructor.
/// </summary>
CoverArt::~CoverArt() {
}


/// <summary>
/// Updates the image used.
/// </summary>
void CoverArt::Update() {
    HANDLE winampHandle;
    ULONG winampProc;
    HWND WA2Window;
    int trackID;
    WCHAR filePath[MAX_PATH];

    // Get Winamps HWND
    if ((WA2Window = FindWindow("Winamp v1.x", NULL)) == NULL) {
        return;
    }

    // Get some basic info
    trackID = (int)SendMessageW(WA2Window, WM_USER, 0, IPC_GETLISTPOS);

    // Open a handle to winamp
    GetWindowThreadProcessId(WA2Window, &winampProc);
    if ((winampHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, winampProc)) == NULL) {
        return;
    }

    // Read the file path
    ReadProcessMemory(winampHandle, (LPCVOID)SendMessageW(WA2Window, WM_USER, trackID, IPC_GETPLAYLISTFILEW), &filePath, sizeof(filePath), NULL);

    CloseHandle(winampHandle);

    if (!SetCoverFromTag(filePath)) {
        if (!SetCoverFromFolder(filePath)) {
            SetDefaultCover();
        }
    }

    this->window->Repaint();
}


/// <summary>
/// Handles window messages.
/// </summary>
LRESULT WINAPI CoverArt::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LPVOID) {
    this->eventHandler->HandleMessage(hWnd, uMsg, wParam, lParam);
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// <summary>
/// Tries to set the cover based on the Tags of the specified file.
/// </summary>
/// <param name="filePath">Path to the file to get the cover from.</param>
bool CoverArt::SetCoverFromTag(LPCWSTR filePathW) {
    char filePath[MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, filePathW, -1, filePath, sizeof(filePath), NULL, NULL);

	ID3_Tag tag(filePath);
	ID3_Frame* frame;
    IWICImagingFactory* factory = NULL;
    IWICBitmapDecoder* decoder = NULL;
    IWICBitmapFrameDecode* source = NULL;
    HRESULT hr = E_FAIL;
    
    frame = tag.Find(ID3FID_PICTURE, ID3FN_PICTURETYPE, ID3PT_COVERFRONT);
    if (frame != NULL) {
        IStream* stream = SHCreateMemStream(frame->Field(ID3FN_DATA).GetRawBinary(), (UINT)frame->Field(ID3FN_DATA).Size());
        if (stream) {
            hr = Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));
            if (SUCCEEDED(hr)) {
                hr = factory->CreateDecoderFromStream(stream, NULL, WICDecodeMetadataCacheOnDemand, &decoder);
            }
            if (SUCCEEDED(hr)) {
                hr = decoder->GetFrame(0, &source);
            }
            if (SUCCEEDED(hr)) {
                (*this->coverArt)->SetSource(source);
            }

            SAFERELEASE(decoder);
            SAFERELEASE(stream);
        }
    }

    return hr == S_OK;
}


/// <summary>
/// Tries to get the cover from the specified folder.
/// </summary>
/// <param name="filePath">Path to the file to get the cover from.</param>
bool CoverArt::SetCoverFromFolder(LPCWSTR filePath) {
    WCHAR folderPath[MAX_PATH] = {0};
    IWICImagingFactory* factory = NULL;
    IWICBitmapDecoder* decoder = NULL;
    IWICBitmapFrameDecode* source = NULL;
    HRESULT hr = E_FAIL;

    StringCchCopyW(folderPath, sizeof(folderPath), filePath);
    PathRemoveFileSpecW(folderPath);

    // Check each covername
    WCHAR artPath[MAX_PATH];
    WIN32_FIND_DATAW FindFileData;

    for (list<wstring>::const_iterator canidate = this->folderCanidates.begin(); canidate != this->folderCanidates.end(); ++canidate) {
        StringCchPrintfW(artPath, sizeof(artPath), L"%s\\%s", folderPath, canidate->c_str());

        if (FindFirstFileW(artPath, &FindFileData) != INVALID_HANDLE_VALUE) {
            // Todo::We could try other files if this fails.
            StringCchPrintfW(artPath, sizeof(artPath), L"%s\\%s", folderPath, FindFileData.cFileName);

            hr = Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));
            if (SUCCEEDED(hr)) {
                hr = factory->CreateDecoderFromFilename(artPath, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
            }
            if (SUCCEEDED(hr)) {
                hr = decoder->GetFrame(0, &source);
            }
            if (SUCCEEDED(hr)) {
                (*this->coverArt)->SetSource(source);
            }

            SAFERELEASE(decoder);

            if (SUCCEEDED(hr)) {
                return true;
            }
        }
    }
    
    return false;
}


/// <summary>
/// Sets the default cover -- when we couldn't find any other cover.
/// </summary>
void CoverArt::SetDefaultCover() {
    IWICImagingFactory* factory = NULL;
    IWICBitmapDecoder* decoder = NULL;
    IWICBitmapFrameDecode* source = NULL;
    HRESULT hr = E_FAIL;

    hr = Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));
    if (SUCCEEDED(hr)) {
        hr = factory->CreateDecoderFromFilename(this->defaultCoverArt, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
    }
    if (SUCCEEDED(hr)) {
        hr = decoder->GetFrame(0, &source);
    }
    if (SUCCEEDED(hr)) {
        (*this->coverArt)->SetSource(source);
    }
    else {
        (*this->coverArt)->SetSource(NULL);
    }

    SAFERELEASE(decoder);
}
