/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TextFunctions.cpp
 *  The nModules Project
 *
 *  Provides Dynamic Text Functions.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "TextFunctions.h"
#include "../nCoreCom/Core.h"
#include <strsafe.h>

#define TEXTFUNCTION(x) size_t __cdecl x(LPCWSTR /* name */, UCHAR /* numArgs */, LPWSTR* /* args */, LPWSTR dest, size_t cchDest)
#define IPC_GETLISTPOS 125
#define IPC_GETPLAYLISTFILEW 214
#define IPC_GET_EXTENDED_FILE_INFOW_HOOKABLE 3027

namespace TextFunctions
{
    TEXTFUNCTION(MusicTrackTitle);
    TEXTFUNCTION(MusicTrackArtist);
    TEXTFUNCTION(MusicAlbumTitle);

    WCHAR albumTitle[4096] = L"";
    WCHAR albumArtist[4096] = L"";
    WCHAR trackTitle[4096] = L"";
    WCHAR trackArtist[4096] = L"";
    WCHAR filePath[MAX_PATH] = L"";
}


void TextFunctions::_Register()
{
    using nCore::System::RegisterDynamicTextFunction;

    RegisterDynamicTextFunction(L"MusicTrackTitle", 0, MusicTrackTitle, true);
    RegisterDynamicTextFunction(L"MusicTrackArtist", 0, MusicTrackArtist, true);
    RegisterDynamicTextFunction(L"MusicAlbumTitle", 0, MusicAlbumTitle, true);
}


void TextFunctions::_UnRegister()
{
    using nCore::System::UnRegisterDynamicTextFunction;

    UnRegisterDynamicTextFunction(L"MusicTrackTitle", 0);
    UnRegisterDynamicTextFunction(L"MusicTrackArtist", 0);
    UnRegisterDynamicTextFunction(L"MusicAlbumTitle", 0);
}


