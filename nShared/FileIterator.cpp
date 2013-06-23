/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  FilterIterator.cpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <Windows.h>
#include "FileIterator.hpp"


FileIterator::FileIterator(LPCWSTR path) {
    this->findFileHandle = FindFirstFileW(path, &this->findFileData);
}


FileIterator::~FileIterator() {
    if (this->findFileHandle != INVALID_HANDLE_VALUE) {
        FindClose(this->findFileHandle);
        this->findFileHandle = INVALID_HANDLE_VALUE;
    }
}


const WIN32_FIND_DATAW &FileIterator::GetFileData() const {
    return this->findFileData;
}


void FileIterator::NextFile() {
    if (FindNextFileW(this->findFileHandle, &this->findFileData) == FALSE) {
        FindClose(this->findFileHandle);
        this->findFileHandle = INVALID_HANDLE_VALUE;
    }
}


FileIterator::Iterator FileIterator::begin() {
    return Iterator(this);
}


FileIterator::Iterator FileIterator::end() {
    static FileIterator::Iterator terminator;
    return terminator;
}
