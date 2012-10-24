
#include "../nShared/LiteStep.h"
#include "../nShared/Macros.h"
#include "ParsedText.hpp"
#include <regex>
#include <strsafe.h>
#include "../nShared/Debugging.h"


// All existing functions.
map<wstring, FormatterData> functionMap;

map<wstring, FormatterData>::iterator FindDynamicTextFunction(LPCWSTR name, UCHAR numArgs);


EXPORT_CDECL(BOOL) RegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs, FORMATTINGPROC formatter, bool dynamic) {
    map<wstring, FormatterData>::iterator iter = FindDynamicTextFunction(name, numArgs);
    iter->second.proc = formatter;
    iter->second.dynamic = dynamic;
    return FALSE;
}


EXPORT_CDECL(BOOL) UnRegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs) {
    RegisterDynamicTextFunction(name, numArgs, NULL, true);
    DynamicTextChangeNotification(name, numArgs);
    return FALSE;
}


EXPORT_CDECL(BOOL) DynamicTextChangeNotification(LPCWSTR name, UCHAR numArgs) {
    map<wstring, FormatterData>::iterator iter = FindDynamicTextFunction(name, numArgs);
    for (set<IParsedText*>::iterator user = iter->second.users.begin(); user != iter->second.users.end(); ++user) {
        ((ParsedText*)*user)->DataChanged();
    }
    return FALSE;
}


/// <summary>
/// Parses text into
/// </summary>
EXPORT_CDECL(IParsedText*) ParseText(LPCWSTR text) {
    return new ParsedText(text);
}


map<wstring, FormatterData>::iterator FindDynamicTextFunction(LPCWSTR name, UCHAR numArgs) {
    map<wstring, FormatterData>::iterator ret = functionMap.find(name);
    if (ret == functionMap.end()) {
        FormatterData d;
        d.dynamic = true;
        d.proc = NULL;
        return functionMap.insert(std::pair<wstring, FormatterData>(wstring(name), d)).first;
    }
    return ret;
}


ParsedText::ParsedText(LPCWSTR text) {
    Parse(text);
    changeHandler = NULL;
    data = NULL;
}


ParsedText::~ParsedText() {
    for (list<Token>::iterator token = this->tokens.begin(); token != this->tokens.end(); ++token) {
        if (token->type == EXPRESSION) {
            token->proc->second.users.erase(this);
            free((LPVOID)token->text);
        }
    }
    this->tokens.clear();
}


void ParsedText::SetChangeHandler(void (*handler)(LPVOID), LPVOID data) {
    this->data = data;
    this->changeHandler = handler;
}


bool ParsedText::IsDynamic() {
    for (list<Token>::const_iterator token = this->tokens.begin(); token != this->tokens.end(); ++token) {
        if (token->type == EXPRESSION && token->proc->second.dynamic) {
            return true;
        }
    }
    return false;
}


bool ParsedText::Evaluate(LPWSTR dest, size_t cchDest) {
    dest[0] = '\0';

    for (list<Token>::const_iterator token = this->tokens.begin(); token != this->tokens.end(); ++token) {
        switch (token->type) {
        case TEXT:
            StringCchCatW(dest, cchDest, token->text);
            break;

        case EXPRESSION:
            if (token->proc->second.proc != NULL) {
                token->proc->second.proc(L"", 0, dest, cchDest); 
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


void ParsedText::AddToken(TokenType type, map<wstring, FormatterData>::iterator proc, LPCWSTR str) {
    Token t;
    t.type = type;
    t.proc = proc;
    t.text = str;
    tokens.push_back(t);
    if (type == EXPRESSION) {
        proc->second.users.insert(this);
    }
}


void ParsedText::DataChanged() {
    if (this->changeHandler) {
        this->changeHandler(this->data);
    }
}


void ParsedText::Release() {
    delete this;
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
