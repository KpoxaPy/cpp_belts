#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;

int main() {
    ifstream input("input.txt");
    int rows, cols;
    input >> rows >> cols;
    string empty;
    getline(input, empty);
    vector<vector<string>> table(rows, vector<string>(cols));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (j < cols - 1) {
                getline(input, table[i][j], ',');
            } else {
                getline(input, table[i][j]);
            }
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << setw(10) << table[i][j];
            if (j < cols - 1) {
                cout << " ";
            }
        }
        cout << endl;
    }
}