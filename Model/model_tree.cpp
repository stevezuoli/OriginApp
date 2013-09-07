#include "Model/model_tree.h"
#include "Model/local_filesystem_tree.h"
#include "Model/cloud_filesystem_tree.h"

namespace dk {

namespace document_model {

const char* ModelTree::EventFileSystemChanged = "EventFileSystemChanged";
const char* ModelTree::EventCoverLoaded = "EventCoverLoaded";

ModelTree::ModelTree()
    : current_node_(0)
{
}

ModelTree::~ModelTree()
{
}

ModelTree* ModelTree::getModelTree(ModelType tree_type)
{
    switch (tree_type)
    {
    case MODEL_LOCAL_FILESYSTEM:
        {
            static LocalFileSystemTree local_filesystem_tree;
            return &local_filesystem_tree;
        }
        break;
    case MODEL_MICLOUD:
        {
            static CloudFileSystemTree cloud_filesystem_tree;
            return &cloud_filesystem_tree;
        }
        break;
    default:
        break;
    }
    return 0;
}

};

};