#ifndef __COMMONUI_UICOVERVIEWITEM_H__
#define __COMMONUI_UICOVERVIEWITEM_H__

#include "GUI/UIContainer.h"
#include "GUI/UIImage.h"
#include "GUI/UIText.h"
#include "GUI/UITextSingleLine.h"
#include "Common/File_DK.h"
#include "GUI/UIImageTransparent.h"
#include "Model/node.h"
#include "Model/model_tree.h"

using namespace dk::document_model;

class UICoverViewItem: public UIContainer
{
public:
    UICoverViewItem(UIContainer* parent,
                    ModelTree* model_tree,
                    BookListUsage usage,
                    int minWidth,
                    int minHeight,
                    int overallWidth,
                    int overallHeight);
    void SetIsDuoKanBook(bool duoKanBook);
    void SetBookFormat(DkFileFormat format);
    void SetIsDir(bool isDir);
    void SetDirNameOrBookName(const char* title);
    void SetDirNameOrBookName(const std::string& title)
    {
        return SetDirNameOrBookName(title.c_str());
    }

    void SetDirBookNumOrBookAuthor(const char* author);
    void SetDirBookNumOrBookAuthor(const std::string& author)
    {
        return SetDirBookNumOrBookAuthor(author.c_str());
    }

    void SetProgress(const char* progressStr);
    virtual HRESULT Draw(DK_IMAGE drawingImg); 
    virtual void MoveWindow(int left, int top, int width, int height);
    void SetCoverImage(const char* coverFile);
    void SetCoverImage(const std::string& coverFile)
    {
        SetCoverImage(coverFile.c_str());
    }
    void SetSelected(bool selected);
    void SetIsTrial(bool isTrial);
    void SetIsNew(bool isNew);
    void SetIsLoading(bool isLoading);
    void SetBookPath(const char* bookPath);
    void SetBookPath(const std::string& bookPath)
    {
        SetBookPath(bookPath.c_str());
    }
#ifdef KINDLE_FOR_TOUCH
    void SetHighlight(bool highlight);
#endif
    bool OnCoverLoadeded(const EventArgs& args);

    BookListUsage usage() const { return m_usage; }

private:
#ifdef KINDLE_FOR_TOUCH
    bool IsHighlight() const;
    void DrawTouchBorder();
#endif
    bool IsDuoKanBook() const;
    bool IsDir() const;
    bool IsLoading() const;
    void DrawProgress();
    void InitUI();
    bool IsSelected() const;
    bool IsTrial() const;
    bool IsNew() const;
    void UpdateSelectImage();
    void UpdateFormatText();
    void DrawShaddowAndBorder(DK_IMAGE imgSelf);
    void DrawCover(DK_IMAGE imgSelf);
    void DrawTrial(DK_IMAGE imgSelf);
    void DrawFormat(DK_IMAGE imgSelf);
    //void DrawProgress(DK_IMAGE imgSelf);
    void DrawSelect(DK_IMAGE imgSelf);
    void DrawBorder(DK_IMAGE imgSelf);
    void DrawBottomTitle(DK_IMAGE imgSelf);
    void DrawBottomProgress(DK_IMAGE imgSelf);

private:
    int GetLeftExtra() const;
    int GetTopExtra() const;
    int GetCoverTop() const;
    int GetCoverLeft() const;
    int GetCoverWidth() const;
    int GetCoverHeight() const;

private:
    UIImage m_imgCover;
    UIImageTransparent m_imgFormat;
    UIImage m_imgNew;
    UIImageTransparent m_imgTrial;
    UIImageTransparent m_imgSelect;
    int m_shaddowLeft;
    int m_shaddowTop;
    int m_shaddowWidth;
    int m_shaddowHeight;
    DkFileFormat m_fileFormat;
    bool m_isDuoKanBook;
    bool m_isDir;
    bool m_selected;
    bool m_isTrial;
    bool m_isNew;
    BookListUsage m_usage;
    std::string m_coverFile;
    std::string m_bookPath;
    int m_innerLeft;
    int m_innerTop;
    int m_innerWidth;
    int m_innerHeight;
    int m_overallWidth;
    int m_overallHeight;
    UIText m_txtDirNameOrBookName;
    UITextSingleLine m_txtDirBookNumOrBookAuthor;
    UIText m_txtItemNameLine;
    UITextSingleLine m_txtReadingProgressLine;
    bool m_isLoading;
    bool m_highlight;
};

#endif
