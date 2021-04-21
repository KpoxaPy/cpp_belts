#include <set>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;


int main()
{
    vector<int> vec;
    auto vector_begin = begin(vec);
    auto vector_end = end(vec);

    string str;
    auto string_begin = begin(str);
    auto string_end = end(str);

    set<int> s;
    auto set_begin = begin(s);
    auto set_end = end(s);

    auto res = next_permutation(set_begin, set_end);

    return 0;
}