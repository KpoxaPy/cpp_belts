#pragma once

#include <ostream>
#include <string>
#include <vector>
#include <list>

#include "index.h"

class SearchResult {
    friend std::ostream& operator<<(std::ostream& s, const SearchResult& r);

public:
    SearchResult(std::string query, WordIndex idx, uint64_t& t);

private:
    std::string query;
    WordIndex results;

    void sort();
};

std::ostream& operator<<(std::ostream& s, const SearchResult& r);
