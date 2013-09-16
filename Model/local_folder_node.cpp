#include <algorithm>
#include <map>

#include "Model/local_folder_node.h"

#include "Utility/PathManager.h"
#include "Utility/FileSystem.h"
#include "Utility/ImageManager.h"
#include "Utility/SystemUtil.h"
#include "Utility/StringUtil.h"
#include "Utility/CharUtil.h"
#include "I18n/StringManager.h"
#include "XiaoMi/XiaoMiServiceFactory.h"
#include "XiaoMi/MiCloudService.h"
#include "I18n/StringManager.h"
#include "CommandID.h"

using namespace dk::utility;
using namespace xiaomi;

namespace dk {

namespace document_model {

LocalFolderNode::LocalFolderNode(Node * p, const string & folder_path)
    : ContainerNode(p)
    , virtual_folder_(false)
{
    status_ = NODE_LOCAL | NODE_SELF_OWN; // update status for cloud later
    mutableAbsolutePath() = folder_path;
    mutableType() = NODE_TYPE_CATEGORY_LOCAL_FOLDER;
    mutableName() = nodeName(folder_path);
    mutableDisplayName() = nodeDisplayName(folder_path);
    mutableGbkName() = EncodeUtil::UTF8ToGBKString(displayName());
    mutableCoverPath() = ImageManager::GetImagePath(type() == NODE_TYPE_CATEGORY_LOCAL_PUSHED 
                            ? IMAGE_ICON_COVER_DUOKAN_DIR
                            : IMAGE_ICON_COVER_DIR);
}

LocalFolderNode::~LocalFolderNode()
{
}

SPtr<ITpImage> LocalFolderNode::getInitialImage()
{
    return ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_FOLDER);
}

NodePtrs LocalFolderNode::updateChildrenInfo()
{
    for(NodePtrsIter iter = children_.begin(); iter != children_.end(); ++iter)
    {
        // Update information if it's file node.
        if ((*iter)->type() == NODE_TYPE_FILE_LOCAL_DOCUMENT)
        {
            dynamic_cast<FileNode *>((*iter).get())->update();
        }
    }
    return filterChildren(children_);
}

bool LocalFolderNode::updateChildren()
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

void LocalFolderNode::scan(const string& dir, NodePtrs &result)
{
    StringList current_dirs = PathManager::GetDirsInPath(dir.c_str());
    StringList current_books = PathManager::GetFilesInPath(dir.c_str());

    for (size_t i = 0; i < current_dirs.size(); i++)
    {
        string folder_path = PathManager::ConcatPath(dir.c_str(), current_dirs[i].c_str());
        //if (LocalFolderNode::testStatus(folder_path, status_filter))
        {
            NodePtr folder_node(new LocalFolderNode(this, folder_path));
            result.push_back(folder_node);
        }
    }

    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    for (size_t i = 0; i < current_books.size(); i++)
    {
        string book_path = PathManager::ConcatPath(dir.c_str(), current_books[i].c_str());
        //if (FileNode::testStatus(book_path, status_filter))
        {
            PCDKFile file = file_manager->GetFileByPath(book_path);
            if (file != 0)
            {
                NodePtr book_node(new FileNode(this, file));
                result.push_back(book_node);
            }
        }
    }
}

void LocalFolderNode::collectDirectories(const string &path, StringList & result)
{
    result.clear();
    StringList sub_dir_names = PathManager::GetDirsInPath(path.c_str());
    for (size_t i = 0; i < sub_dir_names.size(); i++)
    {
        string folder_path = PathManager::ConcatPath(path.c_str(), sub_dir_names[i].c_str());
        result.push_back(folder_path);
    }
}

bool LocalFolderNode::testStatus(const string& path, int status_filter)
{
    // TODO. Filter folder in the future. Currently no filtering is needed
    return true;
}

/// Not use now
void LocalFolderNode::createCloudDirectory()
{
    string cloud_folder_path = cloudFilePath();
    XiaoMiServiceFactory::GetMiCloudService()->CreateDirectory(cloud_folder_path);
}

/// Not use now
// the file_path has already been checked in caller
void LocalFolderNode::onCreateCloudDirectoryFinished(const EventArgs& args)
{
    const DirectoryCreatedArgs& create_dir_args = dynamic_cast<const DirectoryCreatedArgs&>(args);
    if (create_dir_args.succeeded && create_dir_args.result != 0)
    {
        int stat = status();
        cloud_file_info_ = create_dir_args.result->GetDirInfo();
        mutableId() = cloud_file_info_->id;
        stat |= NODE_CLOUD;
        setStatus(stat);
    }

    // TODO. Notify caller directory has been created
    // TODO. Error handling?
    if (upload_ctx_.isActive())
    {
        upload();
    }
}

/// Not use now
void LocalFolderNode::fetchInfo()
{
    string cloud_folder_path = cloudFilePath();
    XiaoMiServiceFactory::GetMiCloudService()->GetInfo(cloud_folder_path);
}

/// Not use now
void LocalFolderNode::onInfoReturned(const EventArgs& args)
{
    const GetInfoArgs& get_info_args = dynamic_cast<const GetInfoArgs&>(args);
    if (get_info_args.succeeded && get_info_args.result != 0)
    {
        int stat = status();
        cloud_file_info_ = get_info_args.result->GetFileInfo();
        mutableId() = cloud_file_info_->id;
        stat |= NODE_CLOUD;
        setStatus(stat);
    }

    // TODO. Error handling
}

void LocalFolderNode::upload(bool exec_now)
{
    // take all of the children into upload waiting queue because we don't know
    // whether the posterities are selected
    //NodePtrs waiting_children = children(false, NODE_LOCAL);
    //upload_ctx_.reset(waiting_children);

    // TODO. Do not support creating directory now.
    // All of the books will be uploaded to first level folder of /books
    //if (selected())
    //{
    //    // make sure this folder has been setup on cloud
    //    if (!isOnCloud())
    //    {
    //        createCloudDirectory();
    //        return;
    //    }
    //}

    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    NodePtrs waiting_children = children(ret, false, statusFilter());
    if (ret == RETRIEVE_DONE)
    {
        for (size_t i = 0; i < waiting_children.size(); i++)
        {
            NodePtr child = waiting_children[i];
            NodeType child_type = child->type();
            if (child_type != NODE_TYPE_FILE_LOCAL_DOCUMENT &&
                child_type != NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK)
            {
                child->upload(exec_now);
            }
            else
            {
                if (child->selected())
                {
                    child->upload(exec_now);
                }
            }
        }
    }

    // reset selected status for this folder
    Node::upload(exec_now);
}

bool LocalFolderNode::isOnCloud()
{
    // TODO.
    return cloud_file_info_ != 0;
}

bool LocalFolderNode::rename(const string& new_name, string& error_msg)
{
    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    if(NULL == file_manager)
    {
        return false;
    }
    if (new_name.empty())
    {
        return false;
    }

    // get parent path
    Node* parent = mutableParent();
    if (parent == 0)
    {
        return false;
    }

    string parent_path = parent->absolutePath();
    string new_path = PathManager::ConcatPath(parent_path, new_name);

    NodePtrs& children = children_; //mutableChildren(result, false);
    for(size_t i = 0; i < children.size(); i++)
    {
        if (new_path == children[i]->absolutePath())
        {
            // same name
            error_msg = StringManager::GetPCSTRById(CATEGORY_EXISTING);
            return false;
        }
    }

    char sztmp[1024] = {0};    
    sprintf(sztmp,"mv \"%s\" \"%s\" ", absolutePath().c_str(), new_path.c_str());
    SystemUtil::ExecuteCommand(sztmp);
    file_manager->ReLoadFileToFileManger(SystemSettingInfo::GetInstance()->GetMTDPathLP(), true);
    file_manager->FireFileListChangedEvent();
    return true;
}

bool LocalFolderNode::remove(bool delete_local_files_if_possible, bool exec_now)
{
    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    if(0 == file_manager)
    {
        return false;
    }
    char sztmp[1024] = {0};
    sprintf(sztmp,"rm \"%s\" -rf", absolutePath().c_str());
    SystemUtil::ExecuteCommand(sztmp);
    file_manager->ReLoadFileToFileManger(SystemSettingInfo::GetInstance()->GetMTDPathLP(), true);

    // rescan
    file_manager->FireFileListChangedEvent();
    return true;
}

bool LocalFolderNode::supportedCommands(std::vector<int>& command_ids,
                                                   std::vector<int>& str_ids)
{
    command_ids.clear();
    command_ids.push_back(ID_BTN_OPEN_FOLDER);
    command_ids.push_back(ID_BTN_RENAME_FOLDER);
    command_ids.push_back(ID_BTN_DELETE_FOLDER);
    command_ids.push_back(ID_INVALID);

    str_ids.clear();
    str_ids.push_back(TOUCH_OPEN_FOLDER);
    str_ids.push_back(CATEGORY_RENAME_BUTTON);
    str_ids.push_back(TOUCH_DELETE_FOLDER);
    str_ids.push_back(-1);
    return true;
}

}  // namespace document_model

}  // namespace dk
