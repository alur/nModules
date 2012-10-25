/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PIDL.cpp
 *  The nModules Project
 *
 *  Functions for dealing with PIDLs.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "PIDL.h"


PCITEMID_CHILD PIDL::GetLastPIDLItem(LPITEMIDLIST pidl) {
    LPITEMIDLIST ret = pidl;
    USHORT lastCB = 0;
    while (ret->mkid.cb != 0) {
        lastCB = ret->mkid.cb;
        ret = LPITEMIDLIST(((LPBYTE)ret)+lastCB);
    }
    return LPITEMIDLIST(((LPBYTE)ret)-lastCB);
}


bool PIDL::Equivalent(LPITEMIDLIST pidl1, LPITEMIDLIST pidl2) {
    do {
        if (pidl1->mkid.cb != pidl2->mkid.cb || memcmp(pidl1, pidl2, pidl1->mkid.cb) != 0) {
            return false;
        }
    } while (pidl1->mkid.cb != 0);

    return true;
}
