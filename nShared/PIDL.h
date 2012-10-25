/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PIDL.h
 *  The nModules Project
 *
 *  Functions for dealing with PIDLs.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <shtypes.h>

namespace PIDL {
    // Gets the last item in the PIDL
    PCITEMID_CHILD GetLastPIDLItem(LPITEMIDLIST pidl);

    // Checks if two idlists are equivalent
    bool Equivalent(LPITEMIDLIST pidl1, LPITEMIDLIST pidl2);
}
