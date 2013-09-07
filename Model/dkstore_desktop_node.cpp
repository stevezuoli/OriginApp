#include "Model/dkstore_desktop_node.h"
#include "Model/dkstore_category_node.h"
#include "Model/dkstore_book_node.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/DownloadTaskBuilder.h"

using namespace std;
using namespace dk::bookstore;

namespace dk {

namespace document_model {

const char* BookStoreDesktopNode::DKSTORE_DESKTOP_PATH = "dkstore_desktop";

BookStoreDesktopNode::BookStoreDesktopNode(Node * parent)
    : ContainerNode(parent)
    , current_display_mode_(BY_FOLDER)
{
    mutableType() = NODE_TYPE_BOOKSTORE_DESKTOP;
    mutableAbsolutePath() = DKSTORE_DESKTOP_PATH;
    SubscribeMessageEvent(
        BookStoreInfoManager::EventCategoryTreeUpdate,
        *BookStoreInfoManager::GetInstance(),
            std::tr1::bind(
                std::tr1::mem_fn(&BookStoreDesktopNode::onCategoryTreeUpdated),
                this,
                std::tr1::placeholders::_1));

    SubscribeMessageEvent(BookStoreInfoManager::EventSearchBookUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&BookStoreDesktopNode::onSearchUpdated),
        this,
        std::tr1::placeholders::_1)
        );
}

BookStoreDesktopNode::~BookStoreDesktopNode()
{
    DeletePtrContainer(&children_);
}

NodePtrs& BookStoreDesktopNode::updateChildrenInfo()
{
    for (NodePtrsIter iter = children_.begin(); iter != children_.end(); ++iter)
    {
        // Update information if it's file node.
        if ((*iter)->type() == NODE_TYPE_BOOKSTORE_BOOK)
        {
            dynamic_cast<FileNode *>(*iter)->update();
        }
    }
    sort(children_, by_field_, sort_order_);
    return children_;
}

void BookStoreDesktopNode::search(const string& keyword, int status_filter, int start, int num)
{
    mutableStatusFilter() = status_filter;
    current_display_mode_ = BY_SORT;
    DeletePtrContainer(&children_);
    BookStoreInfoManager::GetInstance()->FetchSearchBookList(keyword.c_str(), start, num);
}

void BookStoreDesktopNode::updateChildren(int status_filter)
{
    if (current_display_mode_ == BY_FOLDER)
    {
        DeletePtrContainer(&children_);
        scan(absolutePath(), children_, status_filter, true);
    }
}

void BookStoreDesktopNode::scan(const string& dir, NodePtrs &result, int status_filter, bool sort_list)
{
    mutableStatusFilter() = status_filter;
    // fetch the category tree from bookstore.
    // the caller should wait until results returns and Nodes are ready
    BookStoreInfoManager::GetInstance()->FetchCategoryTree();
}

bool BookStoreDesktopNode::onCategoryTreeUpdated(const EventArgs& args)
{
    const CategoryTreeArgs& category_result = dynamic_cast<const CategoryTreeArgs&>(args);
    if (category_result.succeeded)
    {
        for (size_t i = 0; i < category_result.dataList.size(); ++i)
        {
            model::CategoryInfo* first_level_cat = dynamic_cast<model::CategoryInfo*>(category_result.dataList[i].get());
            if (first_level_cat != 0 &&
                BookStoreCategoryNode::testStatus(first_level_cat, statusFilter()))
            {
                // create a category node
                NodePtr cat_node = new BookStoreCategoryNode(this, first_level_cat);
                children_.push_back(cat_node);
            }
        }
        fireChildrenReadyEvent(absolutePath(), children_);
    }
    return false;
}

bool BookStoreDesktopNode::onSearchUpdated(const EventArgs& args)
{
    const SearchBookUpdateArgs& search_args = dynamic_cast<const SearchBookUpdateArgs&>(args);
    if (search_args.succeeded)
    {
        model::BookInfoList results = model::BookInfo::FromBasicObjectList(search_args.dataList);
        for (std::vector<model::BookInfoSPtr>::iterator itr = results.begin(); itr != results.end(); itr++)
        {
            if ((*itr) != 0 && DKStoreBook::testStatus(*itr, statusFilter()))
            {
                // create a book node
                NodePtr book_node = new DKStoreBook(this, *itr);
                children_.push_back(book_node);
            }
        }
        fireChildrenReadyEvent(absolutePath(), children_);
    }
}

}

}
