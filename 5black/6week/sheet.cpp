#include <stdexcept>

#include "common_etc.h"
#include "sheet_impl.h"

using namespace std;

std::unique_ptr<ISheet> CreateSheet() {
  return make_unique<Sheet>();
}
