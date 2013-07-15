/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  CoverArt.hpp
*  The nModules Project
*
*  Renders cover art.
*   
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Drawable.hpp"
#define TAGLIB_STATIC
#include "../External/taglib/mpeg/id3v2/frames/attachedpictureframe.h"
#include <list>
#include "../Utilities/EnumArray.hpp"

using std::wstring;
using std::list;

class CoverArt : public Drawable {
public:
    explicit CoverArt(LPCSTR name);
    virtual ~CoverArt();

private:
    static TagLib::ID3v2::AttachedPictureFrame::Type ID3TypeFromString(LPCWSTR str);

public:
    void Update();

    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID) override;

private:
    bool SetCoverFromTag(LPCWSTR filePath);
    bool SetCoverFromFolder(LPCWSTR filePath);
    void SetDefaultCover();

private:
    void LoadSettings();

private:
    //
    DrawableWindow::OVERLAY mCoverArt;

    // Path to the default cover art.
    WCHAR mDefaultCoverArt[MAX_PATH];

    // The names to search for when looking in folders. May include wildcards.
    list<wstring> mFolderCanidates;

    //
    EnumArray<BYTE, TagLib::ID3v2::AttachedPictureFrame::Type> mID3CoverTypePriority;
};
