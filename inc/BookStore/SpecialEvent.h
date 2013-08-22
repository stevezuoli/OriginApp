#ifndef __BOOKSTORE_SPECIAL_EVENT_H__
#define __BOOKSTORE_SPECIAL_EVENT_H__

#include "BookStore/BasicObject.h"
#include <time.h>
#include <vector>
#include <map>

namespace dk
{
namespace bookstore
{
namespace model
{

class SpecialEvent;

typedef std::tr1::shared_ptr<SpecialEvent> SpecialEventSPtr;
typedef std::vector<SpecialEventSPtr> SpecialEventList;
typedef std::map<std::string, SpecialEventSPtr> SpecialEventMap;

class SpecialEvent : public BasicObject
{
private:
    SpecialEvent();

public:
    virtual ObjectType GetObjectType() const
    {
        return OT_SPECIAL_EVENTS;
    }

    static SpecialEvent* CreateSpecialEvent(const JsonObject* jsonObject);

    virtual BasicObject* Clone() const
    {
        return new SpecialEvent(*this);
    }

    virtual ~SpecialEvent(){};

    double GetStartTime() const { return m_startTime; }
    double GetEndTime() const { return m_endTime; }
    const std::string& GetName() const { return m_name; }
    const std::map<double, double>& GetStrategies() const { return m_strategies; }
    int GetType() const { return m_type; }

    bool GetDiscount(const double price, std::pair<double, double>& activeStrategy, double& specialPrice);
    bool GetGap(const double price, std::pair<double, double>& activeStrategy, double& gapToDiscount);

    static SpecialEventList FromBasicObjectList(const BasicObjectList& basicObjectList);
private:
    bool Init(const JsonObject* jsonObj);

    double m_startTime;
    double m_endTime;
    std::string m_name;
    std::map<double, double> m_strategies;
    int m_type;
}; // class SpecialEvent


} // namespace model
} // namespace bookstore
} // namespace dk
#endif

