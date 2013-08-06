/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nKey.h
 *  The nModules Project
 *
 *  Functions declarations for nKey.cpp.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

void LoadSettings();
void LoadHotKeys();
void LoadVKeyTable();
bool AddHotkey(UINT mods, UINT key, LPCTSTR pszCommand);
UINT ParseMods(LPCTSTR szMods);
UINT ParseKey(LPCTSTR szKey);
