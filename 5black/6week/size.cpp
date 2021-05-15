#include "size.h"

#include <tuple>

using namespace std;

bool Size::operator==(const Size& rhs) const {
  return tie(rows, cols) == tie(rhs.rows, rhs.cols);
}
