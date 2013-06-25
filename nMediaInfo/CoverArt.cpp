/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  CoverArt.cpp
*  The nModules Project
*
*  Renders cover art.
*   
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "CoverArt.hpp"
//#define ID3LIB_LINKOPTION 1
//#include "../External/id3lib/id3/tag.h"
#include <wincodec.h>
#include "../nShared/Factories.h"
#include "../nShared/Macros.h"
#include <Shlwapi.h>
#include <strsafe.h>
#include "../nShared/FileIterator.hpp"
#include "../nShared/Debugging.h"

#define TAGLIB_STATIC
#include "../External/taglib/fileref.h"
#include "../External/taglib/mpeg/mpegfile.h"
#include "../External/taglib/mpeg/id3v2/id3v2tag.h"
#include "../External/taglib/mpeg/id3v2/frames/attachedpictureframe.h"
#include "../External/taglib/flac/flacfile.h"
#include "../External/taglib/mp4/mp4file.h"

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

    D2D1_RECT_F pos = D2D1::RectF(
        0, 0,
        (float)this->window->GetDrawingSettings()->width,
        (float)this->window->GetDrawingSettings()->height
    );
    this->coverArt = this->window->AddOverlay(pos, (IWICBitmapSource*)nullptr);

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
    if ((WA2Window = FindWindow("Winamp v1.x", nullptr)) == nullptr) {
        return;
    }

    // Get some basic info
    trackID = (int)SendMessageW(WA2Window, WM_USER, 0, IPC_GETLISTPOS);

    // Open a handle to winamp
    GetWindowThreadProcessId(WA2Window, &winampProc);
    if ((winampHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, winampProc)) == nullptr) {
        return;
    }

    // Read the file path
    ReadProcessMemory(winampHandle, (LPCVOID)SendMessageW(WA2Window, WM_USER, trackID, IPC_GETPLAYLISTFILEW), &filePath, sizeof(filePath), nullptr);

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
bool CoverArt::SetCoverFromTag(LPCWSTR filePath) {
    LPCWSTR extension = wcsrchr(filePath, L'.');

    if (extension == nullptr) {
        return false;
    }

    auto ParseImage = [this] (const BYTE *data, UINT size) {
        IWICImagingFactory *factory = nullptr;
        IWICBitmapDecoder *decoder = nullptr;
        IWICBitmapFrameDecode *source = nullptr;
        HRESULT hr = E_FAIL;

        IStream *stream = SHCreateMemStream(data, size);
        if (stream) {
            hr = Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));
            if (SUCCEEDED(hr)) {
                hr = factory->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnDemand, &decoder);
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

        return hr == S_OK;
    };

    ++extension;

    if (_wcsicmp(extension, L"mp3") == 0) {
        TagLib::MPEG::File mp3File(filePath);
        if (mp3File.ID3v2Tag()->frameListMap().contains("APIC")) {
            for (auto frame : mp3File.ID3v2Tag()->frameListMap()["APIC"]) {
                auto picFrame = (TagLib::ID3v2::AttachedPictureFrame *)frame;
                if (picFrame->type() == TagLib::ID3v2::AttachedPictureFrame::FrontCover) {
                    return ParseImage((const BYTE *)picFrame->picture().data(), picFrame->picture().size());
                }
            }
        }
    }
    else if (_wcsicmp(extension, L"flac") == 0) {
        TagLib::FLAC::File flacFile(filePath);
        for (auto &picture : flacFile.pictureList()) {
            if (picture->type() == TagLib::FLAC::Picture::FrontCover) {
                return ParseImage((const BYTE *)picture->data().data(), picture->data().size());
            }
        }
    }
    else if (_wcsicmp(extension, L"mp4") == 0) {
        TagLib::MP4::File mp4File(filePath);
        if (mp4File.tag()->itemListMap().contains("covr")) {
            auto &cover = mp4File.tag()->itemListMap()["covr"].toCoverArtList().front();
            return ParseImage((const BYTE *)cover.data().data(), cover.data().size());
        }
    }

    return false;
}


/// <summary>
/// Tries to get the cover from the specified folder.
/// </summary>
/// <param name="filePath">Path to the file to get the cover from.</param>
bool CoverArt::SetCoverFromFolder(LPCWSTR filePath) {
    WCHAR folderPath[MAX_PATH] = {0};
    IWICImagingFactory *factory = nullptr;
    IWICBitmapDecoder *decoder = nullptr;
    IWICBitmapFrameDecode *source = nullptr;
    HRESULT hr = E_FAIL;

    StringCchCopyW(folderPath, sizeof(folderPath), filePath);
    PathRemoveFileSpecW(folderPath);

    // Check each covername
    WCHAR artPath[MAX_PATH];
    for (auto &canidate : this->folderCanidates) {
        StringCchPrintfW(artPath, sizeof(artPath), L"%s\\%s", folderPath, canidate.c_str());
        for (auto &file : FileIterator(artPath)) {
            StringCchPrintfW(artPath, sizeof(artPath), L"%s\\%s", folderPath, file.cFileName);

            hr = Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));
            if (SUCCEEDED(hr)) {
                hr = factory->CreateDecoderFromFilename(artPath, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
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
    IWICImagingFactory *factory = nullptr;
    IWICBitmapDecoder *decoder = nullptr;
    IWICBitmapFrameDecode *source = nullptr;
    HRESULT hr = E_FAIL;

    hr = Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));
    if (SUCCEEDED(hr)) {
        hr = factory->CreateDecoderFromFilename(this->defaultCoverArt, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
    }
    if (SUCCEEDED(hr)) {
        hr = decoder->GetFrame(0, &source);
    }
    if (SUCCEEDED(hr)) {
        (*this->coverArt)->SetSource(source);
    }
    else {
        (*this->coverArt)->SetSource(nullptr);
    }

    SAFERELEASE(decoder);
}
