//-------------------------------------------------------------------------------------------------
// /Utilities/FilterIteratorIterator.cpp
// The nModules Project
//
// Helper class for FileIteratorn.
//-------------------------------------------------------------------------------------------------
#include "Common.h"
#include "FileIterator.hpp"


FileIterator::Iterator::Iterator() {
  mIsEnd = true;
}


FileIterator::Iterator::Iterator(FileIterator *fileIter) : mFileIter(fileIter) {
  mIsEnd = mFileIter->mFindFileHandle == INVALID_HANDLE_VALUE;
}


bool FileIterator::Iterator::operator!=(const FileIterator::Iterator &other) const {
  if (mIsEnd || other.mIsEnd) {
    return mIsEnd != other.mIsEnd;
  }
  return mFileIter->mFindFileHandle != other.mFileIter->mFindFileHandle;
}


const WIN32_FIND_DATA &FileIterator::Iterator::operator*() const {
  return mFileIter->GetFileData();
}


FileIterator::Iterator FileIterator::Iterator::operator++() {
  mFileIter->NextFile();
  if (mFileIter->mFindFileHandle == INVALID_HANDLE_VALUE) {
    mIsEnd = true;
  }
  return *this;
}
