/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IconCache.cpp
 *  The nModules Project
 *
 *  Provides caching of HICONs.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "IconCache.hpp"

#include "../Utilities/Hashing.h"

#include <strsafe.h>

/// <summary>
/// Computes the hash of a particular icon.
/// </summary>
IconCache::Hash IconCache::ComputeHash(LPWSTR path, int id)
{
    struct
    {
        WCHAR path[256];
        int id;
    } data;
    ZeroMemory(&data, sizeof(data));
    StringCchCopy(data.path, _countof(data.path), path);
    data.id = id;

    return Hashing::Crc32(&data, sizeof(data));
}


/// <summary>
/// Retrieves the icon with a particular hash.
/// </summary>
/// <remarks>
/// If this function returns an icon, a corresponding call to ReleaseIcon must be made.
/// </remarks>
/// <returns>The icon, or nullptr if the icon is not found.</returns>
HICON IconCache::GetIcon(Hash iconHash)
{
    CacheMap::iterator iter = mIconCache.find(iconHash);
    if (iter != mIconCache.end())
    {
        iter->second.refCount++;
        return iter->second.icon;
    }
    return nullptr;
}


/// <summary>
/// Stores the specified icon in the cache.
/// </summary>
void IconCache::StoreIcon(Hash iconHash, HICON icon)
{
    mIconCache[iconHash] = CacheItem(icon);
}


/// <summary>
/// Releases a reference to the icon with the specified hash.
/// </summary>
void IconCache::ReleaseIcon(Hash iconHash)
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
