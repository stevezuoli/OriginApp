#include "BookStore/CreateOrderResult.h"
#include "Utility/JsonObject.h"

namespace dk
{
namespace bookstore
{
namespace model
{
bool CreateOrderResult::Init(const JsonObject* jsonObject)
{
    if (!jsonObject->GetIntValue("result", &m_result))
    {
        return false;
    }
    jsonObject->GetIntValue("status", &m_status);
    double tempPrice = 0;
    if (jsonObject->GetDoubleValue("price", &tempPrice))
    {
        m_price = tempPrice * 100;
    }
    jsonObject->GetStringValue("order_uuid", &m_orderId);
    jsonObject->GetStringValue("payment_url", &m_paymentUrl);
    jsonObject->GetIntValue("is_drm", &m_isDrm);
    return true;
}

CreateOrderResult* CreateOrderResult::CreateCreateOrderResult(const JsonObject* jsonObject)
{
    CreateOrderResult* result = new CreateOrderResult();
    if (NULL != result && result->Init(jsonObject))
    {
        return result;
    }
    delete result;
    return NULL;
}

} // namespace model
} // namespace bookstore
} // namespace dk
