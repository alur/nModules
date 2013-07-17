/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LiteStep.cpp
 *  The nModules Project
 *
 *  Wraps LS APIs with unicode versions, and confines them to a namespace.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "LiteStep.h"


/// <summary>
/// Iterates over all lines with the specified key name.
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


/// <summary>
/// Iterates over all lines with the specified key name.
/// </summary>
void LiteStep::IterateOverLinesW(LPCSTR keyName, std::function<void(LPCWSTR line)> callback) {
    char line[MAX_LINE_LENGTH];
    WCHAR wLine[MAX_LINE_LENGTH];
    LPCWSTR callbackLine = wLine + strlen(keyName) + 1;
    LPVOID f = LiteStep::LCOpen(nullptr);
    while (LiteStep::LCReadNextConfig(f, keyName, line, _countof(line))) {
        MultiByteToWideChar(CP_ACP, 0, line, sizeof(line), wLine, _countof(wLine));
        callback(callbackLine);
    }
    LiteStep::LCClose(f);
}


/// <summary>
/// Iterates over all tokens in the specified line
/// </summary>
void LiteStep::IterateOverTokens(LPCSTR str, std::function<void (LPCSTR token)> callback) {
    LPCSTR next = str;
    CHAR token[MAX_LINE_LENGTH];
    while (GetToken(next, token, &next, false)) {
        callback(token);
    }
}


/// <summary>
/// Iterates over all tokens in the specified line
/// </summary>
void LiteStep::IterateOverTokens(LPCWSTR str, std::function<void (LPCWSTR token)> callback) {
    LPCWSTR next = str;
    WCHAR token[MAX_LINE_LENGTH];
    while (GetTokenW(next, token, &next, false)) {
        callback(token);
    }
}
