/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  CoverArt.cpp
*  The nModules Project
*
*  Renders cover art.
*   
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "CoverArt.hpp"
#include "../nShared/Factories.h"
#include "../Utilities/FileIterator.hpp"
#include "../Utilities/StringUtils.h"
#include "../nShared/LSModule.hpp"
#include "nMediaInfo.h"

#include <Shlwapi.h>
#include <strsafe.h>
#include <wincodec.h>

#define TAGLIB_STATIC
#include "../External/taglib/fileref.h"
#include "../External/taglib/mpeg/mpegfile.h"
#include "../External/taglib/mpeg/id3v2/id3v2tag.h"
#include "../External/taglib/flac/flacfile.h"
#include "../External/taglib/mp4/mp4file.h"

#define IPC_GETLISTPOS 125
#define IPC_GETPLAYLISTFILEW 214


extern LSModule gLSModule;


/// <summary>
/// Constructor.
/// </summary>
/// <param name="name">The settings prefix to use.</param>
CoverArt::CoverArt(LPCTSTR name) : Drawable(name)
{
    StateRender<States>::InitData initData;
    mStateRender.Load(initData, mSettings);

    WindowSettings defaults;
    WindowSettings windowSettings;
    defaults.width = 200;
    defaults.height = 200;
    defaults.registerWithCore = true;
    windowSettings.Load(mSettings, &defaults);
    mWindow->Initialize(windowSettings, &mStateRender);

    LoadSettings();
    //Update();

    mWindow->Show();
}


/// <summary>
/// Destructor.
/// </summary>
CoverArt::~CoverArt()
{
}


/// <summary>
/// Retrives the Type that corresponds to str. If str does not correspond to a valid type,
/// Count is returned.
/// </summary>
TagLib::ID3v2::AttachedPictureFrame::Type CoverArt::ID3TypeFromString(LPCWSTR str)
{
    using namespace TagLib::ID3v2;
    
    static const struct
    {
        AttachedPictureFrame::Type type;
        LPCWSTR name;
    } pictureTypes [] = {
        { AttachedPictureFrame::Type::Other,              L"Other"              },
        { AttachedPictureFrame::Type::FileIcon,           L"FileIcon"           },
        { AttachedPictureFrame::Type::OtherFileIcon,      L"OtherFileIcon"      },
        { AttachedPictureFrame::Type::FrontCover,         L"FrontCover"         },
        { AttachedPictureFrame::Type::BackCover,          L"BackCover"          },
        { AttachedPictureFrame::Type::LeafletPage,        L"LeafletPage"        },
        { AttachedPictureFrame::Type::Media,              L"Media"              },
        { AttachedPictureFrame::Type::LeadArtist,         L"LeadArtist"         },
        { AttachedPictureFrame::Type::Artist,             L"Artist"             },
        { AttachedPictureFrame::Type::Conductor,          L"Conductor"          },
        { AttachedPictureFrame::Type::Band,               L"Band"               },
        { AttachedPictureFrame::Type::Composer,           L"Composer"           },
        { AttachedPictureFrame::Type::Lyricist,           L"Lyricist"           },
        { AttachedPictureFrame::Type::RecordingLocation,  L"RecordingLocation"  },
        { AttachedPictureFrame::Type::DuringRecording,    L"DuringRecording"    },
        { AttachedPictureFrame::Type::DuringPerformance,  L"DuringPerformance"  },
        { AttachedPictureFrame::Type::MovieScreenCapture, L"MovieScreenCapture" },
        { AttachedPictureFrame::Type::ColouredFish,       L"ColouredFish"       },
        { AttachedPictureFrame::Type::Illustration,       L"Illustration"       },
        { AttachedPictureFrame::Type::BandLogo,           L"BandLogo"           },
        { AttachedPictureFrame::Type::PublisherLogo,      L"PublisherLogo"      }
    };

    for (auto type : pictureTypes)
    {
        if (_wcsicmp(type.name, str) == 0)
        {
            return type.type;
        }
    }

    return AttachedPictureFrame::Type::Count;
}


