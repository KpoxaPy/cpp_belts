#include "stats.h"

#include <iostream>
#include <string_view>
#include <set>

namespace {
    const set<string> METHODS = {
        "GET",
        "POST",
        "PUT",
        "DELETE"
    };
    const string METHOD_UNKNOWN = "UNKNOWN";

    const set<string> URIS = {
        "/",
        "/order",
        "/product",
        "/basket",
        "/help"
    };
    const string URI_UNKNOWN = "unknown";
} // namespace

Stats::Stats() {
    for (const auto& m : METHODS) {
        method_stats_[GetMethodView(m)] = 0;
    }
    method_stats_[GetMethodView(METHOD_UNKNOWN)] = 0;

    for (const auto& u : URIS) {
        uri_stats_[GetUriView(u)] = 0;
    }
    uri_stats_[GetUriView(URI_UNKNOWN)] = 0;
}

void Stats::AddMethod(string_view method) {
    method_stats_[GetMethodView(method)]++;
}

void Stats::AddUri(string_view uri) {
    uri_stats_[GetUriView(uri)]++;
}

const map<string_view, int>& Stats::GetMethodStats() const {
    return method_stats_;    
}

const map<string_view, int>& Stats::GetUriStats() const {
    return uri_stats_;
}

string_view Stats::GetMethodView(string_view v) {
    if (auto it = METHODS.find(string(v)); it != METHODS.end()) {
        return *it;
    }
    return METHOD_UNKNOWN;
}

string_view Stats::GetUriView(string_view v) {
    if (auto it = URIS.find(string(v)); it != URIS.end()) {
        return *it;
    }
    return URI_UNKNOWN;
}

HttpRequest ParseRequest(string_view line) {
    line.remove_prefix(line.find_first_not_of(' '));
    size_t sp = line.find(' ');
    string_view method = line.substr(0, sp);
    line.remove_prefix(sp + 1);
    sp = line.find(' ');
    string_view uri = line.substr(0, sp);
    line.remove_prefix(sp + 1);
    // cout << "Parsed: method = " << method << " uri = " << uri << " proto = " << line << endl;
    return {method, uri, line};
}