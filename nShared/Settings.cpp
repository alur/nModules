//-------------------------------------------------------------------------------------------------
// /nShared/Settings.cpp
// The nModules Project
//
// Manages RC settings with a certain prefix.
//-------------------------------------------------------------------------------------------------
#include "LiteStep.h"
#include "Settings.hpp"
#include "ErrorHandler.h"

#include <strsafe.h>

using std::unique_ptr;
using std::function;
using namespace LiteStep;


/// <summary>
/// Initalizes a new Settings class.
/// </summary>
/// <param name="prefix">The RC prefix to use.</param>
Settings::Settings(LPCTSTR prefix) {
  StringCchCopy(mPrefix, _countof(mPrefix), prefix);
  mGroup = unique_ptr<Settings>(GreateGroup(nullptr));
}


/// <summary>
/// Creates a deep copy of the specified group.
/// </summary>
/// <param name="settings">The settings to copy.</param>
Settings::Settings(LPCSettings settings) {
  StringCchCopy(mPrefix, _countof(mPrefix), settings->mPrefix);
  if (settings->mGroup != nullptr) {
    mGroup = unique_ptr<Settings>(new (std::nothrow) Settings(settings->mGroup.get()));
  } else {
    mGroup = nullptr;
  }
}


/// <summary>
/// Initalizes a new Settings class, due to the precense of a Group setting in another class.
/// </summary>
/// <param name="prefix">The RC prefix to use.</param>
/// <param name="prefixTrail">A list of previous group names.</param>
Settings::Settings(LPCTSTR prefix, LPCTSTR prefixTrail[]) {
  StringCchCopy(mPrefix, _countof(mPrefix), prefix);
  mGroup = unique_ptr<Settings>(GreateGroup(prefixTrail));
}


/// <summary>
/// Creates a child of this Settings*. If you have a Settings with the prefix of Label, and you want
/// a related setting LabelIcon, you should call ->GetChild("Icon").
/// </summary>
LPSettings Settings::CreateChild(LPCTSTR prefix) const {
  LPSettings head, newTail;
  LPCSettings thisTail;
  TCHAR newPrefix[MAX_RCCOMMAND];

  StringCchPrintf(newPrefix, _countof(newPrefix), L"%s%s", mPrefix, prefix);
  head = new Settings(newPrefix);
  thisTail = this;
  newTail = head;

  while (thisTail->mGroup != nullptr) {
    thisTail = thisTail->mGroup.get();
    StringCchPrintf(newPrefix, _countof(newPrefix), L"%s%s", thisTail->mPrefix, prefix);
    while (newTail->mGroup != nullptr) {
      newTail = newTail->mGroup.get();
    }

    newTail->mGroup = unique_ptr<Settings>(new Settings(newPrefix));
  }

  return head;
}


/// <summary>
/// Appends the specified prefix to the end of the group list. Essentially, lets these
/// settings fall back to that group as a default.
/// </summary>
void Settings::AppendGroup(LPCSettings group) {
  LPSettings tail;
  for (tail = this; tail->mGroup != nullptr; tail = tail->mGroup.get());
  tail->mGroup = unique_ptr<Settings>(new (std::nothrow) Settings(group));
}


/// <summary>
/// Returns the fully qualified prefix used to read settings.
/// </summary>
LPCTSTR Settings::GetPrefix() const {
  return mPrefix;
}


/// <summary>
/// Gets the value we should use for m_pGroup.
/// </summary>
/// <param name="prefixTrail">A list of previous group names.</param>
/// <returns>The value we should use for m_pGroup.</returns>
Settings* Settings::GreateGroup(LPCTSTR prefixTrail[]) {
  TCHAR group[MAX_LINE_LENGTH];
  GetPrefixedRCString(mPrefix, L"Group", group, L"", _countof(group));

  // If there is no group
  if (group[0] == L'\0') {
    // We need to free pszPrev at this point.
    if (prefixTrail != nullptr) {
      free((LPVOID)prefixTrail);
    }

    return nullptr;
  }

  // Avoid circular definitions
  int i = 0;
  if (prefixTrail != nullptr) {
    for (; prefixTrail[i] != nullptr; ++i) {
      if (_wcsicmp(prefixTrail[i], group) == 0) {
        // We found a circle :/

        // Show an error message
        TCHAR message[MAX_LINE_LENGTH];
        StringCchCopy(message, _countof(message), L"Circular group definition!\n");

        // A -> B -> C -> ... -> C
        for (int j = 0; prefixTrail[j] != NULL; j++) {
          StringCchCat(message, _countof(message), prefixTrail[j]);
          StringCchCat(message, _countof(message), L" -> ");
        }
        StringCchCat(message, _countof(message), group);

        ErrorHandler::Error(ErrorHandler::Level::Critical, message);

        // And break out of the chain
        free((LPVOID)prefixTrail);
        return nullptr;
      }
    }
  }

  // Allocate space for storing the prefix of this setting
  prefixTrail = (LPCTSTR*)realloc(prefixTrail, (i + 2)*sizeof(LPCTSTR));
  prefixTrail[i] = mPrefix;
  prefixTrail[i + 1] = nullptr;

  return new Settings(group, prefixTrail);
}