/// <summary>
/// Loads .RC settings
/// </summary>
void CoverArt::LoadSettings()
{
    // Position the image so that it covers the entire window.
    D2D1_RECT_F pos = D2D1::RectF(
        0, 0,
        (float)mWindow->GetSize().width,
        (float)mWindow->GetSize().height
    );
    mCoverArt = mWindow->AddOverlay(pos, (IWICBitmapSource*)nullptr);

    // (group)FileNames -- The file names to search for.
    WCHAR fileNames[MAX_LINE_LENGTH];
    mSettings->GetString(L"FileNames", fileNames, _countof(fileNames), L"AlbumArt*Large.jpg folder.jpg *.jpg *.png *.jpeg *.bmp");
    LiteStep::IterateOverTokens(fileNames, [this] (LPCWSTR fileName)
    {
       mFolderCanidates.push_back(fileName);
    });

    // (group)ID3Priority
    WCHAR ID3Priority[MAX_LINE_LENGTH];
    mSettings->GetString(L"ID3Priority", ID3Priority, _countof(ID3Priority), L"FrontCover Other");
    mID3CoverTypePriority.SetAll(0xFF);
    BYTE priority = 0;
    LiteStep::IterateOverTokens(ID3Priority, [this, &priority] (LPCWSTR str)
    {
        auto type = ID3TypeFromString(str);
        if (type != TagLib::ID3v2::AttachedPictureFrame::Type::Count && mID3CoverTypePriority[type] == 0xFF)
        {
            mID3CoverTypePriority[type] = ++priority;
        }
    });

    mSettings->GetString(L"DefaultCoverArt", mDefaultCoverArt, _countof(mDefaultCoverArt), L"");
}


/// <summary>
/// Updates the image used.
/// </summary>
void CoverArt::Update()
{
    HANDLE winampHandle;
    ULONG winampProc;
    HWND WA2Window;
    int trackID;
    WCHAR filePath[MAX_PATH];

    // Get Winamps HWND
    if ((WA2Window = FindWindow(L"Winamp v1.x", nullptr)) == nullptr)
    {
        return;
    }

    // Get some basic info
    trackID = (int)SendMessageW(WA2Window, WM_USER, 0, IPC_GETLISTPOS);

    // Open a handle to winamp
    GetWindowThreadProcessId(WA2Window, &winampProc);
    if ((winampHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, winampProc)) == nullptr)
    {
        return;
    }

    // Read the file path
    ReadProcessMemory(winampHandle, (LPCVOID)SendMessageW(WA2Window, WM_USER, trackID, IPC_GETPLAYLISTFILEW), &filePath, sizeof(filePath), nullptr);

    CloseHandle(winampHandle);

    if (!SetCoverFromTag(filePath))
    {
        if (!SetCoverFromFolder(filePath))
        {
            SetDefaultCover();
        }
    }
}


/// <summary>
/// Handles window messages.
/// </summary>
LRESULT WINAPI CoverArt::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LPVOID)
{
    mEventHandler->HandleMessage(hWnd, uMsg, wParam, lParam);
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


void CoverArt::SetSource(IWICBitmapSource *source)
{
    mCoverArt->SetSource(source);
    mWindow->Repaint();
}


/// <summary>
/// Tries to set the cover based on the Tags of the specified file.
/// </summary>
/// <param name="filePath">Path to the file to get the cover from.</param>
bool CoverArt::SetCoverFromTag(LPCWSTR filePath)
{
    LPCWSTR extension = wcsrchr(filePath, L'.');

    if (extension == nullptr)
    {
        return false;
    }

    auto ParseImage = [this] (const BYTE * data, UINT size)
    {
        IWICImagingFactory *factory = nullptr;
        IWICBitmapDecoder *decoder = nullptr;
        IWICBitmapFrameDecode *source = nullptr;
        HRESULT hr = E_FAIL;

        IStream *stream = SHCreateMemStream(data, size);
        if (stream)
        {
            hr = Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));
            if (SUCCEEDED(hr))
            {
                hr = factory->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnDemand, &decoder);
            }
            if (SUCCEEDED(hr))
            {
                hr = decoder->GetFrame(0, &source);
            }
            if (SUCCEEDED(hr))
            {
                SendMessage(gLSModule.GetMessageWindow(), WM_COVERARTUPDATE, (WPARAM)this, (LPARAM)source);
            }

            SAFERELEASE(decoder);
            SAFERELEASE(stream);
        }

        return hr == S_OK;
    };

    ++extension;

    if (_wcsicmp(extension, L"mp3") == 0)
    {
        TagLib::ID3v2::AttachedPictureFrame *pictureFrame = nullptr;
        BYTE picturePriority = 0xFF;

        TagLib::MPEG::File mp3File(filePath);
        auto tag = mp3File.ID3v2Tag();

        if (tag && tag->frameListMap().contains("APIC"))
        {
            for (auto frame : mp3File.ID3v2Tag()->frameListMap()["APIC"])
            {
                auto picFrame = (TagLib::ID3v2::AttachedPictureFrame *)frame;

                BYTE priority = mID3CoverTypePriority[picFrame->type()];
                if (priority < picturePriority)
                {
                    pictureFrame = picFrame;
                    picturePriority = priority;
                }
            }
            if (pictureFrame != nullptr) {
                return ParseImage((const BYTE *)pictureFrame->picture().data(), pictureFrame->picture().size());
            }
        }
    }
    else if (_wcsicmp(extension, L"flac") == 0)
    {
        TagLib::FLAC::File flacFile(filePath);
        for (auto &picture : flacFile.pictureList())
        {
            if (picture->type() == TagLib::FLAC::Picture::FrontCover)
            {
                return ParseImage((const BYTE *)picture->data().data(), picture->data().size());
            }
        }
    }
    else if (_wcsicmp(extension, L"mp4") == 0 || _wcsicmp(extension, L"m4a") == 0)
    {
        TagLib::MP4::File mp4File(filePath);
        if (mp4File.tag()->itemListMap().contains("covr"))
        {
            auto map = mp4File.tag()->itemListMap()["covr"];
            auto list = map.toCoverArtList();
            auto cover = list.front();
            return ParseImage((const BYTE *)cover.data().data(), cover.data().size());
        }
    }

    return false;
}


