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
bool AddHotkey(UINT mods, UINT key, LPCSTR pszCommand);
UINT ParseMods(LPCSTR szMods);
UINT ParseKey(LPCSTR szKey);