/// <summary>
/// Gets a boolean from a prefixed RC value.
/// </summary>
/// <param name="key">The RC setting to parse.</param>
/// <param name="defaultValue">The default value to use, if the setting is invalid or unspecified.</param>
/// <returns>The boolean.</returns>
bool Settings::GetBool(LPCTSTR key, bool defaultValue) const {
  return GetPrefixedRCBool(mPrefix, key, mGroup != nullptr ? mGroup->GetBool(key, defaultValue) : defaultValue);
}


/// <summary>
/// Set's a prefixed RC value to a particular boolean.
/// </summary>
/// <param name="key">The RC setting.</param>
/// <param name="value">The value to set the setting to.</param>
void Settings::SetBool(LPCTSTR key, bool value) const {
  SetString(key, value ? L"True" : L"False");
}


/// <summary>
/// Gets a color from a prefixed RC value.
/// </summary>
/// <param name="key">The RC setting to retrive.</param>
/// <param name="defaultValue">The default color to use, if the setting is invalid or unspecified.</param>
/// <returns>The color.</returns>
IColorVal* Settings::GetColor(LPCTSTR key, const IColorVal* defaultValue) const {
  TCHAR colorString[MAX_LINE_LENGTH];
  GetLine(key, colorString, _countof(colorString), nullptr);
  return ParseColor(colorString, defaultValue);
}


/// <summary>
/// Sets a prefixed RC value to a particular color.
/// </summary>
/// <param name="key">The RC setting to set.</param>
/// <param name="value">The value to set the setting to.</param>
void Settings::SetColor(LPCTSTR key, ARGB value) const {
  TCHAR valueString[32];
  StringCchPrintf(valueString, _countof(valueString), L"#%x", value);
  SetString(key, valueString);
}


/// <summary>
/// Gets a double from a prefixed RC value.
/// </summary>
/// <param name="key">The RC setting to parse.</param>
/// <param name="defaultValue">The default value to use, if the setting is invalid or unspecified.</param>
/// <returns>The double.</returns>
double Settings::GetDouble(LPCTSTR key, double defaultValue) const {
  return GetPrefixedRCDouble(mPrefix, key, mGroup != nullptr ? mGroup->GetDouble(key, defaultValue) : defaultValue);
}


/// <summary>
/// Sets a prefixed RC value to a particular double.
/// </summary>
/// <param name="pszSetting">The RC setting.</param>
/// <param name="value">The value to set the setting to.</param>
void Settings::SetDouble(LPCTSTR key, double value) const {
  TCHAR valueString[32];
  StringCchPrintf(valueString, _countof(valueString), L"%.20f", value);
  SetString(key, valueString);
}


/// <summary>
/// Gets a float from a prefixed RC value.
/// </summary>
/// <param name="key">The RC setting to parse.</param>
/// <param name="defaultValue">The default value to use, if the setting is invalid or unspecified.</param>
/// <returns>The float.</returns>
float Settings::GetFloat(LPCTSTR key, float defaultValue) const {
  return GetPrefixedRCFloat(mPrefix, key, mGroup != nullptr ? mGroup->GetFloat(key, defaultValue) : defaultValue);
}


/// <summary>
/// Sets a prefixed RC value to a particular float.
/// </summary>
/// <param name="key">The RC setting.</param>
/// <param name="value">The value to set the setting to.</param>
void Settings::SetFloat(LPCTSTR key, float value) const {
  TCHAR valueString[32];
  StringCchPrintf(valueString, _countof(valueString), L"%.10f", value);
  SetString(key, valueString);
}