/// <summary>
/// Tries to get the cover from the specified folder.
/// </summary>
/// <param name="filePath">Path to the file to get the cover from.</param>
bool CoverArt::SetCoverFromFolder(LPCWSTR filePath)
{
    WCHAR folderPath[MAX_PATH] = {0};
    IWICImagingFactory *factory = nullptr;
    IWICBitmapDecoder *decoder = nullptr;
    IWICBitmapFrameDecode *source = nullptr;
    HRESULT hr = E_FAIL;

    StringCchCopyW(folderPath, _countof(folderPath), filePath);
    PathRemoveFileSpecW(folderPath);

    // Check each covername
    WCHAR artPath[MAX_PATH];
    for (auto &canidate : mFolderCanidates)
    {
        StringCchPrintfW(artPath, _countof(artPath), L"%s\\%s", folderPath, canidate.c_str());
        for (auto &file : FileIterator(artPath))
        {
            StringCchPrintfW(artPath, _countof(artPath), L"%s\\%s", folderPath, file.cFileName);

            hr = Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));
            if (SUCCEEDED(hr))
            {
                hr = factory->CreateDecoderFromFilename(artPath, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
            }
            if (SUCCEEDED(hr))
            {
                hr = decoder->GetFrame(0, &source);
            }
            if (SUCCEEDED(hr))
            {
                SendMessage(gLSModule.GetMessageWindow(), WM_COVERARTUPDATE, (WPARAM)this, (LPARAM)source);
            }

            SAFERELEASE(decoder);

            if (SUCCEEDED(hr))
            {
                return true;
            }
        }
    }
    
    return false;
}


/// <summary>
/// Sets the default cover -- when we couldn't find any other cover.
/// </summary>
void CoverArt::SetDefaultCover()
{
    IWICImagingFactory *factory = nullptr;
    IWICBitmapDecoder *decoder = nullptr;
    IWICBitmapFrameDecode *source = nullptr;
    HRESULT hr = E_FAIL;

    hr = Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));
    if (SUCCEEDED(hr))
    {
        hr = factory->CreateDecoderFromFilename(mDefaultCoverArt, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
    }
    if (SUCCEEDED(hr))
    {
        hr = decoder->GetFrame(0, &source);
    }
    if (SUCCEEDED(hr))
    {
        SendMessage(gLSModule.GetMessageWindow(), WM_COVERARTUPDATE, (WPARAM)this, (LPARAM)source);
    }
    else
    {
        SendMessage(gLSModule.GetMessageWindow(), WM_COVERARTUPDATE, (WPARAM)this, (LPARAM)nullptr);
    }

    SAFERELEASE(decoder);
}
