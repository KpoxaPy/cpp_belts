#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <exception>

#include "stream_utils.h"

template <class T, class U>
void AssertEqual(const T& t, const U& u, const std::string& hint = {}) {
    if (t != u)
    {
        std::ostringstream os;
        os << "Assertion failed: " << t << " != " << u;
        if (!hint.empty())
        {
            os << " hint: " << hint;
        }
        throw std::runtime_error(os.str());
    }
}

#define AssertException(code, exc, hint) \
do { \
    bool thrown = false; \
    try { \
        code; \
    } catch(exc&) { \
        thrown = true; \
    } \
    if (!thrown) { \
        throw std::runtime_error("Expected exception: " #exc " hint: " + string(hint)); \
    } \
} while(false)

void Assert(bool b, const std::string& hint);

class TestRunner
{
public:
    ~TestRunner();

    template <class TestFunc>
    void RunTest(TestFunc func, const std::string& test_name) {
        try
        {
            func();
            std::cerr << test_name << " OK" << std::endl;
        }
        catch (std::exception &e)
        {
            ++fail_count;
            std::cerr << test_name << " fail: " << e.what() << std::endl;
        }
        catch (...)
        {
            ++fail_count;
            std::cerr << "Unknown exception caught" << std::endl;
        }
    }

private:
    int fail_count = 0;
};