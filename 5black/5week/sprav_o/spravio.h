#pragma once

#include <ostream>
#include <istream>
#include <memory>

#include "sprav.h"
#include "request.h"

class SpravIO {
 public:
  enum class Mode {
    MAKE_BASE,
    PROCESS_REQUESTS
  };

 public:
  SpravIO(SpravPtr sprav, Mode mode, std::ostream& output, Format format = Format::JSON);
  ~SpravIO();

  void Process(std::istream& input);

 private:
  class PImpl;
  std::unique_ptr<PImpl> pimpl_;
};
