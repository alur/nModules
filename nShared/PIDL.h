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
}
