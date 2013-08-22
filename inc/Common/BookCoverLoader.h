#ifndef __COMMON_BOOKCOVERLOADER_H__
#define __COMMON_BOOKCOVERLOADER_H__

#include "Common/LockedStack.h"
#include "GUI/EventArgs.h"
#include "GUI/EventSet.h"
#include <string>

class BookCoverLoadedArgs: public EventArgs
{
public:
    BookCoverLoadedArgs()
    {
    }
    virtual ~BookCoverLoadedArgs()
    {
    }
    virtual EventArgs* Clone() const
    {
        return new BookCoverLoadedArgs(*this);
    }
    std::string bookPath;
    std::string coverPath;
};

class BookCoverLoader: public EventSet
{
private:
    BookCoverLoader();
public:
    static BookCoverLoader* GetInstance();
    static const char* EventBookCoverLoaded;
    void AddBook(const char* book);
    void AddBook(const std::string& book);
    void Start();
    void SetMinimumCoverSize(int width, int height)
         {m_minCoverWidth = width; m_minCoverHeight = height;}
    int  GetMinimumCoverWidth() {return m_minCoverWidth;}
    int  GetMinimumCoverHeight() {return m_minCoverHeight;}
    bool IsValidCoverImage(const char* imageFile);

private:
    static void* ThreadFunc(void* arg);
    void FireBookCoverLoadedEvent(const char* bookPath, const char* coverPath);
    void FireBookCoverLoadedEvent(const std::string& bookPath, const std::string& coverPath)
    {
        FireBookCoverLoadedEvent(bookPath.c_str(), coverPath.c_str());
    }
    // disable copy constructor and assign
    BookCoverLoader(const BookCoverLoader&);
    BookCoverLoader& operator=(const BookCoverLoader&);

private:
    int m_minCoverWidth;
    int m_minCoverHeight;
    dk::common::LockedStack<std::string> m_toLoadBooks;

};
#endif

