/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  FilterIterator.hpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

class FileIterator {
public:
    class Iterator {
    public:
        explicit Iterator();
        explicit Iterator(FileIterator*);

    public:
        bool operator!=(const Iterator&) const;
        const WIN32_FIND_DATAW &operator*() const;
        Iterator operator++();

    private:
        bool isEnd;
        FileIterator *fileIter;
    };

public:
    explicit FileIterator(LPCWSTR path);
    virtual ~FileIterator();

public:
    const WIN32_FIND_DATAW &GetFileData() const;
    void NextFile();

public:
    Iterator begin();
    Iterator end();

private:
    HANDLE findFileHandle;
    WIN32_FIND_DATAW findFileData;
};
