/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ContentPopup.hpp
 *  The nModules Project
 *
 *  A popup which retrives its content from outside LiteStep's .rc files.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Popup.hpp"
#include "FolderItem.hpp"
#include <ShlObj.h>

class ContentPopup : public Popup {
public:
    enum ContentSource {
        PATH,
        ADMIN_TOOLS,
        CONTROL_PANEL,
        MY_COMPUTER,
        NETWORK,
        PRINTERS,
        RECENT_DOCUMENTS,
        RECYCLE_BIN,
        START_MENU,
        PROGRAMS
    };

    explicit ContentPopup(ContentSource source, LPCTSTR title, LPCTSTR bang, LPCTSTR prefix);
    explicit ContentPopup(LPCTSTR path, bool dynamic, LPCTSTR title, LPCTSTR bang, LPCTSTR prefix);
    virtual ~ContentPopup();

    //
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

    //
    void AddPath(LPCTSTR path);

protected:
    void PreShow() override;
    virtual void PostClose() override;

private:

    //
    void LoadContent();

    //
    void LoadShellFolder(GUID folder, bool dontExpandFolders = false);

    //
    void LoadPath(LPCTSTR path);

    //
    void LoadFromIDList(IShellFolder *targetFolder, PIDLIST_ABSOLUTE idList, bool dontExpandFolders);

    //
    void LoadSingleItem(IShellFolder *targetFolder, PIDLIST_RELATIVE itemID, bool dontExpandFolders);

    // True if the content needs to be reloaded every time the popup is shown.
    bool dynamic;

    // True if the content has been loaded.
    bool loaded;

    // If the source is a folder, the path to it.
    std::list<LPCTSTR> paths;

    // What to retrive the popup contents from.
    ContentSource source;

    // Message, SHChangeNofityRegister return value, IShellFolder
    typedef std::map<UINT, std::pair<ULONG, IShellFolder *>> WATCHFOLDERMAP;

    // Folders which this popup is watching for changes.
    WATCHFOLDERMAP watchedFolders;
};
