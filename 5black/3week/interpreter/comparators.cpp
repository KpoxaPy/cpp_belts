#include "comparators.h"
#include "object.h"
#include "object_holder.h"
#include "runtime_error.h"

#include <functional>
#include <optional>
#include <sstream>

using namespace std;

namespace Runtime {

bool Less(ObjectHolder lhs, ObjectHolder rhs) {
  if (!lhs || !rhs) {
    throw RuntimeError("Cannot operate with None");
  }

  return lhs->Less(rhs);
}

bool Equal(ObjectHolder lhs, ObjectHolder rhs) {
  if (!lhs || !rhs) {
    throw RuntimeError("Cannot operate with None");
  }
  
  return lhs->Equal(rhs);
}

} /* namespace Runtime */
