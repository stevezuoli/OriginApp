#include <algorithm>
#include <map>

#include "Model/local_bookstore_category_node.h"
#include "Model/local_doc_node.h"

#include "Utility/PathManager.h"
#include "Utility/FileSystem.h"
#include "BookStore/LocalCategoryManager.h"

using namespace dk::utility;
using namespace dk::bookstore;

namespace dk {

namespace document_model {

LocalBookStoreCategoryNode::LocalBookStoreCategoryNode(Node * p, const string & category_name)
    : ContainerNode(p)
    , virtual_folder_(true)
{
    mutableAbsolutePath() = category_name;
    mutableType() = NODE_TYPE_CATEGORY_VIRTUAL_BOOK_STORE;
    mutableName() = category_name;  // Utf8 name
    mutableDisplayName() = category_name;
}

LocalBookStoreCategoryNode::~LocalBookStoreCategoryNode()
{
    DeletePtrContainer(&children_);
}

NodePtrs& LocalBookStoreCategoryNode::updateChildrenInfo()
{
    for (NodePtrsIter iter = children_.begin(); iter != children_.end(); ++iter)
    {
        // Update information if it's file node.
        if ((*iter)->type() == NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK)
        {
            dynamic_cast<FileNode *>((*iter).get())->update();
        }
    }
    sort(children_, by_field_, sort_order_);
    return children_;
}

bool LocalBookStoreCategoryNode::updateChildren(int status_filter)
{
    DeletePtrContainer(&children_);
    name_filters_.clear();
    scan(absolutePath(), children_, status_filter, true);
    dirty_ = false;

    NodeChildenReadyArgs children_ready_args;
    children_ready_args.current_node_path = absolutePath();
    children_ready_args.succeeded = true;
    children_ready_args.children = children_;
    mutableRoot()->FireEvent(EventChildrenIsReady, children_ready_args);
    return true;
}

bool LocalBookStoreCategoryNode::testStatus(const string& path, int status_filter)
{
    if (status_filter == NODE_NONE)
    {
        return true;
    }
    if (status_filter & (NODE_SELF_OWN |
                         NODE_NOT_ON_LOCAL |
                         NODE_CLOUD |
                         NODE_IS_DOWNLOADING |
                         NODE_IS_UPLOADING))
    {
        return false;
    }
    return true;
}

void LocalBookStoreCategoryNode::scan(const string &cat_name,
                                      NodePtrs &result,
                                      int status_filter,
                                      bool sort_list)
{
    StringList current_books = LocalCategoryManager::GetBookIdsByCategory(cat_name);

    // TODO. Support sub directory

    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    for (DK_AUTO(cur, current_books.begin()); cur != current_books.end(); ++cur)
    {
        string file_name = *cur + ".epub";
        string full_path = PathManager::ConcatPath(absolutePath(), file_name);
        if (FileNode::testStatus(full_path, status_filter))
        {
            PCDKFile file = file_manager->GetFileByPath(full_path);
            if (file != 0)
            {
                NodePtr book_node(new FileNode(this, file));
                result.push_back(book_node);
            }
        }
    }

    // Sort.
    if (sort_list)
    {
        sort(result, by_field_, sort_order_);
    }
}

size_t LocalBookStoreCategoryNode::nodePosition(NodePtr node)
{
    // check
    if (node == 0)
    {
        return INVALID_ORDER;
    }

    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    const NodePtrs& nodes  = children(ret, false, statusFilter());
    NodePtrs::const_iterator it = find(nodes.begin(), nodes.end(), node);
    if (it == nodes.end())
    {
        return INVALID_ORDER;
    }
    else
    {
        return it - nodes.begin();
    }
}

/// Exactly match.
size_t LocalBookStoreCategoryNode::nodePosition(const string &name)
{
    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    const NodePtrs& all = children(ret, false, statusFilter());
    for(NodePtrs::const_iterator it = all.begin(); it != all.end(); ++it)
    {
        if ((*it)->name() == name)
        {
            return it - all.begin();
        }
    }
    return INVALID_ORDER;
}

/// Search from current directory by using the specified name filters.
/// Recursively search if needed.
bool LocalBookStoreCategoryNode::search(const StringList &name_filters,
                             bool recursive,
                             bool & stop)
{
    //name_filters_ = name_filters;
    //DeletePtrContainer(&children_);

    //// Search from current directory.
    //if (!recursive)
    //{
    //    scan(absolutePath(), name_filters_, children_, true);
    //    return true;
    //}

    //scan(absolutePath(), name_filters_, children_, false);

    //// Collect all directories.
    //StringList targets;
    //collectDirectories(absolutePath(), targets);

    //while (!targets.empty())
    //{
    //    StringList sub;
    //    for(int i = 0; i < targets.size(); ++i)
    //    {
    //        // Search the directory.
    //        NodePtrs tmp;
    //        //QDir dir(t);
    //        string dir = targets[i];
    //        scan(dir, name_filters_, tmp, false);
    //        children_.insert(children_.end(), tmp.begin(), tmp.end());
    //        collectDirectories(dir, sub);

    //        // Check if caller wants to stop.
    //        if (stop)
    //        {
    //            sub.clear();
    //            targets.clear();
    //            break;
    //        }
    //    }
    //    targets = sub;
    //}

    //if (stop)
    //{
    //    return false;
    //}

    //// Sort.
    //sort(children_, by_field_, sort_order_);
    //dirty_ = false;
    return false;
}

bool LocalBookStoreCategoryNode::rename(const string& new_name, string& error_msg)
{
    if (!LocalCategoryManager::RenameCategory(name().c_str(), new_name.c_str(), &error_msg))
    {
        return false;
    }
    CDKFileManager* fileManager = CDKFileManager::GetFileManager();
    fileManager->FireFileListChangedEvent();
    return true;
}

bool LocalBookStoreCategoryNode::remove(bool delete_local_files_if_possible)
{
    CDKFileManager* fileManager = CDKFileManager::GetFileManager();
    if (delete_local_files_if_possible)
    {
        DK_AUTO(ids, LocalCategoryManager::GetBookIdsByCategory(name()));
        for (DK_AUTO(id, ids.begin()); id != ids.end(); ++id)
        {
            std::string path = PathManager::BookIdToEpubFile(*id);
            fileManager->DelFile(path);
        }
    }
    LocalCategoryManager::RemoveCategory(name().c_str());
    fileManager->FireFileListChangedEvent();
    return true;
}

}  // namespace document_model

}  // namespace dk
