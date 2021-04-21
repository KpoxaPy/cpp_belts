int Factorial(int n) {
    if (n < 2) {
        return 1;
    }
    return n * Factorial(n - 1);
}

// #include <iostream>

// using namespace std;

// int main() {
//     for (int i = -1; i <= 10; i++) {
//         cout << i << "! = " << Factorial(i) << endl;
//     }
//     return 0;
// }