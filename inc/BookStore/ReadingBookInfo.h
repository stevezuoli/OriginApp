#ifndef __BOOKSTORE_READINGFBOOKINFO_H__
#define __BOOKSTORE_READINGFBOOKINFO_H__

#include "XMLDomNode.h"
#include "DKXManager/BaseType/CT_RefPos.h"
#include <string>

class ICT_ReadingDataItem;

namespace dk
{
namespace bookstore
{
namespace sync
{
enum SyncResultCode
{
    SRC_SUCCEEDED = 0,
    SRC_LACK_ARGS = 1,
    SRC_ARGS_EMPRY = 2,
    SRC_NO_LOGIN = 3,
    SRC_INVALID_BOOKID = 4,
    SRC_NO_READING_DATA = 5,
    SRC_INPUT_TYPE_ERROR = 6,
    SRC_CONFLICT = 7,
    SRC_LATEST_ALREADY = 8,
    SRC_UNMATCHED_BOOKIDS_VERSIONS = 9,
    SRC_DB_CONNECT_ERROR = 10

};

class SyncResult
{
public:
    SyncResult()
        : m_code(-1)
        , m_latestVersion(-1)
    {
    }
    void InitFromDomNode(const XMLDomNode* domNode);

    bool HasResultCode() const
    {
        return -1 != m_code;
    }
    SyncResultCode GetResultCode() const
    {
        return (SyncResultCode)m_code;
    }
    int GetLatestVersion() const
    {
        return m_latestVersion;
    }
    std::string GetBookId() const
    {
        return m_bookId;
    }
    std::string GetMessage() const
    {
        return m_message;
    }

private:
    int m_code;
    std::string m_message;
    std::string m_bookId;
    int m_latestVersion;
};

class ReadingDataItem
{
public:
    ReadingDataItem()
    {
    }
    ReadingDataItem(const XMLDomNode* domNode);
    enum Type
    {
        BOOKMARK,
        PROGRESS,
        COMMENT,
        UNKNOWN
    };

    bool WriteToDomNode(XMLDomNode* domNode) const;

    bool IsValid() const
    {
        switch (m_type)
        {
            case BOOKMARK:
            case PROGRESS:
            case COMMENT:
                return true;
            default:
                return false;
        }
    }
    Type GetType() const
    {
        return m_type;
    }
    void SetType(Type type)
    {
        m_type = type;
    }

    // 是否书摘,没有用户内容即是书摘
    bool IsDigest() const
    {
        return COMMENT == GetType() && !HasContent();
    }
    std::string GetDataId() const
    {
        return m_dataId;
    }
    void SetDataId(const char* dataId) 
    {
        m_dataId = dataId;
    }
    std::string GetCreateTime() const
    {
        return m_createTime;
    }
    void SetCreateTime(const char* createTime)
    {
        m_createTime = createTime;
    }

    std::string GetLastModifyTime() const
    {
        return m_lastModifyTime;
    }
    void SetLastModifyTime(const char* lastModifyTime)
    {
        m_lastModifyTime = lastModifyTime;
    }

    std::string GetRefContent() const
    {
        return m_refContent;
    }
    void SetRefContent(const char* refContent)
    {
        m_refContent = refContent;
    }

    bool HasContent() const
    {
        return !m_content.empty();
    }
    std::string GetContent() const
    {
        return m_content;
    }
    void SetContent(const char* content)
    {
        m_content = content;
    }

    const CT_RefPos& GetRefPos() const
    {
        return m_refPos;
    }
    void SetRefPos(const CT_RefPos& refPos)
    {
        m_refPos = refPos;
    }

    const CT_RefPos& GetBeginRefPos() const
    {
        return m_beginRefPos;
    }
    void SetBeginRefPos(const CT_RefPos& refPos)
    {
        m_beginRefPos = refPos;
    }

    const CT_RefPos& GetEndRefPos() const
    {
        return m_endRefPos;
    }
    void SetEndRefPos(const CT_RefPos& refPos)
    {
        m_endRefPos = refPos;
    }
    std::string GetColor() const
    {
        return m_color;
    }
    void SetColor(const char* color)
    {
        if (color != NULL)
        {
            m_color = color;
        }
    }
    std::string GetTag() const
    {
        return m_tag;
    }
    void SetTag(const char* tag)
    {
        if (tag != NULL)
        {
            m_tag = tag;
        }
    }

