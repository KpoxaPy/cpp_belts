#pragma once

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <algorithm>

#include "date.h"

struct Entry {
    Date date;
    std::string event;
};

std::ostream& operator<<(std::ostream& s, const Entry& entry);

class Database {
private:
    struct DateEvents {
        std::vector<std::string> v;
        std::set<std::string> s;
    };

public:
    void Add(const Date& date, const std::string& event);

    template <typename Predicate>
    int RemoveIf(const Predicate& predicate) {
        int removed_count = 0;
        std::set<Date> dates_to_remove;
        for (auto& date_pair : events) {
            const Date& date = date_pair.first;
            DateEvents& date_events = date_pair.second;
            auto it = remove_if(begin(date_events.v), end(date_events.v), [&date, &removed_count, &predicate, &date_events](const auto& event) {
                if (predicate(date, event)) {
                    date_events.s.erase(event);
                    ++removed_count;
                    return true;
                }
                return false;
            });
            date_events.v.erase(it, end(date_events.v));
            if (date_events.v.size() == 0) {
                dates_to_remove.insert(date);
            } 
        }

        for (const auto& date : dates_to_remove) {
            events.erase(date);
        }
        
        return removed_count;
    }

    template <typename Predicate>
    std::vector<Entry> FindIf(const Predicate& predicate) const {
        std::vector<Entry> entries;
        for (const auto &[date, date_events] : events) {
            for (const auto& event : date_events.v) {
                if (predicate(date, event)) {
                    entries.push_back({date, event});
                }
            }
        }
        return entries;
    }

    Entry Last(const Date& date) const;

    void Print(std::ostream& s) const;

private:
    std::map<Date, DateEvents> events;
};
