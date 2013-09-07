#ifndef DUOKAN_NODE_TYPES_H_
#define DUOKAN_NODE_TYPES_H_

#include "GUI/EventListener.h"
#include "GUI/EventSet.h"
#include "GUI/EventArgs.h"

namespace dk {

namespace document_model {

/// Define node type. Used by node type identification.
enum NodeType
{
    NODE_TYPE_NULL = -1,                 ///< Null node type.
    NODE_TYPE_LOCAL_DESKTOP,             ///< The unique root node.
    NODE_TYPE_CATEGORY_LOCAL_BOOK_STORE, ///< Root node of DK book store
    NODE_TYPE_CATEGORY_LOCAL_PUSHED,     ///< Root node of Pushed books
    NODE_TYPE_CATEGORY_LOCAL_DOCUMENTS,  ///< Root node of Local documents
    NODE_TYPE_CATEGORY_LOCAL_FOLDER,     ///< Folder.
    NODE_TYPE_CATEGORY_VIRTUAL_BOOK_STORE, ///< virtual category of book store's book
    NODE_TYPE_FILE_LOCAL_DOCUMENT,         ///< File content or unknown file format.
    NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK,  ///< Book from Duokan BookStore
    NODE_TYPE_BOOKSTORE_CATEGORY, ///< Category from Duokan BookStore
    NODE_TYPE_BOOKSTORE_BOOK,   ///< Book from Book Store
    NODE_TYPE_BOOKSTORE_DESKTOP, ///< Desktop of Book Store
    NODE_TYPE_MICLOUD_BOOK,     ///< Book from Mi cloud
    NODE_TYPE_MICLOUD_CATEGORY, ///< Category from Mi clouds
    NODE_TYPE_MICLOUD_DESKTOP,  ///< Desktop of Mi clouds
    NODE_TYPE_WEBSITE,          ///< A single web site.
    NODE_TYPE_GAME,             ///< Game container.
    NODE_TYPE_COUNT             ///< The last type, do not use it.
};

/// Define tree type.
enum ModelType
{
    MODEL_LOCAL_FILESYSTEM = 0,
    MODEL_BOOKSTORE,
    MODEL_MICLOUD
};

/// Define content types. Make sure it does not conflict with
/// node type.
enum ContentType
{
    CONTENT_START = NODE_TYPE_COUNT,
    CONTENT_BMP,
    CONTENT_CHM,
    CONTENT_DIRECTORY,
    CONTENT_EPUB,
    CONTENT_GIF,
    CONTENT_HTML,
    CONTENT_JPEG,
    CONTENT_MOBI,
    CONTENT_PDB,
    CONTENT_PDF,
    CONTENT_PNG,
    CONTENT_RTF,
    CONTENT_TIFF,
    CONTENT_TEXT,
    CONTENT_FB2,
    CONTENT_ZIP,
    CONTENT_MUSIC,
    CONTENT_DJVU,
    CONTENT_ABF,
    CONTENT_DOC,
    CONTENT_XLS,
    CONTENT_PPT,
    CONTENT_RUNNABLE,
    CONTENT_UNKNOWN,
    CONTENT_END
};

/// Node Status
enum NodeStatus
{
    NODE_NONE = 0x0000,
    NODE_LOCAL = 0x0001,
    NODE_NOT_ON_LOCAL = 0x0002,
    NODE_CLOUD = 0x0004,
    NODE_NOT_ON_CLOUD = 0x0008,
    NODE_PURCHASED = 0x0010,
    NODE_SELF_OWN = 0x0020,
    NODE_IS_TRIAL = 0x0040,
    NODE_IS_DOWNLOADING = 0x0080,
    NODE_IS_UPLOADING = 0x0100,
    NODE_ALL = NODE_LOCAL | NODE_NOT_ON_LOCAL | NODE_CLOUD | NODE_NOT_ON_CLOUD | NODE_PURCHASED | NODE_SELF_OWN | NODE_IS_TRIAL |
               NODE_IS_DOWNLOADING | NODE_IS_UPLOADING
};

enum Field
{
    FIELD_NONE = 0,                ///< Do not sort.
    LAST_ACCESS_TIME,
    BY_DIRECTORY,
    RECENTLY_ADD,
    NAME,
    TITLE,
    DESCRIPTION,
    SIZE,
    RATING,
    READ_COUNT,
    CREATE_TIME,
    NODE_TYPE,
};

enum DKDisplayMode
{
    BY_FOLDER = 0,
    BY_SORT,
    EXPAND_ALL,
};

enum SortOrder
{
    NO_ORDER = 0,
    ASCENDING,
    DESCENDING
};

// Release each pointer in a container, and clear the container
// itself. The pointer to the container is NOT deleted.
template<typename Container>
void DeletePtrContainer(Container* container) {
    if (0 == container) return;
    //for(typename Container::iterator i = container->begin();
    //    i != container->end();
    //    ++i) {
    //    delete *i;
    //}
    container->clear();
}

}  // namespace document_model

}  // namespace dk

#endif
