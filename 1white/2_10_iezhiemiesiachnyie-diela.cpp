#include <iostream>
#include <vector>

using namespace std;

const vector<size_t> days_in_month = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int main()
{
    size_t month = 0; // start from january
    vector<vector<string>> cal(days_in_month[month]);

    size_t q;    
    cin >> q;

    while (q > 0) {
        q--;
        string command;
        cin >> command;
        if (command == "ADD") {
            size_t day;
            string thing;
            cin >> day >> thing;
            cal[day - 1].push_back(thing);
        } else if (command == "DUMP") {
            size_t day;
            cin >> day;
            cout << cal[day - 1].size() << " ";
            for (const string& thing : cal[day - 1]) {
                cout << thing << " ";
            }
            cout << endl;
        } else if (command == "NEXT") {
            size_t next_month = (month + 1) % 12;
            size_t cur_month_days = days_in_month[month];
            size_t next_month_days = days_in_month[next_month];
            if (cur_month_days > next_month_days) {
                for (size_t i = next_month_days; i < cur_month_days; i++) {
                    cal[next_month_days-1].insert(end(cal[next_month_days-1]), begin(cal[i]), end(cal[i]));
                }
            }
            cal.resize(next_month_days);
            month = next_month;
        }
    }
    return 0;
}