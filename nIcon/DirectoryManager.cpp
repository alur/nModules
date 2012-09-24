/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DirectoryWatcher.cpp                                       September, 2012
 *  The nModules Project
 *
 *  Watches a single folder for modifications.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "DirectoryManager.hpp"
#include <process.h>


UINT __stdcall WorkerThread(DirectoryManager* pDataSource);
void __stdcall FileIOCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);


/// <summary>
/// Creates a new DirectoryManager.
/// </summary>
DirectoryManager::DirectoryManager(LPCWSTR pszPath, HWND hCallback, MessageMap messageMap) {
    // Initalize variables
    m_bWorkerRunning = false;
    m_hCallback = hCallback;
    m_hKillWorkerEvent = CreateEvent(NULL, false, false, NULL);
    m_MessageMap = messageMap;
    StringCchCopyW(m_szPath, sizeof(m_szPath), pszPath);
    m_uWorkerID = NULL;

    // Start the worker.
    _beginthreadex(NULL, 0, (UINT (__stdcall *)(void *))WorkerThread, this, 0, &m_uWorkerID);
}


/// <summary>
/// Destroys this DirectoryManager.
/// </summary>
DirectoryManager::~DirectoryManager() {
    KillWorker();
    CloseHandle(m_hKillWorkerEvent);
}


/// <summary>
/// Causes the worker thread to terminate.
/// </summary>
void DirectoryManager::KillWorker() {
    if (m_bWorkerRunning) {
        SetEvent(m_hKillWorkerEvent);
        while (m_bWorkerRunning) {
            Sleep(5);
        }
    }
}


/// <summary>
/// Monitors the specified folder and notifies m_hCallback.
/// </summary>
UINT __stdcall WorkerThread(DirectoryManager* pDataSource) {
    // Mark that we are running.
    pDataSource->m_bWorkerRunning = true;

    // Open up an asynchronous handle to the folder
    HANDLE hDirectory = CreateFileW(
        pDataSource->m_szPath,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL);
    
    // Holds the FILE_NOTIFY_INFORMATION.
    LPBYTE pBuffer = (LPBYTE)malloc(MAX_LINE_LENGTH);

    //
    OVERLAPPED oLapped = {0};
    oLapped.hEvent = CreateEvent(NULL, false, false , NULL);

    // Events to wait for
    HANDLE hEvents[] = {oLapped.hEvent, pDataSource->m_hKillWorkerEvent};

    // Listen for directory changes.
    bool bEndLoop = false;
	do {

        ReadDirectoryChangesW(
            hDirectory,
            pBuffer,
            MAX_LINE_LENGTH,
            false,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES,
            NULL,
            &oLapped,
            FileIOCompletionRoutine);

        switch (WaitForMultipleObjectsEx(2, hEvents, false, INFINITE, true)) {

        // Triggered by ReadDirectoryChangesW completing.
        case WAIT_OBJECT_0:
            {
                // Used for the rename notification.
				PFILE_NOTIFY_INFORMATION pChangeInfo = (PFILE_NOTIFY_INFORMATION)pBuffer;
                LPWSTR pFileName = NULL, pNewName = NULL, pOldName = NULL;
                DWORD dwOffset = 0;

                // Go through the 
				do {
                    // Lots of work for putting a terminating NULL at the end of the filename.
                    pFileName = (LPWSTR)realloc(pFileName, pChangeInfo->FileNameLength + 2); // Terminating NULL.
                    memcpy(pFileName, pChangeInfo->FileName, pChangeInfo->FileNameLength);
                    pFileName[pChangeInfo->FileNameLength/sizeof(WCHAR)] = 0;

					switch (pChangeInfo->Action) {
					case FILE_ACTION_ADDED:
						{
                            SendMessage(pDataSource->m_hCallback, pDataSource->m_MessageMap.Added, (WPARAM)pFileName, NULL);
						}
                        break;

					case FILE_ACTION_REMOVED:
						{
                            SendMessage(pDataSource->m_hCallback, pDataSource->m_MessageMap.Delted, (WPARAM)pFileName, NULL);
						}
                        break;

					case FILE_ACTION_MODIFIED:
						{
                            SendMessage(pDataSource->m_hCallback, pDataSource->m_MessageMap.Modified, (WPARAM)pFileName, NULL);
						}
                        break;

					case FILE_ACTION_RENAMED_OLD_NAME:
						{
							if (pNewName == NULL) {
                                if (pOldName != NULL) {
                                    free(pOldName);
                                }
								pOldName = _wcsdup(pFileName);
							}
							else {
                                SendMessage(pDataSource->m_hCallback, pDataSource->m_MessageMap.Renamed, (WPARAM)pNewName, (LPARAM)pFileName);
                                free(pNewName);
                                pNewName = NULL;
							}
                        }
                        break;

					case FILE_ACTION_RENAMED_NEW_NAME:
						{
							if (pOldName == NULL) {
                                if (pNewName != NULL) {
                                    free(pNewName);
                                }
								pNewName = _wcsdup(pFileName);
							}
							else {
                                SendMessage(pDataSource->m_hCallback, pDataSource->m_MessageMap.Renamed, (WPARAM)pFileName, (LPARAM)pOldName);
                                free(pOldName);
                                pOldName = NULL;
							}
                        }
                        break;
					}

                    if (pChangeInfo->NextEntryOffset == 0)
                        break;

                    dwOffset += pChangeInfo->NextEntryOffset;

                    pChangeInfo = (PFILE_NOTIFY_INFORMATION)(pBuffer+dwOffset);
                } while (dwOffset < MAX_LINE_LENGTH);

                free(pFileName);
                free(pOldName);
                free(pNewName);
            }
            break;

        // Triggered when we should end the thread.
        case WAIT_OBJECT_0+1:
            {
                bEndLoop = true;
            }
            break;

        default:
            break;

        } // WaitForMultiple

    } while(!bEndLoop);
    
    // Free the buffer
    free(pBuffer);

    // Close the directory handle.
    CloseHandle(hDirectory);
    CloseHandle(oLapped.hEvent);

    // Mark that we are no longer running.
    pDataSource->m_bWorkerRunning = false;

    return 0;
}


void __stdcall FileIOCompletionRoutine(DWORD, DWORD, LPOVERLAPPED lpOverlapped) {
    SetEvent(lpOverlapped->hEvent);
}
