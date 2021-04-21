#include <iostream>
#include <map>
#include <vector>

using namespace std;

using sprav_map = map<string, vector<string>>;

void new_bus(sprav_map& buses, sprav_map& stops) {
    string bus;
    size_t stops_count;
    cin >> bus >> stops_count;
    vector<string>& bus_stops = buses[bus];
    bus_stops.resize(stops_count);
    for (int i = 0; i < stops_count; i++) {
        cin >> bus_stops[i];
        stops[bus_stops[i]].push_back(bus);
    }
}

void buses_for_stop(const sprav_map& buses, const sprav_map& stops) {
    string stop;
    cin >> stop;
    if (stops.count(stop) == 0) {
        cout << "No stop" << endl;
    } else {
        for (const auto& bus : stops.at(stop)) {
            cout << bus << " ";
        }
        cout << endl;
    }
}

void stops_for_bus(const sprav_map& buses, const sprav_map& stops) {
    string bus;
    cin >> bus;
    if (buses.count(bus) == 0) {
        cout << "No bus" << endl;
    } else {
        for (const auto& stop : buses.at(bus)) {
            cout << "Stop " << stop << ": ";
            size_t count = 0;
            for (const auto& stop_bus : stops.at(stop)) {
                if (stop_bus != bus) {
                    ++count;
                    cout << stop_bus << " ";
                }
            }
            if (count == 0) {
                cout << "no interchange";
            }
            cout << endl;
        }
    }
}

void all_buses(const sprav_map& buses, const sprav_map& stops) {
    if (buses.empty()) {
        cout << "No buses" << endl;
    } else {
        for (const auto& [bus, stops] : buses) {
            cout << "Bus " << bus << ": ";
            for (const auto& stop : stops) {
                cout << stop << " ";
            }
            cout << endl;
        }
    }
}

int main()
{
    sprav_map buses;
    sprav_map stops;
    size_t q;    
    cin >> q;

    while (q > 0) {
        q--;
        string command;
        cin >> command;
        if (command == "NEW_BUS") {
            new_bus(buses, stops);
        } else if (command == "BUSES_FOR_STOP") {
            buses_for_stop(buses, stops);
        } else if (command == "STOPS_FOR_BUS") {
            stops_for_bus(buses, stops);
        } else if (command == "ALL_BUSES") {
            all_buses(buses, stops);
        }
    }
    return 0;
}