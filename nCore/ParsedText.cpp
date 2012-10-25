
#include "../nShared/LiteStep.h"
#include "../nShared/Macros.h"
#include "ParsedText.hpp"
#include <regex>
#include <strsafe.h>
#include "../nShared/Debugging.h"
#include "../nShared/Strings.h"


// All existing functions.
FUNCMAP functionMap;

FUNCMAP::iterator FindDynamicTextFunction(LPCWSTR name, UCHAR numArgs);


EXPORT_CDECL(BOOL) RegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs, FORMATTINGPROC formatter, bool dynamic) {
    FUNCMAP::iterator iter = FindDynamicTextFunction(name, numArgs);
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
    FUNCMAP::iterator iter = FindDynamicTextFunction(name, numArgs);
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


FUNCMAP::iterator FindDynamicTextFunction(LPCWSTR name, UCHAR numArgs) {
    FUNCMAP::iterator ret = functionMap.find(FUNCMAP::key_type(name, numArgs));
    if (ret == functionMap.end()) {
        FormatterData d;
        d.dynamic = true;
        d.proc = NULL;
        return functionMap.insert(FUNCMAP::value_type(FUNCMAP::key_type(wstring(name), numArgs), d)).first;
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
            for (int i = 0; i < token->proc->first.second; ++i) {
                free(token->args[i]);
            }
            if (token->args) {
                free(token->args);
            }
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

    for (list<Token>::iterator token = this->tokens.begin(); token != this->tokens.end(); ++token) {
        switch (token->type) {
        case TEXT:
            StringCchCatW(dest, cchDest, token->text);
            break;

        case EXPRESSION:
            if (token->proc->second.proc != NULL) {
                token->proc->second.proc(L"", (UCHAR)token->proc->first.second, token->args, dest, cchDest); 
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


void ParsedText::AddToken(TokenType type, FUNCMAP::iterator proc, LPCWSTR str, LPWSTR* args) {
    Token t;
    t.type = type;
    t.proc = proc;
    t.text = str;
    t.args = args;
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

    // Where the begining of the current text segment is.
    LPCWSTR pos = text;

    // The start of the current expression.
    LPCWSTR expressionStart = NULL;

    // If we have read a whole function name, pointer to it. Otherwise, NULL;
    LPWSTR functionName = NULL;

    // What we are currently searching for
    // 0 -> Begining of an expression, [
    // 1 -> End of expression or start of arguments, ( or ] --- reverting to 0 on non-alphanumeric
    // 2 -> Arguments
    // 9  -> Failure -> deallocate.
    // 10 -> Success -> push token.
    UINT mode = 0;

    //
    UCHAR numArgs = 0;

    //
    LPWSTR* arguments = NULL;

    //
    LPCWSTR argumentStart = NULL;

    //
    LPCWSTR searchPos = text;

    while (searchPos != NULL && *searchPos != L'\0') {
        switch (mode) {
        case 0:
            {
                searchPos = wcswcs(searchPos, L"[");
                if (searchPos != NULL) {
                    expressionStart = searchPos;
                    ++searchPos;
                    mode = 1;
                }
            }
            break;

        case 1:
            {
                if (*++searchPos == L']') {
                    functionName = Strings::wcsPartialDup(expressionStart + 1, searchPos - expressionStart - 1);
                    mode = 10;
                }
                else if (*searchPos == L'(') {
                    if (*++searchPos == L'\'') {
                        argumentStart = searchPos;
                        functionName = Strings::wcsPartialDup(expressionStart + 1, searchPos - expressionStart - 2);
                        mode = 2;
                    }
                    else {
                        mode = 9;
                    }
                }
                else if (!iswalnum(*searchPos)) {
                    mode = 9;
                }
            }
            break;

        case 2:
            {
                searchPos = wcswcs(++searchPos, L"'");
                if (searchPos != NULL) {
                    ++numArgs;
                    arguments = (LPWSTR*)realloc(arguments, numArgs*sizeof(LPWSTR));
                    arguments[numArgs-1] = Strings::wcsPartialDup(argumentStart + 1, searchPos - argumentStart - 1);

                    if (*++searchPos == L',') {
                        // We REQUIRE a space after the ,
                        if (*++searchPos == L' ' && *++searchPos == L'\'') {
                            argumentStart = searchPos;
                        }
                        else {
                            --searchPos;
                            mode = 9;
                        }
                    }
                    else if (*searchPos == L')') {
                        // The next character needs to be a ]
                        if (*++searchPos == L']') {
                            mode = 10;
                        }
                        else {
                            // Terminating ) not followed by ]
                            mode = 9;
                        }
                    }
                    else {
                        // Terminating ' not followed by , or )
                        mode = 9;
                    }
                }
                else {
                    // Missing terminating '
                    mode = 9;
                }
            }
            break;

        case 9:
            {
                expressionStart = NULL;
                argumentStart = NULL;
                if (functionName) {
                    free(functionName);
                    functionName = NULL;
                }
                if (arguments) {
                    for (int i = 0; i < numArgs; ++i) {
                        free(arguments[i]);
                    }
                    free(arguments);
                    arguments = NULL;
                }
                mode = 0;
                numArgs = 0;
                --searchPos;
            }
            break;

        case 10:
            {
                // pos through expressionStart is regular text
                AddToken(TEXT, functionMap.end(), Strings::wcsPartialDup(pos, expressionStart - pos), NULL);

                //
                AddToken(EXPRESSION, FindDynamicTextFunction(functionName, numArgs),
                    Strings::wcsPartialDup(expressionStart, searchPos - expressionStart), arguments);
                free(functionName);
                functionName = NULL;
                
                pos = ++searchPos;
                mode = 0;
            }
            break;
        }
    }

    // If there is anything left in the string, it is a text segment.
    if (*pos != '\0') {
        AddToken(TEXT, functionMap.end(), _wcsdup(pos), NULL);
    }
}
