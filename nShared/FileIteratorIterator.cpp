/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  FilterIteratorIterator.cpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <Windows.h>
#include "FileIterator.hpp"


FileIterator::Iterator::Iterator() {
    this->isEnd = true;
}


FileIterator::Iterator::Iterator(FileIterator *fileIter) : fileIter(fileIter) {
    this->isEnd = this->fileIter->findFileHandle == INVALID_HANDLE_VALUE;
}


bool FileIterator::Iterator::operator!=(const FileIterator::Iterator &other) const {
    if (this->isEnd || other.isEnd) {
        return this->isEnd != other.isEnd;
    }
    return this->fileIter->findFileHandle != other.fileIter->findFileHandle;
}


const WIN32_FIND_DATAW &FileIterator::Iterator::operator*() const {
    return this->fileIter->GetFileData();
}


FileIterator::Iterator FileIterator::Iterator::operator++() {
    this->fileIter->NextFile();
    if (this->fileIter->findFileHandle == INVALID_HANDLE_VALUE) {
        this->isEnd = true;
    }
    return *this;
}
