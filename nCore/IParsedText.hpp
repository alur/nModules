/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IParsedText.hpp
 *  The nModules Project
 *
 *  Interface for parsed text.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../Utilities/Common.h"

// Function which parses dynamic text functions.
typedef size_t(__cdecl * FORMATTINGPROC)(LPCWSTR name, UCHAR numArgs, LPWSTR * args, LPWSTR dest, size_t cchDest);

class IParsedText
{
public:
  // Evaluates this parsed text with current data.
  virtual bool Evaluate(LPWSTR dest, size_t cchDest) = 0;

  // Returns true if this parsed text contains functions whose value can change over time.
  virtual bool IsDynamic() = 0;

  // Specifies a function which should be called when.
  virtual void SetChangeHandler(void(*handler)(LPVOID), LPVOID data) = 0;

  // Releases the memory used by this particular parsedtext.
  virtual void Release() = 0;
};
