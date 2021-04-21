#include "bus_manager.h"

using namespace std;

void BusManager::AddBus(const string &bus, const vector<string> &stops) {
    buses_to_stops_[bus] = stops;
    for (const auto &stop : stops)
    {
        stops_to_buses_[stop].push_back(bus);
    }
}

BusesForStopResponse BusManager::GetBusesForStop(const string &stop) const {
    if (auto it = stops_to_buses_.find(stop); it != end(stops_to_buses_))
    {
        return {it->second};
    }
    return {};
}

StopsForBusResponse BusManager::GetStopsForBus(const string &bus) const {
    StopsForBusResponse r;
    if (auto it = buses_to_stops_.find(bus); it != end(buses_to_stops_))
    {
        for (const auto &stop : it->second)
        {
            vector<string> buses;
            for (const string &other_bus : stops_to_buses_.at(stop))
            {
                if (bus != other_bus)
                {
                    buses.push_back(other_bus);
                }
            }
            r.stops.emplace_back(stop, buses);
        }
    }
    return r;
}

AllBusesResponse BusManager::GetAllBuses() const {
    return {buses_to_stops_};
}