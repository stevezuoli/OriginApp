#include <algorithm>
#include <map>

#include "Model/local_bookstore_node.h"
#include "Model/local_doc_node.h"
#include "Model/local_bookstore_category_node.h"

#include "BookStore/LocalCategoryManager.h"
#include "Utility/FileSystem.h"
#include "Utility/SystemUtil.h"
#include "Utility/PathManager.h"
#include "Utility/StringUtil.h"
#include "Utility/CharUtil.h"
#include "Utility/ImageManager.h"
#include "I18n/StringManager.h"
#include "CommandID.h"

using namespace dk::utility;
using namespace dk::bookstore;

namespace dk {

namespace document_model {

LocalBookStoreNode::LocalBookStoreNode(Node * p, const string & folder_path)
    : ContainerNode(p)
    , virtual_folder_(false)
{
    mutableAbsolutePath() = folder_path;
    mutableType() = NODE_TYPE_CATEGORY_LOCAL_BOOK_STORE;
    mutableName() = nodeName(folder_path);
    mutableDisplayName() = nodeDisplayName(folder_path);
    mutableGbkName() = EncodeUtil::UTF8ToGBKString(displayName());
    mutableCoverPath() = ImageManager::GetImagePath(IMAGE_ICON_COVER_DUOKAN_DIR);
}

LocalBookStoreNode::~LocalBookStoreNode()
{
}

SPtr<ITpImage> LocalBookStoreNode::getInitialImage()
{
    return ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_FOLDER);
}

NodePtrs LocalBookStoreNode::updateChildrenInfo()
{
    for(NodePtrsIter iter = children_.begin(); iter != children_.end(); ++iter)
    {
        // Update information if it's file node.
        if ((*iter)->type() == NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK)
        {
            dynamic_cast<FileNode *>((*iter).get())->update();
        }
    }
    return filterChildren(children_);
}

bool LocalBookStoreNode::updateChildren()
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

void LocalBookStoreNode::scan(const string &dir, NodePtrs &result)
{
    StringList current_cats = LocalCategoryManager::GetAllCategories();
    StringList current_books = LocalCategoryManager::GetBookIdsWithNoCategory();
    for (size_t i = 0; i < current_cats.size(); i++)
    {
        string cat_path = current_cats[i];
        //if (LocalBookStoreCategoryNode::testStatus(cat_path, status_filter))
        {
            NodePtr folder_node(new LocalBookStoreCategoryNode(this, cat_path));
            result.push_back(folder_node);
        }
    }

    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    for (DK_AUTO(cur, current_books.begin()); cur != current_books.end(); ++cur)
    {
        string file_name = *cur + ".epub";
        string full_path = PathManager::ConcatPath(absolutePath(), file_name);
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

void LocalBookStoreNode::collectDirectories(StringList & result)
{
    result = LocalCategoryManager::GetAllCategories();
}

bool LocalBookStoreNode::testStatus(const string& path, int status_filter)
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

bool LocalBookStoreNode::supportedCommands(std::vector<int>& command_ids,
                                           std::vector<int>& str_ids)
{
    command_ids.clear();
    str_ids.clear();
    return true;
}

}  // namespace document_model

}  // namespace dk
