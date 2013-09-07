#ifndef DUOKAN_BOOKSTORE_BOOK_NODE_H_
#define DUOKAN_BOOKSTORE_BOOK_NODE_H_

#include "Model/node.h"
#include "BookStore/BookInfo.h"

using namespace dk::bookstore::model;

namespace dk {

namespace document_model {

/// File Node implement. The file node can contain
/// another virtual file system.
class DKStoreBook : public Node
{
public:
    DKStoreBook(Node * parent, BookInfoSPtr book_info);
    ~DKStoreBook();

public:
    size_t fileSize() const { return size_; }
    void update();
    BookInfoSPtr bookInfo() { return book_info_; }

    static bool testStatus(BookInfoSPtr book_info, int status_filter);

private:
    enum MetaDataState
    {
        MD_INVALID = -1,
        MD_TOSCAN,              ///< Need to scan.
        MD_SCANNED              ///< Alaredy scanned.
    };

private:
    bool onCoverLoaded(const EventArgs& args);

protected:
    MetaDataState data_state_;
    BookInfoSPtr book_info_;
    size_t size_;
};

}  // namespace document_model

}  // namespce dk

#endif
