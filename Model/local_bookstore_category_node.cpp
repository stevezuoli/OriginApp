#include <algorithm>
#include <map>

#include "Model/local_bookstore_category_node.h"
#include "Model/local_doc_node.h"

#include "Utility/PathManager.h"
#include "Utility/FileSystem.h"
#include "BookStore/LocalCategoryManager.h"
#include "I18n/StringManager.h"
#include "CommandID.h"

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
    mutableGbkName() = EncodeUtil::UTF8ToGBKString(displayName());
}

LocalBookStoreCategoryNode::~LocalBookStoreCategoryNode()
{
}

NodePtrs LocalBookStoreCategoryNode::updateChildrenInfo()
{
    for (NodePtrsIter iter = children_.begin(); iter != children_.end(); ++iter)
    {
        // Update information if it's file node.
        if ((*iter)->type() == NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK)
        {
            dynamic_cast<FileNode *>((*iter).get())->update();
        }
    }
    return filterChildren(children_);
}

bool LocalBookStoreCategoryNode::updateChildren()
{
    clearChildren();
    scan(absolutePath(), children_);
    setDirty(false);

    NodeChildenReadyArgs children_ready_args;
    children_ready_args.current_node_path = absolutePath();
    children_ready_args.succeeded = true;
    children_ready_args.children = filterChildren(children_);
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

void LocalBookStoreCategoryNode::scan(const string &cat_name, NodePtrs &result)
{
    StringList current_books = LocalCategoryManager::GetBookIdsByCategory(cat_name);

    // TODO. Support sub directory

    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    for (DK_AUTO(cur, current_books.begin()); cur != current_books.end(); ++cur)
    {
        string file_name = *cur + ".epub";

        // NODE. LocalBookStoreCategoryNode is just a virtual node so abosolute path is just a name
        // use parent(Book Store Root Node) path as the real path
        string full_path = PathManager::ConcatPath(parent()->absolutePath(), file_name);
        //if (FileNode::testStatus(full_path, status_filter))
        {
            PCDKFile file = file_manager->GetFileByPath(full_path);
            if (file != 0)
            {
                NodePtr book_node(new FileNode(this, file));
                result.push_back(book_node);
            }
        }
    }
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

bool LocalBookStoreCategoryNode::remove(bool delete_local_files_if_possible, bool exec_now)
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

bool LocalBookStoreCategoryNode::supportedCommands(std::vector<int>& command_ids,
                                                   std::vector<int>& str_ids)
{
    command_ids.clear();
    command_ids.push_back(ID_BTN_ADD_FILES_TO_CATEGORY);
    command_ids.push_back(ID_BTN_RENAME_CATEGORY);
    command_ids.push_back(ID_BTN_DELETE_CATEGORY);
    command_ids.push_back(ID_INVALID);

    str_ids.clear();
    str_ids.push_back(CATEGORY_ADD_FILE);
    str_ids.push_back(CATEGORY_RENAME_BUTTON);
    str_ids.push_back(BOOK_DELETE);
    str_ids.push_back(-1);
    return true;
}

bool LocalBookStoreCategoryNode::satisfy(int status_filter)
{
    if (status_filter & NODE_DUOKAN_BOOK_NOT_CLASSIFIED)
    {
        // Do NOT display category on select mode.
        return false;
    }
    return Node::satisfy(status_filter);
}

}  // namespace document_model

}  // namespace dk
