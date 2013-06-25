/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ParsedText.cpp
 *  The nModules Project
 *
 *  Contains a dynamic text string split up into its tokens.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nShared/Macros.h"
#include "ParsedText.hpp"
#include <regex>
#include <strsafe.h>
#include "../nShared/Debugging.h"
#include "../nShared/Strings.h"


// All existing functions.
FUNCMAP functionMap;


/// <summary>
/// Finds a dynamic text function. If the specified function does not exist, it is created.
/// </summary>
/// <param name="name">The name of the funtion to find.</param>
/// <param name="numArgs">The number of arguments in the function to find.</param>
FUNCMAP::iterator FindDynamicTextFunction(LPCWSTR name, UCHAR numArgs) {
    FUNCMAP::iterator ret = functionMap.find(FUNCMAP::key_type(name, numArgs));
    if (ret == functionMap.end()) {
        FormatterData d;
        d.dynamic = true;
        d.proc = nullptr;
        return functionMap.insert(FUNCMAP::value_type(FUNCMAP::key_type(wstring(name), numArgs), d)).first;
    }
    return ret;
}


/// <summary>
/// Registers a dynamic text function.
/// </summary>
/// <param name="name">The name of the funtion to register.</param>
/// <param name="numArgs">The number of arguments in the function to register.</param>
/// <param name="formatter">The evaluator for this function.</param>
/// <param name="dynamic">True if the value this function returns may change over time.</param.
EXPORT_CDECL(BOOL) RegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs, FORMATTINGPROC formatter, bool dynamic) {
    FUNCMAP::iterator iter = FindDynamicTextFunction(name, numArgs);
    iter->second.proc = formatter;
    iter->second.dynamic = dynamic;
    return FALSE;
}


/// <summary>
/// Unregisters a dynamic text function.
/// </summary>
/// <param name="name">The name of the funtion to unregister.</param>
/// <param name="numArgs">The number of arguments in the function to unregister.</param>
EXPORT_CDECL(BOOL) UnRegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs) {
    RegisterDynamicTextFunction(name, numArgs, nullptr, true);
    DynamicTextChangeNotification(name, numArgs);
    return FALSE;
}


/// <summary>
/// Should be called when the value of a single registered dynamic text function is changed.
/// </summary>
/// <param name="name">The name of the funtion which changed.</param>
/// <param name="numArgs">The number of arguments in the function which changed.</param>
EXPORT_CDECL(BOOL) DynamicTextChangeNotification(LPCWSTR name, UCHAR numArgs) {
    FUNCMAP::iterator iter = FindDynamicTextFunction(name, numArgs);
    for (set<IParsedText*>::iterator user = iter->second.users.begin(); user != iter->second.users.end(); ++user) {
        ((ParsedText*)*user)->DataChanged();
    }
    return FALSE;
}


/// <summary>
/// Returns a ParsedText object based on the specified text.
/// </summary>
/// <param name="text">The text to parse.</param>
EXPORT_CDECL(IParsedText*) ParseText(LPCWSTR text) {
    return new ParsedText(text);
}


/// <summary>
/// Creates a new ParsedText object based on the specified text.
/// </summary>
/// <param name="text">The text to parse.</param>
ParsedText::ParsedText(LPCWSTR text) {
    Parse(text);
    changeHandler = nullptr;
    data = nullptr;
}


/// <summary>
/// Destructor.
/// </summary>
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


/// <summary>
/// Specifies a function to be called when this ParsedText has changed.
/// </summary>
void ParsedText::SetChangeHandler(void (*handler)(LPVOID), LPVOID data) {
    this->data = data;
    this->changeHandler = handler;
}


/// <summary>
/// Returns true if the value of this parsedtext may change over time.
/// </summary>
bool ParsedText::IsDynamic() {
    for (auto &token : this->tokens) {
        if (token.type == EXPRESSION && token.proc->second.dynamic) {
            return true;
        }
    }
    return false;
}


/// <summary>
/// Evaluates this object using current values.
/// </summary>
/// <param name="dest">Output</param>
/// <param name="cchDest"># of characters in dest</param>
bool ParsedText::Evaluate(LPWSTR dest, size_t cchDest) {
    dest[0] = '\0';

    for (list<Token>::iterator token = this->tokens.begin(); token != this->tokens.end(); ++token) {
        switch (token->type) {
        case TEXT:
            StringCchCatW(dest, cchDest, token->text);
            break;

        case EXPRESSION:
            if (token->proc->second.proc != nullptr) {
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


/// <summary>
/// Pushes a token onto the end.
/// </summary>
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


/// <summary>
/// Calls the changehandler for this object.
/// </summary>
void ParsedText::DataChanged() {
    if (this->changeHandler) {
        this->changeHandler(this->data);
    }
}


/// <summary>
/// Parses text and pushes its tokens onto the end.
/// </summary>
/// <param name="text">The text to parse.</param>
void ParsedText::Parse(LPCWSTR text) {
    // An expression starts with a [, and ends with the first ] which is not enclosed within quotes.

    // Where the begining of the current text segment is.
    LPCWSTR pos = text;

    // The start of the current expression.
    LPCWSTR expressionStart = nullptr;

    // If we have read a whole function name, pointer to it. Otherwise, NULL;
    LPWSTR functionName = nullptr;

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
    LPWSTR* arguments = nullptr;

    //
    LPCWSTR argumentStart = nullptr;

    //
    LPCWSTR searchPos = text;

    while (searchPos != nullptr && *searchPos != L'\0') {
        switch (mode) {
        case 0: // Searching for the begining of the next expression.
            {
                searchPos = wcswcs(searchPos, L"[");
                if (searchPos != nullptr) {
                    expressionStart = searchPos;
                    ++searchPos;
                    mode = 1;
                }
            }
            break;

        case 1: // Reads the function name.
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

        case 2: // Reads arguments
            {
                searchPos = wcswcs(++searchPos, L"'");
                if (searchPos != nullptr) {
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

        case 9: // Failure state, does cleanup.
            {
                expressionStart = nullptr;
                argumentStart = nullptr;
                if (functionName) {
                    free(functionName);
                    functionName = nullptr;
                }
                if (arguments) {
                    for (int i = 0; i < numArgs; ++i) {
                        free(arguments[i]);
                    }
                    free(arguments);
                    arguments = nullptr;
                }
                mode = 0;
                numArgs = 0;
                --searchPos;
            }
            break;

        case 10: // Success state, pushes tokens
            {
                // pos through expressionStart is regular text
                AddToken(TEXT, functionMap.end(), Strings::wcsPartialDup(pos, expressionStart - pos), nullptr);

                //
                AddToken(EXPRESSION, FindDynamicTextFunction(functionName, numArgs),
                    Strings::wcsPartialDup(expressionStart + 1, searchPos - expressionStart - 1), arguments);
                free(functionName);
                functionName = nullptr;
                
                pos = ++searchPos;
                mode = 0;
            }
            break;
        }
    }

    // If there is anything left in the string, it is a text segment.
    if (*pos != '\0') {
        AddToken(TEXT, functionMap.end(), _wcsdup(pos), nullptr);
    }
}

/// <summary>
/// Deletes this object.
/// </summary>
void ParsedText::Release() {
    delete this;
}
