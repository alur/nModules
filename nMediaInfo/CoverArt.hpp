/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  CoverArt.hpp
*  The nModules Project
*
*  Renders cover art.
*   
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Drawable.hpp"
#include <list>

using std::wstring;
using std::list;

class CoverArt : public Drawable {
public:
    explicit CoverArt(LPCSTR name);
    virtual ~CoverArt();

public:
    void Update();

    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID) override;

private:
    DrawableWindow::OVERLAY coverArt;

    bool SetCoverFromTag(LPCWSTR filePath);
    bool SetCoverFromFolder(LPCWSTR filePath);
    void SetDefaultCover();

private:
    // Path to the default cover art.
    WCHAR defaultCoverArt[MAX_PATH];

    // The names to search for when looking in folders. May include wildcards.
    list<wstring> folderCanidates;
};
