#include <iostream>
#include <string>
#include <deque>
#include <utility>

using namespace std;

const bool less_pars = true;

int main() {
    int value, ops;
    cin >> value >> ops;

    deque<string> d = {to_string(value)};
    char prev_op = '*';
    while (ops > 0) {
        char op;
        int n;
        cin >> op >> n;

        bool dont_pars = false;
        if (less_pars && (op == '+' || op == '-' || prev_op == '*' || prev_op == '/')) {
            dont_pars = true;
        }

        if (!(less_pars && dont_pars)) {
            d.push_front("(");
            d.push_back(")");
        }
        d.push_back(" " + string(1, op) + " " + to_string(n));

        --ops;
        prev_op = op;
    }

    for (const auto& s : d) {
        cout << s;
    }
    return 0;
}