/// <summary>
/// Gets an integer from a prefixed RC value.
/// </summary>
/// <param name="key">The RC setting to parse.</param>
/// <param name="defaultValue">The default value to use, if the setting is invalid or unspecified.</param>
/// <returns>The float.</returns>
int Settings::GetInt(LPCTSTR key, int defaultValue) const {
  return GetPrefixedRCInt(mPrefix, key, mGroup != nullptr ? mGroup->GetInt(key, defaultValue) : defaultValue);
}


/// <summary>
/// Sets a prefixed RC value to a particular integer.
/// </summary>
/// <param name="key">The RC setting.</param>
/// <param name="value">The value to set the setting to.</param>
void Settings::SetInt(LPCTSTR key, int value) const {
  TCHAR valueString[32];
  StringCchPrintf(valueString, _countof(valueString), L"%d", value);
  SetString(key, valueString);
}


/// <summary>
/// Gets a 64-bit integer from a prefixed RC value.
/// </summary>
/// <param name="key">The RC setting to parse.</param>
/// <param name="defaultValue">The default value to use, if the setting is invalid or unspecified.</param>
/// <returns>The float.</returns>
__int64 Settings::GetInt64(LPCTSTR key, __int64 defaultValue) const {
  return GetPrefixedRCInt64(mPrefix, key, mGroup != nullptr ? mGroup->GetInt64(key, defaultValue) : defaultValue);
}


/// <summary>
/// Sets a prefixed RC value to a particular integer.
/// </summary>
/// <param name="key">The RC setting.</param>
/// <param name="value">The value to set the setting to.</param>
void Settings::SetInt64(LPCTSTR key, __int64 value) const {
  TCHAR valueString[32];
  StringCchPrintf(valueString, _countof(valueString), L"%lld", value);
  SetString(key, valueString);
}


/// <summary>
/// Get's a Monitor from a prefixed RC value.
/// </summary>
/// <param name="key">The RC setting to parse.</param>
/// <param name="defaultValue">The default value to use, if the setting is invalid or unspecified.</param>
/// <returns>The monitor.</returns>
UINT Settings::GetMonitor(LPCTSTR key, UINT defaultValue) const {
  return GetPrefixedRCMonitor(mPrefix, key, mGroup != nullptr ? mGroup->GetMonitor(key, defaultValue) : defaultValue);
}


/// <summary>
/// Set's a prefixed RC value to a particular monitor.
/// </summary>
/// <param name="key">The RC setting.</param>
/// <param name="value">The value to set the setting to.</param>
void Settings::SetMonitor(LPCTSTR key, UINT value) const {
  TCHAR valueString[10];
  StringCchPrintf(valueString, _countof(valueString), L"%u", value);
  SetString(key, valueString);
}


/// <summary>
/// Get's a Distance from a prefixed RC value.
/// </summary>
/// <param name="key">The RC setting to parse.</param>
/// <param name="defaultValue">The default value to use, if the setting is invalid or unspecified.</param>
/// <returns>The related number.</returns>
Distance Settings::GetDistance(LPCTSTR key, Distance defaultValue) const {
  return GetPrefixedRCDistance(mPrefix, key, mGroup != nullptr ? mGroup->GetDistance(key, defaultValue) : defaultValue);
}


/// <summary>
/// Gets a line from a prefixed RC value.
/// </summary>
/// <param name="key">The RC setting.</param>
/// <param name="buffer">Where the string should be read to.</param>
/// <param name="cchBuffer">The maximum number of characters to write to buffer.</param>
/// <param name="defaultValue">The default string, used if the RC value is unspecified.</param>
/// <returns>False if the length of the RC value is > cchDest. True otherwise.</returns>
bool Settings::GetLine(LPCTSTR key, LPTSTR buffer, UINT cchBuffer, LPCTSTR defaultValue) const {
  if (mGroup != nullptr) {
    TCHAR propagatedDefault[MAX_LINE_LENGTH];
    mGroup->GetLine(key, propagatedDefault, _countof(propagatedDefault), defaultValue);
    return GetPrefixedRCLine(mPrefix, key, buffer, propagatedDefault, cchBuffer);
  }
  return GetPrefixedRCLine(mPrefix, key, buffer, defaultValue, cchBuffer);
}


