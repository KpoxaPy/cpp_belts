#include <iostream>
#include <set>
#include <map>
using namespace std;

using namespace std;
int main()
{
    map<string, set<string>> synonyms;
    size_t q;    
    cin >> q;

    while (q > 0) {
        q--;
        string command;
        cin >> command;
        if (command == "ADD") {
            string word1, word2;
            cin >> word1 >> word2;
            synonyms[word1].insert(word2);
            synonyms[word2].insert(word1);
        } else if (command == "COUNT") {
            string word;
            cin >> word;
            cout << synonyms[word].size() << endl;
        } else if (command == "CHECK") {
            string word1, word2;
            cin >> word1 >> word2;
            cout << (synonyms[word1].count(word2) > 0 ? "YES" : "NO") << endl;
        }
    }
    return 0;
}