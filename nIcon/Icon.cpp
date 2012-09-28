/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Icon.hpp                                                   September, 2012
 *  The nModules Project
 *
 *  A single icon in an IconGroup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "Icon.hpp"


Icon::Icon(PITEMID_CHILD item, IShellFolder2* shellFolder) {
    this->shellFolder = shellFolder;
    this->item = (PITEMID_CHILD)malloc(item->mkid.cb + 2);
    memcpy(this->item, item, item->mkid.cb + 2);
}


Icon::~Icon() {
    free(this->item);
}


LRESULT WINAPI Icon::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    return 0;
}
