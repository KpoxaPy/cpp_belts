#include <iostream>
#include <utility>

template <typename T, typename U>
T&& MisteryOp(U&& x) {
    return static_cast<T&&>(x);
}

void CheckRef(const int&) {
    std::cout << "const int&" << std::endl;
}

void CheckRef(int&) {
    std::cout << "int&" << std::endl;
}

void CheckRef(int&&) {
    std::cout << "int&&" << std::endl;
}

template <typename T>
void Foo(T&& x) {
    CheckRef(MisteryOp<T>(x));
}

template <typename T>
void Bar(T&& x) {
    CheckRef(std::forward<T>(x));
}

int main() {
    int i = 0;
    Foo(i);
    Foo(1);
    Bar(i);
    Bar(1);

    CheckRef(MisteryOp<int, int&>(1));

    return 0;
}