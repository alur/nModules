/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nMediaInfo.h
 *  The nModules Project
 *
 *  Functions declarations for nMediaInfo.cpp.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

static void LoadSettings();
static void Update();
static void CreateCoverart(LPCTSTR name);


enum WindowMessages
{
    WM_TEXTUPDATENOTIFY = WM_USER,
    WM_COVERARTUPDATE = WM_USER + 1
};
