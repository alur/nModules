/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  FolderPopup.hpp
 *  The nModules Project
 *
 *  A popup which contains the contents a *Popup Folder, or *Popup !New
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Popup.hpp"

class FolderPopup : public Popup {
public:
    explicit FolderPopup(LPCSTR title, LPCSTR bang, LPCSTR prefix);
    virtual ~FolderPopup();

protected:
    void PreShow();
    void PostClose();
};
