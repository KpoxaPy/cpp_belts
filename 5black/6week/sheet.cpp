#include "sheet.h"
#include "sheet_impl.h"

#include <stdexcept>

using namespace std;

std::unique_ptr<ISheet> CreateSheet() {
  return make_unique<Sheet>();
}
