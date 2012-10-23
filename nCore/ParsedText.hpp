/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ParsedText.hpp
 *  The nModules Project
 *
 *  Functions declarations for nCore.cpp
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "IParsedText.hpp"
#include <list>
#include <map>

using std::list;
using std::map;
using std::wstring;

EXPORT_CDECL(BOOL) RegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs, FORMATTINGPROC formatter, bool dynamic);
EXPORT_CDECL(BOOL) UnRegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs);

class ParsedText : public IParsedText {
public:
    explicit ParsedText(LPCWSTR text);
    virtual ~ParsedText();

    bool Evaluate(LPWSTR dest, size_t cchDest);
    bool IsDynamic();

private:
    enum TokenType {
        TEXT,
        EXPRESSION
    };

    typedef struct {
        TokenType type;
        map<wstring, FORMATTINGPROC>::const_iterator proc;
        LPCWSTR text;
    } Token;

    void Parse(LPCWSTR text);
    void AddToken(TokenType type, map<wstring, FORMATTINGPROC>::const_iterator proc, LPCWSTR text);

    list<Token> tokens;
    LPCWSTR text;
};
