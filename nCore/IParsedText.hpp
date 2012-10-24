/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IParsedText.hpp
 *  The nModules Project
 *
 *  .
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

typedef BOOL (__cdecl * FORMATTINGPROC)(LPCWSTR name, UCHAR numArgs, LPWSTR* args, LPWSTR dest, size_t cchDest);

class IParsedText {
public:
    virtual bool Evaluate(LPWSTR dest, size_t cchDest) = 0;
    virtual bool IsDynamic() = 0;
    virtual void SetChangeHandler(void (*handler)(LPVOID), LPVOID data) = 0;

    virtual void Release() = 0;
};
