
#include "../nShared/LiteStep.h"
#include "../nShared/Macros.h"
#include "ParsedText.hpp"
#include <regex>
#include <strsafe.h>
#include "../nShared/Debugging.h"


// All existing functions.
map<wstring, FORMATTINGPROC> functionMap;


EXPORT_CDECL(BOOL) RegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs, FORMATTINGPROC formatter) {
    functionMap.insert(std::pair<wstring, FORMATTINGPROC>(wstring(name), formatter)).first;
    return FALSE;
}


EXPORT_CDECL(BOOL) UnRegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs) {
    RegisterDynamicTextFunction(name, numArgs, NULL);
    return FALSE;
}


map<wstring, FORMATTINGPROC>::const_iterator FindDynamicTextFunction(LPCWSTR name, UCHAR numArgs) {
    map<wstring, FORMATTINGPROC>::const_iterator ret = functionMap.find(name);
    if (ret == functionMap.end()) {
        return functionMap.insert(std::pair<wstring, FORMATTINGPROC>(wstring(name), NULL)).first;
    }
    return ret;
}


ParsedText::ParsedText(LPCWSTR text) {
    Parse(text);
}


bool ParsedText::IsDynamic() {
    return true;
}


ParsedText::~ParsedText() {
    for (list<Token>::const_iterator token = this->tokens.begin(); token != this->tokens.end(); ++token) {
        free((LPVOID)token->text);
    }
    this->tokens.clear();
}


bool ParsedText::Evaluate(LPWSTR dest, size_t cchDest) {
    dest[0] = '\0';

    for (list<Token>::const_iterator token = this->tokens.begin(); token != this->tokens.end(); ++token) {
        switch (token->type) {
        case TEXT:
            StringCchCatW(dest, cchDest, token->text);
            break;

        case EXPRESSION:
            if (token->proc->second != NULL) {
                token->proc->second(L"", 0, dest, cchDest); 
            }
            else {
                StringCchCatW(dest, cchDest, token->text);
            }
            break;
        }
    }

    return true;
}


void ParsedText::AddToken(TokenType type, map<wstring, FORMATTINGPROC>::const_iterator proc, LPCWSTR str) {
    Token t;
    t.type = type;
    t.proc = proc;
    t.text = str;
    tokens.push_back(t);
}


void ParsedText::Parse(LPCWSTR text) {
    // An expression starts with a [, and ends with the first ] which is not enclosed within quotes.

    LPCWSTR expBegin = text, expEnd = text, pos = text;
    while ((expBegin = wcswcs(pos, L"[")) != NULL && (expEnd = wcswcs(expBegin, L"]")) != NULL) {
        size_t textLength = expBegin - pos;
        LPWSTR text = (LPWSTR)malloc((textLength+1)*sizeof(WCHAR));
        memcpy(text, pos, textLength*sizeof(WCHAR));
        text[textLength] = NULL;
        AddToken(TEXT, functionMap.end(), text);

        size_t expLength = expEnd - expBegin - 1;
        LPWSTR exp = (LPWSTR)malloc((expLength+1)*sizeof(WCHAR));
        memcpy(exp, expBegin + 1, expLength*sizeof(WCHAR));
        exp[expLength] = NULL;
        AddToken(EXPRESSION, FindDynamicTextFunction(exp, 0), exp);

        pos = expEnd+1;
    }
    AddToken(TEXT, functionMap.end(), _wcsdup(pos));
}
