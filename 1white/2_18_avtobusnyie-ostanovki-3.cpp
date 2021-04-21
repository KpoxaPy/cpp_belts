#include <iostream>
#include <map>
#include <set>

using namespace std;

int main()
{
    map<set<string>, size_t> schedules;
    size_t q;    
    cin >> q;

    while (q > 0) {
        q--;
        int stops_count;
        cin >> stops_count;
        set<string> stops;
        for (int i = 0; i < stops_count; i++) {
            string stop;
            cin >> stop;
            stops.insert(stop);
        }
        if (schedules.count(stops) == 0) {
            size_t new_schedule_num = schedules.size() + 1;
            schedules[stops] = new_schedule_num;
            cout << "New bus " << new_schedule_num << endl;
        } else {
            cout << "Already exists for " << schedules[stops] << endl;
        }
    }
    return 0;
}