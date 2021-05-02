#include "spravio.h"
#include "reader.h"
#include "string_view_utils.h"
#include "json.h"
#include "profile.h"

#include <list>

using namespace std;
using namespace std::placeholders;

class SpravIO::PImpl {
 public:
  PImpl(SpravPtr sprav, std::ostream& output, Format format)
      : sprav_(move(sprav)), os_(output), output_format_(format) {}

  void Process(std::istream& input) {
    ProcessJson(input);
  }

  template <typename InputIt>
  void Output(InputIt begin, InputIt end) {
    switch (output_format_) {
      case Format::JSON:
      case Format::JSON_PRETTY:
      {
        Json::Node node = Json::Array{};
        Json::Array& array = node.AsArray();
        for (; begin != end; ++begin) {
          array.push_back(begin->get()->AsJson());
        }
        os_ << Json::Printer(node, output_format_ == Format::JSON_PRETTY) << "\n";
        break;
      }

      default:
        throw runtime_error("Unknown format for output");
    }
  }

  void ProcessJson(std::istream& input) {
    LOG_DURATION("ProcessJson");
    auto doc = [&input]() mutable {
      LOG_DURATION("Json::Load");
      return Json::Load(input);
    }();
    auto dict = doc.GetRoot().AsMap();

    if (auto it = dict.find("routing_settings"); it != dict.end()) {
      sprav_->SetRoutingSettings({it->second.AsMap()});
    }
    if (auto it = dict.find("render_settings"); it != dict.end()) {
      sprav_->SetRenderSettings({it->second.AsMap()});
    }

    {
      LOG_DURATION("Process base requests");
      for (auto& r : dict.at("base_requests").AsArray()) {
        MakeBaseRequest(r)->Process(sprav_);
      }
    }

    {
      LOG_DURATION("Build sprav");
      sprav_->Build();
    }

    list<ResponsePtr> responses;
    {
      LOG_DURATION("Process stat requests");
      for (auto& r : dict.at("stat_requests").AsArray()) {
        auto resp = MakeRequest(r)->Process(sprav_);
        if (!resp->empty()) {
          responses.emplace_back(move(resp));
        }
      }
    }
    {
      LOG_DURATION("Output stat responses");
      Output(responses.begin(), responses.end());
    }
  }

  SpravPtr sprav_;
  std::ostream& os_;
  Format output_format_;
};

SpravIO::SpravIO(SpravPtr sprav, std::ostream& output, Format format)
    : pimpl_(make_unique<PImpl>(sprav, output, format)) {}

SpravIO::~SpravIO() = default;

void SpravIO::Process(std::istream& input) {
  pimpl_->Process(input);
}