/// <summary>
/// Gets a string from a prefixed RC value.
/// </summary>
/// <param name="key">The RC setting.</param>
/// <param name="buffer">Where the string should be read to.</param>
/// <param name="cchBuffer">The maximum number of characters to write to buffer.</param>
/// <param name="defaultValue">The default string, used if the RC value is unspecified.</param>
/// <returns>False if the length of the RC value is > cchDest. True otherwise.</returns>
bool Settings::GetString(LPCTSTR key, LPTSTR buffer, UINT cchBuffer, LPCTSTR defaultValue) const {
  if (mGroup != nullptr) {
    TCHAR propagatedDefault[MAX_LINE_LENGTH];
    mGroup->GetString(key, propagatedDefault, _countof(propagatedDefault), defaultValue);
    return GetPrefixedRCString(mPrefix, key, buffer, propagatedDefault, cchBuffer);
  }
  return GetPrefixedRCString(mPrefix, key, buffer, defaultValue, cchBuffer);
}


/// <summary>
/// Sets a prefixed RC value to a particular string.
/// </summary>
/// <param name="pszSetting">The RC setting.</param>
/// <param name="pszValue">The value to set the setting to.</param>
void Settings::SetString(LPCTSTR key, LPCTSTR value) const {
  TCHAR keyName[MAX_LINE_LENGTH];
  StringCchPrintf(keyName, _countof(keyName), L"%s%s", mPrefix, key);
  LSSetVariable(keyName, value);
}


/// <summary>
/// Reads an X, Y, Width, Height series of settings into a RECT.
/// </summary>
/// <param name="key">The key to read.</param>
/// <param name="defaultValue">The default x/y/width/height, if not specified.</param>
RECT Settings::GetRect(LPCTSTR key, LPCRECT defaultValue) const {
  return GetRect(key, defaultValue->left, defaultValue->top, defaultValue->right - defaultValue->left, defaultValue->bottom - defaultValue->top);
}


/// <summary>
/// Reads an X, Y, Width, Height series of settings into a RECT.
/// </summary>
/// <param name="key">The key to read.</param>
/// <param name="defX">The default X value, if not specified.</param>
/// <param name="defY">The default Y value, if not specified.</param>
/// <param name="defW">The default width, if not specified.</param>
/// <param name="defH">The default height, if not specified.</param>
RECT Settings::GetRect(LPCTSTR key, LONG defX, LONG defY, LONG defWidth, LONG defHeight) const {
  TCHAR xKey[MAX_RCCOMMAND], yKey[MAX_RCCOMMAND], widthKey[MAX_RCCOMMAND], heightKey[MAX_RCCOMMAND];
  StringCchPrintf(xKey, _countof(xKey), L"%sX", key);
  StringCchPrintf(yKey, _countof(yKey), L"%sY", key);
  StringCchPrintf(widthKey, _countof(widthKey), L"%sWidth", key);
  StringCchPrintf(heightKey, _countof(heightKey), L"%sHeight", key);

  RECT rect;
  rect.left = GetInt(xKey, defX);
  rect.top = GetInt(yKey, defY);
  rect.right = rect.left + GetInt(widthKey, defWidth);
  rect.bottom = rect.top + GetInt(heightKey, defHeight);

  return rect;
}


/// <summary>
/// Sets an X, Y, Width, Height series of settings from a RECT.
/// </summary>
/// <param name="key">The key to set.</param>
/// <param name="value">The source RECT.</param>
void Settings::SetRect(LPCTSTR key, LPCRECT value) const {
  TCHAR xKey[MAX_RCCOMMAND], yKey[MAX_RCCOMMAND], widthKey[MAX_RCCOMMAND], heightKey[MAX_RCCOMMAND];
  StringCchPrintf(xKey, _countof(xKey), L"%sX", key);
  StringCchPrintf(yKey, _countof(yKey), L"%sY", key);
  StringCchPrintf(widthKey, _countof(widthKey), L"%sWidth", key);
  StringCchPrintf(heightKey, _countof(heightKey), L"%sHeight", key);

  SetInt(xKey, value->left);
  SetInt(yKey, value->top);
  SetInt(widthKey, value->right - value->left);
  SetInt(heightKey, value->bottom - value->top);
}


/// <summary>
/// Reads a series of values into a RECT structure.
/// </summary>
/// <param name="key">The key to read.</param>
/// <param name="defaultValue">The default value.</param>
RECT Settings::GetOffsetRect(LPCTSTR key, LPCRECT defaultValue) const {
  return GetOffsetRect(key, defaultValue->left, defaultValue->top, defaultValue->right, defaultValue->bottom);
}


