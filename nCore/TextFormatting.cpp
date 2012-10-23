/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TextFormatting.cpp
 *  The nModules Project
 *
 *  Formats text strings.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "../nShared/Macros.h"
#include "TextFormatting.h"
#include <map>
#include <ctime>
#include "ParsedText.hpp"

using std::map;


/// <summary>
/// Parses text into
/// </summary>
EXPORT_CDECL(IParsedText*) ParseText(LPCWSTR text) {
    return new ParsedText(text);

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


/// <summary>
/// Finds a formatter for the specified function.
/// </summary>
FORMATTINGPROC FindFormatter(LPCSTR pszFunc) {
    //map<string, FORMATTINGPROC>::const_iterator iter = g_formatters.find(string(pszFunc));
    //return iter != g_formatters.end() ? iter->second : NULL;
    return NULL;
}
