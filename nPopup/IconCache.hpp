/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IconCache.hpp
 *  The nModules Project
 *
 *  Provides caching of HICONs.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../Utilities/Common.h"
#include <map>

class IconCache
{
private:
    struct CacheItem
    {
        CacheItem()
        {
            refCount = 0;
            this->icon = nullptr;
        }
        CacheItem(HICON icon)
        {
            refCount = 1;
            this->icon = icon;
        }

        UINT refCount;
        HICON icon;
    };

    typedef std::map<UINT32, CacheItem> CacheMap;

private:
    CacheMap mIconCache;

public:
    UINT32 ComputeHash(LPWSTR path, int id);

    HICON GetIcon(UINT32 iconHash);
    void StoreIcon(UINT32 iconHash, HICON icon);
    void ReleaseIcon(UINT32 iconHash);
};
