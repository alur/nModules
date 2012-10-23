
#include "../nShared/LiteStep.h"
#include "../nShared/Macros.h"
#include "ParsedText.hpp"
#include <regex>
#include <strsafe.h>
#include "../nShared/Debugging.h"


// All existing functions.
map<wstring, FORMATTINGPROC> functionMap;


EXPORT_CDECL(BOOL) RegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs, FORMATTINGPROC formatter, bool dynamic) {
    functionMap[wstring(name)] = formatter;
    return FALSE;
}


EXPORT_CDECL(BOOL) UnRegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs) {
    RegisterDynamicTextFunction(name, numArgs, NULL, true);
    return FALSE;
}


/// <summary>
/// Parses text into
/// </summary>
EXPORT_CDECL(IParsedText*) ParseText(LPCWSTR text) {
    return new ParsedText(text);
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


ParsedText::~ParsedText() {
    for (list<Token>::const_iterator token = this->tokens.begin(); token != this->tokens.end(); ++token) {
        free((LPVOID)token->text);
    }
    this->tokens.clear();
}


bool ParsedText::IsDynamic() {
    return true;
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
                StringCchCatW(dest, cchDest, L"[");
                StringCchCatW(dest, cchDest, token->text);
                StringCchCatW(dest, cchDest, L"]");
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

    /* WCHAR segment[MAX_LINE_LENGTH];

    LPCWSTR searchPos = text;
    LPCWSTR searchEnd = text + wcslen(text);
    LPCWSTR segmentStart = text;

    // What we are currently searching for
    // 0 -> [
    // 1 -> ( or ] --- reverting to 0 on non-alphanumeric
    // 2 -> '
    // 3 -> )
    UINT searchMode = 0;

    LPCWSTR nameStart = NULL;
    LPCWSTR nameEnd = NULL;
    LPCWSTR paramStart = NULL;
    LPCWSTR paramEnd = NULL;

    while (searchPos <= searchEnd) {
        switch (searchMode) {
        case 0:
            if (*searchPos == '[' && (searchPos == text || *(searchPos-1) != '\\')) { // TOOD::Not entierly sure on order of execution.
                nameStart = searchPos;
                searchMode = 1;
            }
            break;

        case 1:
            if (*searchPos == ']') {
                // TODO::Validate and stuff
            }
            else if (*searchPos == '(') {
                nameEnd = searchPos-1;
                if (searchPos < searchEnd && *(searchPos+1) == '\'') {
                    searchMode = 2;
                }
                else {
                    searchMode = 3;
                }
            }
            else if (!(*searchPos >= 'A' && *searchPos <= 'Z' || *searchPos >= 'a' && *searchPos <= 'z' || *searchPos >= '0' && *searchPos <= '9')) {
                nameStart = NULL;
                searchMode = 0;
                --searchPos;
            }
            break;

        case 2:
            if (*searchPos == '\'' && *(searchPos-1) != '\\') {
                searchMode = 0;
            }
            break;

        case 3:
            break;
        }

        ++searchPos;
    }

    //parsedText->PushSegment(L"Test", NULL);

    //parsedText->Optimize();

    return parsedText; */
}
