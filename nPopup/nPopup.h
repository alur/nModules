/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nPopup.h
 *  The nModules Project
 *
 *  General declarations for nPopup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

void LoadSettings();
void LoadPopups();
bool LoadPopup(LPVOID f, bool * success, Popup** popup);
Popup* LoadFolder(LPVOID f, LPCSTR title, LPCSTR prefix);
