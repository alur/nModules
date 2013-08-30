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
bool AddHotkey(UINT mods, UINT key, LPCTSTR command);
UINT ParseMods(LPCTSTR mods);
UINT ParseKey(LPCTSTR key);
