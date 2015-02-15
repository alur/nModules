#pragma once

#include "../nCoreApi/ISettingsReader.hpp"

#include <vector>

class SettingsReader : public ISettingsReader {
public:
  static ISettingsReader *Create(LPCWSTR prefix);

private:
  SettingsReader();

  // IDiscardable
public:
  void APICALL Discard() override;

  // ISettingsReader
public:
  ISettingsReader* APICALL CreateChild(LPCWSTR) const override;

  bool APICALL GetBool(LPCWSTR key, bool defaultValue) const override;
  bool APICALL GetBool(LPCWSTR key, bool *value) const override;

  double APICALL GetDouble(LPCWSTR key, double defaultValue) const override;
  bool APICALL GetDouble(LPCWSTR key, double *value) const override;

  float APICALL GetFloat(LPCWSTR key, float defaultValue) const override;
  bool APICALL GetFloat(LPCWSTR key, float *value) const override;

  int APICALL GetInt(LPCWSTR key, int defaultValue) const override;
  bool APICALL GetInt(LPCWSTR key, int *value) const override;

  __int64 APICALL GetInt64(LPCWSTR key, __int64 defaultValue) const override;
  bool APICALL GetInt64(LPCWSTR key, __int64 *value) const override;

  bool APICALL GetString(LPCWSTR key, LPWSTR value, size_t cchValue,
    LPWSTR defaultValue) const override;

  NLENGTH APICALL GetLength(LPCWSTR key, const NLENGTH &defaultValue) const override;
  bool APICALL GetLength(LPCWSTR key, NLENGTH *value) const override;

  NRECT APICALL GetXYWHRect(LPCWSTR key, const NRECT &defaultValue) const override;
  bool APICALL GetXYWHRect(LPCWSTR key, NRECT *value) const override;

  NRECT APICALL GetLTRBRect(LPCWSTR key, const NRECT &defaultValue) const override;
  bool APICALL GetLTRBRect(LPCWSTR key, NRECT *value) const override;

private:
  class PrefixVal {
  public:
    operator const wchar_t*() const {
      return mVal;
    }
    operator wchar_t*() {
      return mVal;
    }
  private:
    wchar_t mVal[MAX_PREFIX];
  };

private:
  std::vector<PrefixVal> mPrefixes;
};
