#include <iostream>
#include <vector>
#include <stdexcept>

// #include <random>
// #include "test_runner.h"

using namespace std;

template <class T>
class Deque {
public:
    Deque() = default;

    bool Empty() const {
        return front.empty() && back.empty();
    }

    size_t Size() const {
        return front.size() + back.size();
    }

    T& Front() {
        if (front.empty()) {
            return back.front();
        }
        return front.back();
    }

    const T& Front() const {
        if (front.empty()) {
            return back.front();
        }
        return front.back();
    }

    T& Back() {
        if (back.empty()) {
            return front.front();
        }
        return back.back();
    }

    const T& Back() const {
        if (back.empty()) {
            return front.front();
        }
        return back.back();
    }

    void PushFront(const T& v) {
        front.push_back(v);
    }

    void PushBack(const T& v) {
        back.push_back(v);
    }

    T& operator[](size_t idx) {
        if (idx < front.size()) {
            return front[front.size() - 1 - idx];
        }
        return back[idx - front.size()];
    }

    const T& operator[](size_t idx) const {
        if (idx < front.size()) {
            return front[front.size() - 1 - idx];
        }
        return back[idx - front.size()];
    }

    T& At(size_t idx) {
        CheckRange(idx);
        return operator[](idx);
    }

    const T& At(size_t idx) const {
        CheckRange(idx);
        return operator[](idx);
    }

private:
    void CheckRange(size_t idx) const {
        if (idx >= Size()) {
            throw out_of_range("");
        }
    }

private:
    vector<T> front;
    vector<T> back;
};

// void TestRand() {
//     random_device dev("default");
//     size_t size = uniform_int_distribution(10, 100)(dev);
//     auto choise_gen = uniform_int_distribution(0, 1);
//     auto int_gen = uniform_int_distribution(numeric_limits<int>::min(), numeric_limits<int>::max());
//     Deque<int> d;

//     for (size_t i = 0; i < size; ++i) {
//         if (choise_gen(dev) == 0) {
//             d.PushFront(int_gen(dev));
//         } else {
//             d.PushBack(int_gen(dev));
//         }
//     }
//     ASSERT_EQUAL(d.Size(), size);

//     for (size_t i = 0; i < size; ++i) {
//         d[i];
//     }
// }

// void TestAll() {
//     TestRunner tr;
//     tr.RunTest([](){
//         Deque<int> d;
//         ASSERT(d.Empty());
//         d.PushBack(1);
//         ASSERT_EQUAL(d.Size(), static_cast<size_t>(1));
//         d.PushFront(-1);
//         ASSERT_EQUAL(d.Size(), static_cast<size_t>(2));

//         ASSERT_EQUAL(d[0], -1);
//         ASSERT_EQUAL(d[1], 1);
//     }, "Deque simple");
//     tr.RunTest([](){
//         Deque<int> d;
//         d.PushFront(4);
//         d.PushFront(3);
//         d.PushFront(2);
//         d.PushFront(1);
//         d.PushFront(0);
//         d.PushBack(5);
//         d.PushBack(6);
//         d.PushBack(7);
//         d.PushBack(8);
//         d.PushBack(9);
//         for (int i = 0; i < 10; ++i) {
//             ASSERT_EQUAL(i, d[i]);
//         }
//     }, "Deque check order");
//     tr.RunTest([](){
//         Deque<int> d1;
//         d1.PushBack(0);
//         ASSERT_EQUAL(d1.Front(), 0);
//         ASSERT_EQUAL(d1.Back(), 0);
//         Deque<int> d2;
//         d2.PushFront(0);
//         ASSERT_EQUAL(d1.Front(), 0);
//         ASSERT_EQUAL(d1.Back(), 0);
//     }, "Deque front and back correctness");
//     for (int i = 0; i < 10; ++i) {
//         tr.RunTest(TestRand, "TestRand" + to_string(i));
//     }
// }

int main() {
    // TestAll();
    return 0;
}