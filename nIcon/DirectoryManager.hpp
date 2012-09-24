/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DirectoryManager.hpp                                       September, 2012
 *  The nModules Project
 *
 *  Manages a single directory.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef DIRECTORYMANAGER_HPP
#define DIRECTORYMANAGER_HPP

class DirectoryManager {
public:
    typedef struct {
        int Enum;
        int Added;
        int Delted;
        int Renamed;
        int Modified;
    } MessageMap;

    explicit DirectoryManager(LPCWSTR pszPath, HWND hCallback, MessageMap messageMap);
    virtual ~DirectoryManager();

    void SendFileList();

    // The window which should recieve notification messages.
    HWND m_hCallback;

    // The IDs of the various messages we send.
    MessageMap m_MessageMap;

    // Event for terminating the worker.
    HANDLE m_hKillWorkerEvent;

    // Managed by the worker.
    bool m_bWorkerRunning;

    // The path to the folder we are watching.
    WCHAR m_szPath[MAX_PATH];
    
private:
    void KillWorker();

    // The ID of the worker thread.
    UINT m_uWorkerID;
};

#endif /* DIRECTORYMANAGER_HPP */
