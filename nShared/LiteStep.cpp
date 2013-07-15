/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LiteStep.cpp
 *  The nModules Project
 *
 *  Wraps LS APIs with unicode versions, and confines them to a namespace.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "LiteStep.h"


/// <summary>
/// 
/// </summary>
void LiteStep::IterateOverLines(LPCSTR keyName, std::function<void(LPCSTR line)> callback) {
    char line[MAX_LINE_LENGTH];
    LPCSTR callbackLine = line + strlen(keyName) + 1;
    LPVOID f = LiteStep::LCOpen(nullptr);
    while (LiteStep::LCReadNextConfig(f, keyName, line, _countof(line))) {
        callback(callbackLine);
    }
    LiteStep::LCClose(f);
}


void LiteStep::IterateOverTokens(LPCWSTR str, std::function<void (LPCWSTR token)> callback) {
    LPCWSTR next = str;
    WCHAR token[MAX_LINE_LENGTH];
    while (GetTokenW(next, token, &next, false)) {
        callback(token);
    }
}
