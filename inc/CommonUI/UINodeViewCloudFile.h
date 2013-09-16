#ifndef __COMMONUI_UINODEVIEW_CLOUD_FILE_H__
#define __COMMONUI_UINODEVIEW_CLOUD_FILE_H__

#include "CommonUI/IUINodeView.h"
#include "GUI/UIImage.h"
#include "GUI/UITouchComplexButton.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIProgressBar.h"
#include "CommonUI/UICoverViewItem.h"
#include "CommonUI/UIBookListProgressBar.h"

#include "Model/node.h"
#include "Model/model_tree.h"

using namespace dk::document_model;

class UISizer;
class UISizerItem;
class CDKFile;

class UINodeViewCloudFile : public IUINodeView
{
public:
    UINodeViewCloudFile(UIModelView* pParent,
                        ModelTree* model_tree,
                        BookListUsage usage,
                        int iconWidth,
                        int iconHeight);
    virtual ~UINodeViewCloudFile();

    virtual LPCSTR GetClassName() const
    {
        return "UINodeViewCloudFile";
    }

    virtual void SetModelDisplayMode(ModelDisplayMode mode);

    virtual UISizer* getListViewSizer();
    virtual void update();
    virtual void execute(unsigned long command_id);
    virtual void handleClicked();
    virtual std::vector<int> getSupportedNodeTypes();
    virtual bool OnChildClick(UIWindow* child);
protected:
    virtual bool onLoadingFinishedAction(const EventArgs& args);

private:
    bool deleteFile();
    bool openFile();
    bool shareToWeibo();
    bool download();

    void updateWndStauts();
    void updateWndVisible();
    void Init();

    static int PageCountToProgressBarDotCount(int pageCount);

private:
    UIImage m_imgItem;
    UIImage m_imgNewBook;
    UITouchComplexButton m_btnDownloadOrRead;

    UITextSingleLine m_fileSpace;
    UITextSingleLine m_uploadTime;
    bool m_downloading;
};

#endif

