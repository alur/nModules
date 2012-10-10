/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  FolderPopup.hpp
 *  The nModules Project
 *
 *  A popup which contains the contents a *Popup Folder, or *Popup !New
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "FolderPopup.hpp"
#include "../nShared/Debugging.h"


FolderPopup::FolderPopup(LPCSTR title, LPCSTR bang, LPCSTR prefix) : Popup(title, bang, prefix) {

}


FolderPopup::~FolderPopup() {

}


void FolderPopup::PreShow() {
}


void FolderPopup::PostClose() {
}
