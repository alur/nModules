#include "LayoutSettings.hpp"


LayoutSettings::LayoutSettings() {
  mColumnSpacing = NLENGTH(0, 0, 2);
  mPadding.left = NLENGTH(0, 0, 0);
  mPadding.top = NLENGTH(0, 0, 0);
  mPadding.right = NLENGTH(0, 0, 0);
  mPadding.bottom = NLENGTH(0, 0, 0);
  mPrimaryDirection = Direction::Horizontal;
  mRowSpacing = NLENGTH(0, 0, 2);
  mStartPosition = StartPosition::TopLeft;
}


void LayoutSettings::Load(const ISettingsReader *reader) {
  TCHAR buffer[32];

  mColumnSpacing = reader->GetLength(L"ColumnSpacing", mColumnSpacing);
  mRowSpacing = reader->GetLength(L"RowSpacing", mRowSpacing);
  mPadding = reader->GetLTRBRect(L"Padding", mPadding);

  reader->GetString(L"Start", buffer, 32, L"TopLeft");
  if (_wcsicmp(buffer, L"TopRight") == 0) {
    mStartPosition = StartPosition::TopRight;
  } else if (_wcsicmp(buffer, L"BottomLeft") == 0) {
    mStartPosition = StartPosition::BottomLeft;
  } else if (_wcsicmp(buffer, L"BottomRight") == 0) {
    mStartPosition = StartPosition::BottomRight;
  } else {
    mStartPosition = StartPosition::TopLeft;
  }

  reader->GetString(L"PrimaryDirection", buffer, 32, L"Horizontal");
  if (_wcsicmp(buffer, L"Vertical") == 0) {
    mPrimaryDirection = Direction::Vertical;
  } else {
    mPrimaryDirection = Direction::Horizontal;
  }
}
