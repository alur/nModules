/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IParsedText.hpp
 *  The nModules Project
 *
 *  .
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

typedef BOOL (__cdecl * FORMATTINGPROC)(LPCWSTR name, UCHAR numArgs, LPWSTR dest, size_t cchDest);

class IParsedText {
public:
    virtual bool Evaluate(LPWSTR dest, size_t cchDest) = 0;
    virtual bool IsDynamic() = 0;
};
