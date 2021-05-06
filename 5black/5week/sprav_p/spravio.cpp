#include "spravio.h"

#include <queue>

#include "json.h"
#include "profile.h"
#include "reader.h"
#include "string_view_utils.h"

using namespace std;
using namespace std::placeholders;

class SpravIO::PImpl {
 public:
  PImpl(SpravPtr sprav, Mode mode, std::ostream& output, Format format)
      : sprav_(move(sprav))
      , mode_(mode)
      , os_(output)
      , output_format_(format) {}

  void Process(std::istream& input) {
    LOG_DURATION("Process");
    Json::Document* doc = new Json::Document([&input]() mutable {
      LOG_DURATION("Process: loading json");
      return Json::Load(input);
    }());
    auto& dict = doc->GetRoot().AsMap();

    ReadSettings(dict);

    if (mode_ == Mode::MAKE_BASE) {
      LOG_DURATION("Process: MakeBase");
      MakeBase(dict);
    } else if (mode_ == Mode::PROCESS_REQUESTS) {
      LOG_DURATION("Process: ProcessRequests");
      ProcessRequests(dict);
    }

    // {
      // LOG_DURATION("Process: destruct json");
      // doc.reset();
    // }
  }

  void Output(queue<ResponsePtr> responses) {
    SumMeter resp_destr("Response desruction");
    SumMeter resp_output("Response output");

    switch (output_format_) {
      case Format::JSON:
      case Format::JSON_PRETTY:
      {
        os_ << "[";
        bool first = true;
        while (!responses.empty()) {
          {
            METER_DURATION(resp_output);
            os_ << (first ? first = false, "" : ",")
              << Json::Printer(responses.front()->AsJson(), output_format_ == Format::JSON_PRETTY);
          }
          {
            METER_DURATION(resp_destr);
            responses.pop();
          }
        }
        os_ << "]\n";
        break;
      }

      default:
        throw runtime_error("Unknown format for output");
    }
  }

  void ReadSettings(const Json::Map& root) {
    if (auto it = root.find("serialization_settings"); it != root.end()) {
      sprav_->SetSerializationSettings({it->second.AsMap()});
    }
    if (auto it = root.find("routing_settings"); it != root.end()) {
      sprav_->SetRoutingSettings({it->second.AsMap()});
    }
    if (auto it = root.find("render_settings"); it != root.end()) {
      sprav_->SetRenderSettings({it->second.AsMap()});
    }
  }

  void MakeBase(const Json::Map& root) {
    for (auto& r : root.at("base_requests").AsArray()) {
      MakeBaseRequest(r)->Process(sprav_);
    }

    sprav_->BuildBase();
    sprav_->Serialize();
  }

  void ProcessRequests(const Json::Map& root) {
    {
      LOG_DURATION("ProcessRequests: Deserialization");
      sprav_->Deserialize();
    }

    queue<ResponsePtr> responses;
    {
      LOG_DURATION("ProcessRequests responses generating");
      for (auto& r : root.at("stat_requests").AsArray()) {
        auto resp = MakeRequest(r)->Process(sprav_);
        if (!resp->empty()) {
          responses.emplace(move(resp));
        }
      }
    }

    {
      LOG_DURATION("ProcessRequests: Output");
      Output(std::move(responses));
    }
  }

  SpravPtr sprav_;
  Mode mode_;
  std::ostream& os_;
  Format output_format_;
};

SpravIO::SpravIO(SpravPtr sprav, Mode mode, std::ostream& output, Format format)
    : pimpl_(make_unique<PImpl>(sprav, mode, output, format)) {}

SpravIO::~SpravIO() = default;

void SpravIO::Process(std::istream& input) {
  pimpl_->Process(input);
}