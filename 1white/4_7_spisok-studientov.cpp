#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Student {
    string name;
    string surname;
    int bd_day = 0;
    int bd_month = 0;
    int bd_year = 0; 

    void Read(istream& in) {
        in >> name >> surname >> bd_day >> bd_month >> bd_year;
    }

    string GetName() const {
        return name + " " + surname;
    }

    string GetBDate() const {
        return to_string(bd_day) + "." + to_string(bd_month) + "." + to_string(bd_year);
    }
};


int main() {
    int q;
    cin >> q;
    vector<Student> students(q);
    for (Student& s : students) {
        s.Read(cin);
    }

    cin >> q;
    while (q-- > 0) {
        string op;
        int num;
        cin >> op >> num;
        if (num < 1 || num > students.size()) {
            cout << "bad request" << endl;
        } else if (op == "name") {
            cout << students[num-1].GetName() << endl;
        } else if (op == "date") {
            cout << students[num-1].GetBDate() << endl;
        } else {
            cout << "bad request" << endl;
        }
    }
}