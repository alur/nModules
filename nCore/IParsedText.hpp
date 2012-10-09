#pragma once

class IParsedText {
public:
    virtual bool Evaluate(LPWSTR dest, size_t cchDest) = 0;
};
