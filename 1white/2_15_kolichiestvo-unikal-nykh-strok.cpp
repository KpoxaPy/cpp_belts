#include <iostream>
#include <set>

using namespace std;

int main()
{
    set<string> words;
    size_t q;    
    cin >> q;

    while (q > 0) {
        q--;
        string word;
        cin >> word;
        words.insert(word);
    }
    cout << words.size() << endl;
    return 0;
}