#pragma once

#include "IParsedText.hpp"
#include "TextFormatting.h"

class ParsedText : public IParsedText {
public:
    bool Evaluate(LPWSTR dest, size_t cchDest);
    void PushSegment(LPWSTR segment, FORMATTINGPROC proc);
    void Optimize();
};
