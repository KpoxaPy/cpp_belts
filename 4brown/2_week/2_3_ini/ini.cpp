#include "ini.h"

#include <string_view>
#include <stdexcept>

namespace {
    using namespace std;
    using namespace Ini;

    Section EMPTY;

    string_view Strip(string_view s) {
        while (s.size() > 0 && isspace(s.front())) {
            s.remove_prefix(1);
        }
        while (s.size() > 0 && isspace(s.back())) {
            s.remove_suffix(1);
        }
        return s;
    }
}

namespace Ini {

Section& Document::AddSection(string name) {
    return sections[move(name)];
}

const Section& Document::GetSection(const string& name) const {
    if (auto it = sections.find(name); it != sections.end()) {
        return it->second;
    }
    throw out_of_range("");
}

size_t Document::SectionCount() const {
    return sections.size();
}

Document Load(istream& input) {
    Document doc;
    Section* current_section = nullptr;
    for (string line; getline(input, line);) {
        string_view l = Strip(line);
        if (l.empty()) {
            continue;
        } else if (l.front() == '[' && l.back() == ']') {
            l.remove_prefix(1);
            l.remove_suffix(1);
            current_section = &doc.AddSection(string(l));
        } else {
            size_t eq_pos = l.find('=', 0);
            string_view key = l.substr(0, eq_pos);
            string_view value = l.substr(eq_pos + 1, string_view::npos);
            current_section->emplace(key, value);
        }
    }

    return doc;
}

} // namespace Ini