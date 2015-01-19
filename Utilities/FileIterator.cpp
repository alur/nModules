//-------------------------------------------------------------------------------------------------
// /Utilities/FilterIterator.cpp
// The nModules Project
//
// Allows you to iterate over all files matching a certain pattern.
//-------------------------------------------------------------------------------------------------
#include "Common.h"
#include "FileIterator.hpp"


/// <summary>
/// Creates a new FileIterator for the given path.
/// </summary>
/// <param name="path">The path to iterate over.</param>
FileIterator::FileIterator(LPCTSTR path) {
  mFindFileHandle = FindFirstFile(path, &mFindFileData);
}


FileIterator::~FileIterator() {
  if (mFindFileHandle != INVALID_HANDLE_VALUE) {
    FindClose(mFindFileHandle);
    mFindFileHandle = INVALID_HANDLE_VALUE;
  }
}


const WIN32_FIND_DATA &FileIterator::GetFileData() const {
  return mFindFileData;
}


void FileIterator::NextFile() {
  if (FindNextFile(mFindFileHandle, &mFindFileData) == FALSE) {
    FindClose(mFindFileHandle);
    mFindFileHandle = INVALID_HANDLE_VALUE;
  }
}


FileIterator::Iterator FileIterator::begin() {
  return Iterator(this);
}


FileIterator::Iterator FileIterator::end() {
  static const FileIterator::Iterator terminator;
  return terminator;
}
