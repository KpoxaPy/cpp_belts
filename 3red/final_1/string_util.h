#pragma once

#include <string_view>
#include <list>

std::list<std::string_view> SplitIntoWords(std::string_view line);