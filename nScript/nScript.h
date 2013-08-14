/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nScript.h
 *  The nModules Project
 *
 *  Functions declarations for nScript.cpp.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

void LoadSettings();

void InitV8();
void CleanupV8();

void RunCode(LPCSTR code, void (*callback)(v8::Handle<v8::Value>));
void BangExec(HWND, LPCSTR code);
void BangAlert(HWND, LPCSTR code);
