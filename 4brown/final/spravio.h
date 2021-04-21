#pragma once

#include <ostream>
#include <istream>
#include <memory>

#include "sprav.h"
#include "request.h"

class SpravIO {
 public:
  SpravIO(SpravPtr sprav, std::ostream& output, Format format = Format::JSON);
  ~SpravIO();

  void Process(std::istream& input, Format format = Format::JSON);

 private:
  class PImpl;
  std::unique_ptr<PImpl> pimpl_;
};
