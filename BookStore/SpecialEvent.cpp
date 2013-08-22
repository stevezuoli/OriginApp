#include "BookStore/SpecialEvent.h"
#include "Utility/JsonObject.h"
#include "Common/LockObject.h"
#include "Utility/StringUtil.h"
#include "Utility/EncodeUtil.h"
#include "KernelDef.h"
#include <map>

using namespace std;
using namespace dk::common;
using namespace dk::utility;

namespace dk
{
namespace bookstore
{
namespace model
{

SpecialEvent::SpecialEvent()
    : m_startTime(0)
    , m_endTime(0)
    , m_name()
    , m_strategies()
    , m_type(0)
{
}

bool SpecialEvent::Init(const JsonObject* jsonObj)
{
    jsonObj->GetDoubleValue("end", &m_endTime);
    jsonObj->GetStringValue("name", &m_name);
    jsonObj->GetDoubleValue("start", &m_startTime);
    jsonObj->GetIntValue("type", &m_type);

    JsonObjectSPtr items = jsonObj->GetSubObject("strategy");
    if (items != 0)
    {
        int count = items->GetArrayLength();
        for (int i = 0; i < count; ++i)
        {
            JsonObjectSPtr item = items->GetElementByIndex(i);
            if (!item)
            {
                continue;
            }
            int sub_count = item->GetArrayLength();

            // TODO: currently we only support <int, int> type
            if (sub_count != 2)
            {
                continue;
            }
            double first_var = 0;
            if (!item->GetElementByIndex(0)->GetDoubleValue(&first_var))
            {
                continue;
            }
            double second_var = 0;
            if (!item->GetElementByIndex(1)->GetDoubleValue(&second_var))
            {
                continue;
            }
            m_strategies[first_var] = second_var;
        }
    }

    return true;
}

SpecialEvent* SpecialEvent::CreateSpecialEvent(const JsonObject* jsonObject)
{
    SpecialEvent* specialEvent = new SpecialEvent();
    if (NULL != specialEvent && specialEvent->Init(jsonObject))
    {
        return specialEvent;
    }
    if (specialEvent != 0)
    {
        delete specialEvent;
    }
    return 0;
}

bool SpecialEvent::GetDiscount(const double price,
                               std::pair<double, double>& activeStrategy,
                               double& specialPrice)
{
    if (m_strategies.empty())
    {
        // No strategies, return false
        return false;
    }
    specialPrice = 0.0f;
    std::map<double, double>::const_iterator iter = m_strategies.end();
    while (iter != m_strategies.begin())
    {
        iter--;
        if (iter->first <= 0)
        {
            // invalid strategy
            continue;
        }
        double modulus = price / iter->first;
        if (modulus < 1.0f)
        {
            // Failed to reach this strategy, turn to lower one
            continue;
        }
        int multiple = static_cast<int>(modulus);
        double specialCount = iter->second * static_cast<double>(multiple);
        specialPrice = price - specialCount;
        activeStrategy.first = iter->first;
        activeStrategy.second = iter->second;
        return true;
    }
    return false;
}

bool SpecialEvent::GetGap(const double price,
                          std::pair<double, double>& activeStrategy,
                          double& gapToDiscount)
{
    if (m_strategies.empty())
    {
        // No strategies, return false
        return false;
    }
    gapToDiscount = 0.0f;
    std::map<double, double>::const_iterator iter = m_strategies.end();
    while (iter != m_strategies.begin())
    {
        iter--;
        if (iter->first <= 0)
        {
            // invalid strategy
            continue;
        }
        double modulus = price / iter->first;
        if (modulus >= 1.0f)
        {
            // Failed to reach this strategy, turn to lower one
            break;
        }
        gapToDiscount = iter->first - price;
        activeStrategy.first  = iter->first;
        activeStrategy.second = iter->second;
    }
    return gapToDiscount != 0.0f;
}

SpecialEventList SpecialEvent::FromBasicObjectList(const BasicObjectList& basicObjectList)
{
    SpecialEventList result;
    for (size_t i = 0; i < basicObjectList.size(); ++i)
    {
        if (basicObjectList[i] &&
			(basicObjectList[i]->GetObjectType() == OT_SPECIAL_EVENTS))
        {
            result.push_back(SpecialEventSPtr((SpecialEvent*)(basicObjectList[i]->Clone())));
        }
    }
    return result;
}


} // namespace model
} // namespace bookstore
} // namespace dk
