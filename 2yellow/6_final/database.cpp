#include "database.h"

using namespace std;

std::ostream& operator<<(std::ostream& s, const Entry& entry) {
    s << entry.date << " " << entry.event;
    return s;
}

void Database::Add(const Date &date, const string &event)
{
    auto& es = events[date];
    if (es.s.count(event) == 0) {
        es.s.insert(event);
        es.v.push_back(event);
    }
}

void Database::Print(ostream& s) const
{
    for (const auto &[date, date_events] : events)
    {
        for (const auto &event : date_events.v)
        {
            s << Entry{date, event} << endl;
        }
    }
}

Entry Database::Last(const Date& date) const {
    auto ub_it = events.upper_bound(date);
    if (ub_it == begin(events)) {
        return {};
    }
    --ub_it;
    if (ub_it->second.v.size() == 0) {
        return {};
    }
    return {ub_it->first, ub_it->second.v.back()};
}