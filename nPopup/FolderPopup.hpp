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
    explicit FolderPopup(LPCTSTR title, LPCTSTR bang, LPCTSTR prefix);
    virtual ~FolderPopup();

protected:
    void PreShow() override;
    void PostClose() override;
};