/// <summary>
/// Reads a series of values into a RECT structure.
/// </summary>
/// <param name="key">The key to read.</param>
/// <param name="defLeft">The default left value, if not specified.</param>
/// <param name="defTop">The default top value, if not specified.</param>
/// <param name="defRight">The default right, if not specified.</param>
/// <param name="defBottom">The default bottom, if not specified.</param>
RECT Settings::GetOffsetRect(LPCTSTR key, LONG defLeft, LONG defTop, LONG defRight, LONG defBottom) const {
  TCHAR leftKey[MAX_RCCOMMAND], topKey[MAX_RCCOMMAND], rightKey[MAX_RCCOMMAND], bottomKey[MAX_RCCOMMAND];
  StringCchPrintf(leftKey, _countof(leftKey), L"%sLeft", key);
  StringCchPrintf(topKey, _countof(topKey), L"%sTop", key);
  StringCchPrintf(rightKey, _countof(rightKey), L"%sRight", key);
  StringCchPrintf(bottomKey, _countof(bottomKey), L"%sBottom", key);

  RECT rect;
  rect.left = GetInt(leftKey, defLeft);
  rect.top = GetInt(topKey, defTop);
  rect.right = GetInt(rightKey, defRight);
  rect.bottom = GetInt(bottomKey, defBottom);

  return rect;
}


/// <summary>
/// Sets a Left, Top, Right, Bottom series of settings from a RECT.
/// </summary>
/// <param name="key">The key to set.</param>
/// <param name="value">The source RECT.</param>
void Settings::SetOffsetRect(LPCTSTR key, LPCRECT value) const {
  TCHAR leftKey[MAX_RCCOMMAND], topKey[MAX_RCCOMMAND], rightKey[MAX_RCCOMMAND], bottomKey[MAX_RCCOMMAND];
  StringCchPrintf(leftKey, _countof(leftKey), L"%sLeft", key);
  StringCchPrintf(topKey, _countof(topKey), L"%sTop", key);
  StringCchPrintf(rightKey, _countof(rightKey), L"%sRight", key);
  StringCchPrintf(bottomKey, _countof(bottomKey), L"%sBottom", key);

  SetInt(leftKey, value->left);
  SetInt(topKey, value->top);
  SetInt(rightKey, value->right);
  SetInt(bottomKey, value->bottom);
}


/// <summary>
/// Reads a series of values into a D2D1_RECT_F structure.
/// </summary>
/// <param name="key">The key to read.</param>
/// <param name="defaultValue">The default value.</param>
D2D1_RECT_F Settings::GetOffsetRectF(LPCTSTR key, const D2D1_RECT_F * defaultValue) const {
  TCHAR leftKey[MAX_RCCOMMAND], topKey[MAX_RCCOMMAND], rightKey[MAX_RCCOMMAND], bottomKey[MAX_RCCOMMAND];
  StringCchPrintf(leftKey, _countof(leftKey), L"%sLeft", key);
  StringCchPrintf(topKey, _countof(topKey), L"%sTop", key);
  StringCchPrintf(rightKey, _countof(rightKey), L"%sRight", key);
  StringCchPrintf(bottomKey, _countof(bottomKey), L"%sBottom", key);

  D2D1_RECT_F rect;
  rect.left = GetFloat(leftKey, defaultValue->left);
  rect.top = GetFloat(topKey, defaultValue->top);
  rect.right = GetFloat(rightKey, defaultValue->right);
  rect.bottom = GetFloat(bottomKey, defaultValue->bottom);

  return rect;
}


/// <summary>
/// Iterates over lines of the form *PrefixKey.
/// </summary>
/// <param name="key">Key of the lines to iterate over.</param>
/// <param name="callback">Function to call for each line.</param>
void Settings::IterateOverCommandLines(LPCTSTR key, function<void(LPCTSTR line)> callback) const {
  TCHAR keyPrefix[MAX_RCCOMMAND];

  LPCSettings settings = this;
  while (settings != nullptr) {
    StringCchPrintf(keyPrefix, _countof(keyPrefix), L"*%s%s", settings->mPrefix, key);
    IterateOverLines(keyPrefix, callback);
    settings = settings->mGroup.get();
  }
}


/// <summary>
/// Iterates over lines of the form *PrefixKey.
/// </summary>
/// <param name="key">Key of the lines to iterate over.</param>
/// <param name="callback">Function to call for each token.</param>
void Settings::IterateOverCommandTokens(LPCTSTR key, function<void(LPCTSTR token)> callback) const {
  IterateOverCommandLines(key, [&callback] (LPCTSTR line) {
    IterateOverTokens(line, callback);
  });
}