    void SetChapterTitle(const std::string& title)
    {
        m_chapterTitle = title;
    }
    std::string GetChapterTitle() const
    {
        return m_chapterTitle;
    }

private:
    static Type StringToType(const char* typeString);
    static std::string TypeToString(Type type);
    static bool AddRefPosFunc(const XMLDomNode* node, void* arg);
    static bool AddBeginRefPosFunc(const XMLDomNode* node, void* arg);
    static bool AddEndRefPosFunc(const XMLDomNode* node, void* arg);
    static bool WriteBeginRefPosFunc(XMLDomNode* node, const void* arg);
    static bool WriteEndRefPosFunc(XMLDomNode* node, const void* arg);
    static bool WriteRefPosFunc(XMLDomNode* node, const void* arg);
    Type m_type;
    std::string m_dataId;
    std::string m_createTime;
    std::string m_lastModifyTime;
    std::string m_refContent;
    std::string m_content;
    std::string m_color;
    std::string m_tag;
    std::string m_chapterTitle;
    CT_RefPos m_refPos;
    CT_RefPos m_beginRefPos;
    CT_RefPos m_endRefPos;
};

class ReadingBookInfo
{
public:
    static const char* CURRENT_VERSION;
    ReadingBookInfo();
    void InitFromDomNode(const XMLDomNode* domNode);
    bool WriteToDomNode(XMLDomNode* domNode) const;
    std::string GetVersion() const
    {
        return m_version;
    }
    void SetVersion(const char* version)
    {
        m_version = version;
    }
    std::string GetBookId() const
    {
        return m_bookId;
    }
    void SetBookId(const char* bookId)
    {
        m_bookId = bookId;
    }
    std::string GetBookRevision() const
    {
        return m_bookRevision;
    }
    void SetBookRevision(const char* bookRevision)
    {
        m_bookRevision = bookRevision;
    }
    std::string GetKernelVersion() const
    {
        return m_kernelVersion;
    }
    void SetKernelVersion(const char* kernelVersion)
    {
        m_kernelVersion = kernelVersion;
    }
	std::string GetDeviceId() const
	{
		return m_deviceId;
	}
	void SetDeviceId(const char* deviceId)
	{
		m_deviceId = deviceId;
	}
    size_t GetReadingDataItemCount() const
    {
        return m_readingDataItems.size();
    }

    const ReadingDataItem& GetReadingDataItem(size_t index) const
    {
        return m_readingDataItems[index];
    }

	void UpdateItemPos(size_t index, const CT_RefPos& beginPos, const CT_RefPos& endPos, const CT_RefPos& refPos)
	{
		m_readingDataItems[index].SetRefPos(refPos);
		m_readingDataItems[index].SetBeginRefPos(beginPos);
		m_readingDataItems[index].SetEndRefPos(endPos); 
	}

    bool HasSyncResult() const
    {
        return m_hasSyncResult;
    }
    const SyncResult& GetSyncResult() const
    {
        return m_syncResult;
    }
    void AddReadingDataItem(const ReadingDataItem& readingDataItem)
    {
        m_readingDataItems.push_back(readingDataItem);
    }
    void RemoveReadingDataItem(int index);
    void ClearReadingCommentDataItem();
    std::string ToPostData() const;

private:
    static bool AddReadingDataFunc(const XMLDomNode* node, void* arg);
    static bool AddReadingDataItemFunc(const XMLDomNode* node, void* arg);
    static bool ParseResultFunc(const XMLDomNode* node, void* arg);
    static bool WriteReadingDataFunc(XMLDomNode* node, const void* arg);
    bool AddReadingData(const XMLDomNode* node);

    std::string m_version;
    std::string m_bookId;
    std::string m_bookRevision;
    std::string m_kernelVersion;
	std::string m_deviceId;
    std::vector<ReadingDataItem> m_readingDataItems;
    bool m_hasSyncResult;
    SyncResult m_syncResult;
};

class ReadingProgressGetResult
{
public:
    ReadingProgressGetResult();
    void InitFromDomNode(const XMLDomNode* domNode);
    bool HasSyncResult() const
    {
        return m_hasSyncResult;
    }
    const SyncResult& GetSyncResult() const
    {
        return m_syncResult;
    }
    const ReadingBookInfo& GetReadingProgress() const
    {
        return m_readingProgress;
    }
    bool HasReadingProgress() const
    {
        return m_hasReadingProgress;
    }
private:
    static bool ParseResultFunc(const XMLDomNode* domNode, void* arg);
    static bool ParseProgressFunc(const XMLDomNode* domNode, void* arg);
    SyncResult m_syncResult;
    bool m_hasSyncResult;
    ReadingBookInfo m_readingProgress;
    bool m_hasReadingProgress;
};

class ReadingDataSyncResult
{
public:
    ReadingDataSyncResult();
    void InitFromDomNode(const XMLDomNode* domNode);
    bool HasSyncResult() const
    {
        return m_hasSyncResult;
    }
    const SyncResult& GetSyncResult() const
    {
        return m_syncResult;
    }
    const ReadingBookInfo& GetReadingData() const
    {
        return m_readingData;
    }
    bool HasReadingData() const
    {
        return m_hasReadingData;
    }
private:
    static bool ParseResultFunc(const XMLDomNode* domNode, void* arg);
    static bool ParseReadingDataFunc(const XMLDomNode* domNode, void* arg);
    SyncResult m_syncResult;
    bool m_hasSyncResult;
    ReadingBookInfo m_readingData;
    bool m_hasReadingData;
};

ICT_ReadingDataItem* CreateICTReadingDataItemFromReadingDataItem(
        const ReadingDataItem& readingDataItem);

ReadingDataItem* CreateReadingDataItemFromICTReadingDataItem(
        const ICT_ReadingDataItem& localReadingDataItem);

bool IsEqual(const ICT_ReadingDataItem& localItem, const ReadingDataItem& readingDataItem);


} // namespace sync
} // namespace bookstore
} // namespace dk
#endif
