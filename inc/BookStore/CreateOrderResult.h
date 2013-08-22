#ifndef __BOOKSTORE_CREATEORDERRESULT_H__
#define __BOOKSTORE_CREATEORDERRESULT_H__

#include "BookStore/BasicObject.h"
#include "BookStore/BookStoreErrno.h"

namespace dk
{
namespace bookstore
{
namespace model
{

enum OrderStatus
{
    OS_UNKNOWN = 0,
    OS_UNPAID = 1,
    OS_PAID = 2,
    OS_CANCELED = 3,
    OS_FROZEN = 4
};

class CreateOrderResult: public BasicObject
{
public:
    CreateOrderResult()
        : m_status(OS_UNKNOWN)
        , m_result(-1)
        , m_price(0)
        , m_isDrm(1)
    {
    }

    virtual ObjectType GetObjectType() const
    {
        return OT_CREATE_ORDER_RESULT;
    }
     
    virtual BasicObject*  Clone() const
    {
        return new CreateOrderResult(*this);
    }

    bool IsSuccessful() const
    {
        return BSE_OK == m_result || BSE_BOOK_PURCHASED == m_result;
    }

    int GetResultCode() const
    {
        return m_result;
    }

    void SetBookId(const char* bookId)
    {
        m_bookId = bookId;
    }
    std::string GetBookId()
    {
        return m_bookId;
    }

    OrderStatus GetStatus() const
    {
        return (OrderStatus)m_status;
    }

    std::string GetOrderId() const
    {
        return m_orderId;
    }

    int GetPrice() const
    {
        return m_price;
    }
    bool IsDrm() const
    {
        return m_isDrm != 0;
    }
    static CreateOrderResult* CreateCreateOrderResult(const JsonObject* jsonObject);

private:
    bool Init(const JsonObject* jsonObject);
    int m_status;
    int m_result;
    int m_price;
    std::string m_paymentUrl;
    std::string m_orderId;
    std::string m_sign;
    std::string m_bookId;
    int m_isDrm;
};

typedef std::tr1::shared_ptr<CreateOrderResult> CreateOrderResultSPtr;

} // namespace model
} // namespace bookstore
} // namespace dk
#endif
