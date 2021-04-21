#include "event.h"

#include <iomanip>

using namespace std;

string ParseEvent(istream& is) {
    string res;
    getline(is >> ws, res);
    return res;
}