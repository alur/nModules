//-------------------------------------------------------------------------------------------------
// /Utilities/FilterIterator.hpp
// The nModules Project
//
// Allows you to iterate over all files matching a certain pattern.
//-------------------------------------------------------------------------------------------------
#pragma once

class FileIterator {
public:
  class Iterator {
  public:
    explicit Iterator();
    explicit Iterator(FileIterator*);

  public:
    bool operator!=(const Iterator&) const;
    const WIN32_FIND_DATA &operator*() const;
    Iterator operator++();

  private:
    bool mIsEnd;
    FileIterator *mFileIter;
  };

public:
  explicit FileIterator(LPCTSTR path);
  ~FileIterator();

public:
  const WIN32_FIND_DATA &GetFileData() const;
  void NextFile();

public:
  Iterator begin();
  Iterator end();

private:
  HANDLE mFindFileHandle;
  WIN32_FIND_DATA mFindFileData;
};
