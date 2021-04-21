#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

class SortedStrings {
public:
  void AddString(const string& s) {
    words.insert(upper_bound(begin(words), end(words), s), s);
  }

  vector<string> GetSortedStrings() {
    return words;
  }
  
private:
  vector<string> words;
};

// void PrintSortedStrings(SortedStrings& strings) {
//   for (const string& s : strings.GetSortedStrings()) {
//     cout << s << " ";
//   }
//   cout << endl;
// }

// int main() {
//   SortedStrings strings;
  
//   strings.AddString("first");
//   strings.AddString("third");
//   strings.AddString("second");
//   PrintSortedStrings(strings);
  
//   strings.AddString("second");
//   PrintSortedStrings(strings);
  
//   return 0;
// }