void TextFunctions::_Update()
{
    HANDLE winampHandle;
    ULONG winampProc;
    HWND WA2Window;
    int trackID;

    // Winamp expects a 32bit struct so we can't use the ones in WA_IPC.
    struct
    {
        DWORD filename; // LPCWSTR
        DWORD metadata; // LPCWSTR
        DWORD ret;      // LPWSTR
        UINT retlen;
    } fileInfo;

    // Get Winamps HWND
    if ((WA2Window = FindWindowW(L"Winamp v1.x", nullptr)) == nullptr)
    {
        StringCchCopyW(trackTitle, sizeof(trackTitle), L"");
        return;
    }

    // Get some basic info
    trackID = (int)SendMessageW(WA2Window, WM_USER, 0, IPC_GETLISTPOS);
    //TRACE("trackID: %d", trackID);

    // Open a handle to winamp
    GetWindowThreadProcessId(WA2Window, &winampProc);
    if ((winampHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, winampProc)) == nullptr)
    {
        StringCchCopyW(trackTitle, sizeof(trackTitle), L"");
        return;
    }

    // Allocate memory in Winamp's process
    LPVOID remoteStruct = VirtualAllocEx(winampHandle, nullptr, sizeof(fileInfo), MEM_COMMIT, PAGE_READWRITE);

    // Configure the remote extendedFileInfoStructW
    fileInfo.filename = (DWORD)SendMessageW(WA2Window, WM_USER, trackID, IPC_GETPLAYLISTFILEW);
    fileInfo.metadata = (DWORD)VirtualAllocEx(winampHandle, nullptr, 64, MEM_COMMIT, PAGE_READWRITE);
    fileInfo.ret = (DWORD)VirtualAllocEx(winampHandle, nullptr, 4096*sizeof(WCHAR), MEM_COMMIT, PAGE_READWRITE);;
    fileInfo.retlen = 4096;
    WriteProcessMemory(winampHandle, remoteStruct, &fileInfo, sizeof(fileInfo), nullptr);

    // Read the file path
    WCHAR newFilePath[MAX_PATH];
    ReadProcessMemory(winampHandle, (LPCVOID)fileInfo.filename, &newFilePath, sizeof(newFilePath), nullptr);
    if (wcscmp(newFilePath, filePath) == 0) {
        return;
    }
    memcpy(filePath, newFilePath, sizeof(filePath));
    //TRACEW(L"filePath: %s", filePath);

    // Read the track title
    WriteProcessMemory(winampHandle, (LPVOID)fileInfo.metadata, L"Title", 64, nullptr);
    if (SendMessageW(WA2Window, WM_USER, (WPARAM)remoteStruct, IPC_GET_EXTENDED_FILE_INFOW_HOOKABLE))
    {
        ReadProcessMemory(winampHandle, (LPCVOID)fileInfo.ret, &trackTitle, sizeof(trackTitle), nullptr);
    }
    else
    {
        trackTitle[0] = '\0';
    }
    //TRACEW(L"trackTitle: %s", trackTitle);

    // Read the track artist
    WriteProcessMemory(winampHandle, (LPVOID)fileInfo.metadata, L"Artist", 64, nullptr);
    if (SendMessageW(WA2Window, WM_USER, (WPARAM)remoteStruct, IPC_GET_EXTENDED_FILE_INFOW_HOOKABLE))
    {
        ReadProcessMemory(winampHandle, (LPCVOID)fileInfo.ret, &trackArtist, sizeof(trackArtist), nullptr);
    }
    else 
    {
        trackArtist[0] = '\0';
    }

    // Read the album title
    WriteProcessMemory(winampHandle, (LPVOID)fileInfo.metadata, L"Album", 64, nullptr);
    if (SendMessageW(WA2Window, WM_USER, (WPARAM)remoteStruct, IPC_GET_EXTENDED_FILE_INFOW_HOOKABLE))
    {
        ReadProcessMemory(winampHandle, (LPCVOID)fileInfo.ret, &albumTitle, sizeof(albumTitle), nullptr);
    }
    else
    {
        albumTitle[0] = '\0';
    }

    // Deallocate memory from Winamp's process and close the handle.
    VirtualFreeEx(winampHandle, (LPVOID)fileInfo.metadata, 64, MEM_DECOMMIT);
    VirtualFreeEx(winampHandle, (LPVOID)fileInfo.metadata, 0, MEM_RELEASE);
    VirtualFreeEx(winampHandle, remoteStruct, sizeof(fileInfo), MEM_DECOMMIT);
    VirtualFreeEx(winampHandle, remoteStruct, 0, MEM_RELEASE);
    VirtualFreeEx(winampHandle, (LPVOID)fileInfo.ret, 4096*sizeof(WCHAR), MEM_DECOMMIT);
    VirtualFreeEx(winampHandle, (LPVOID)fileInfo.ret, 0, MEM_RELEASE);

    CloseHandle(winampHandle);
}


void TextFunctions::_UpdateNotify() {
  nCore::System::DynamicTextChangeNotification(L"MusicTrackTitle", 0);
  nCore::System::DynamicTextChangeNotification(L"MusicTrackArtist", 0);
  nCore::System::DynamicTextChangeNotification(L"MusicAlbumTitle", 0);
}


TEXTFUNCTION(TextFunctions::MusicTrackTitle)
{
    size_t rem;
    StringCchCopyExW(dest, cchDest, trackTitle, nullptr, &rem, 0);
    return cchDest - rem;
}


TEXTFUNCTION(TextFunctions::MusicTrackArtist)
{
    size_t rem;
    StringCchCopyExW(dest, cchDest, trackArtist, nullptr, &rem, 0);
    return cchDest - rem;
}


TEXTFUNCTION(TextFunctions::MusicAlbumTitle)
{
    size_t rem;
    StringCchCopyExW(dest, cchDest, albumTitle, nullptr, &rem, 0);
    return cchDest - rem;
}
