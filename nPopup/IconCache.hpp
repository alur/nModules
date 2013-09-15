/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IconCache.hpp
 *  The nModules Project
 *
 *  Provides caching of HICONs.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../Utilities/Common.h"
#include <unordered_map>

class IconCache
{
public:
    typedef UINT32 Hash;

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

    typedef std::unordered_map<Hash, CacheItem> CacheMap;

private:
    CacheMap mIconCache;

public:
    Hash ComputeHash(LPWSTR path, int id);

    HICON GetIcon(Hash iconHash);
    void StoreIcon(Hash iconHash, HICON icon);
    void ReleaseIcon(Hash iconHash);
};
