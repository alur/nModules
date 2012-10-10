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

    explicit ContentPopup(ContentSource source, LPCSTR title, LPCSTR bang, LPCSTR prefix);
    explicit ContentPopup(LPCSTR path, bool dynamic, LPCSTR title, LPCSTR bang, LPCSTR prefix);
    virtual ~ContentPopup();

protected:
    void PreShow();
    void PostClose();

private:
    //
    void LoadContent();

    //
    void LoadShellFolder(GUID folder);

    //
    void LoadPath(LPCSTR path);

    //
    void LoadFromIDList(IShellFolder *targetFolder, PIDLIST_ABSOLUTE idList);

    // True if the content needs to be reloaded every time the popup is shown.
    bool dynamic;

    // True if the content has been loaded.
    bool loaded;

    // If the source is a folder, the path to it.
    list<LPCSTR> paths;

    // What to retrive the popup contents from.
    ContentSource source;
};
