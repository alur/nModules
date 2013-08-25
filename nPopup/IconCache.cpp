/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IconCache.cpp
 *  The nModules Project
 *
 *  Provides caching of HICONs.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "IconCache.hpp"
#include "../Utilities/CRC32.h"


UINT32 IconCache::ComputeHash(LPWSTR path, int id)
{
    struct IconData
    {
        WCHAR path[256];
        int id;
    };
    IconData data;
    ZeroMemory(&data, sizeof(IconData));
    StringCchCopy(data.path, _countof(data.path), path);
    data.id = id;

    return crc32(&data, sizeof(data));
}


HICON IconCache::GetIcon(UINT32 iconHash)
{
    CacheMap::iterator iter = mIconCache.find(iconHash);
    if (iter != mIconCache.end())
    {
        iter->second.refCount++;
        return iter->second.icon;
    }
    return nullptr;
}


void IconCache::StoreIcon(UINT32 iconHash, HICON icon)
{
    mIconCache[iconHash] = CacheItem(icon);
}


void IconCache::ReleaseIcon(UINT32 iconHash)
{
    CacheMap::iterator iter = mIconCache.find(iconHash);
    if (iter != mIconCache.end())
    {
        if (--iter->second.refCount == 0)
        {
            DestroyIcon(iter->second.icon);
            mIconCache.erase(iter);
        }
    }
}
