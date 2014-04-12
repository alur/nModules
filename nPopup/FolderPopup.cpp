/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  FolderPopup.hpp
 *  The nModules Project
 *
 *  A popup which contains the contents a *Popup Folder, or *Popup !New
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "FolderPopup.hpp"


FolderPopup::FolderPopup(LPCTSTR title, LPCTSTR bang, LPCTSTR prefix)
    : Popup(title, bang, prefix)
{
}


FolderPopup::~FolderPopup()
{
}


void FolderPopup::PreShow()
{
}


void FolderPopup::PostClose()
{
}
