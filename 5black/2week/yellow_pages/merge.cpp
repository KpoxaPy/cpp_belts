#include <google/protobuf/util/message_differencer.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <tuple>
#include <vector>

#include "yellow_pages.h"

using namespace std;

namespace {

using namespace YellowPages;
using ::google::protobuf::Message;
using ::google::protobuf::Reflection;
using ::google::protobuf::FieldDescriptor;
using ::google::protobuf::util::MessageDifferencer;

using PriorityToSignals = map<uint64_t, vector<size_t>, greater<uint64_t>>;

PriorityToSignals FillPriorityToSignals(const Signals& signals, const Providers& providers) {
  PriorityToSignals priority_to_signals;
  auto signals_size = signals.size();
  for (size_t idx = 0; idx < signals_size; ++idx) {
    auto priority = providers.at(signals[idx].provider_id()).priority();
    priority_to_signals[priority].push_back(idx);
  }
  return priority_to_signals;
}

class Merger {
 public:
  Merger(const Signals& signals, const Providers& providers)
    : signals_(signals)
    , providers_(providers)
    , priority_to_signals_(FillPriorityToSignals(signals_, providers_))
  {}

  Company Merge() const {
    Company result;
    auto descriptor = result.GetDescriptor();
    const int size = descriptor->field_count();
    for (int i = 0; i < size; ++i) {
      MergeField(result, descriptor->field(i));
    }
    return result;
  }

  void MergeRepeatedField(Company& company, const FieldDescriptor* field) const {
    vector<const Message*> values;
    auto reflection = company.GetReflection();
    for (auto [_, signals_set] : priority_to_signals_) {
      for (auto idx : signals_set) {
        const Company& c = signals_.at(idx).company();
        const int size = reflection->FieldSize(c, field);
        for (int i = 0; i < size; ++i) {
          auto new_message = &reflection->GetRepeatedMessage(c, field, i);
          bool already_in_values = false;
          for (auto v : values) {
            if (MessageDifferencer::Equals(*v, *new_message)) {
              already_in_values = true;
              break;
            }
          }
          if (!already_in_values) {
            values.push_back(new_message);
          }
        }
      }

      if (values.size() > 0) {
        break;
      }
    }

    for (auto value : values) {
      reflection->AddMessage(&company, field)->CopyFrom(*value);
    }
  }

  void MergeSingularField(Company& company, const FieldDescriptor* field) const {
    auto reflection = company.GetReflection();
    for (auto [_, signals_set] : priority_to_signals_) {
      for (auto idx : signals_set) {
        const auto& c = signals_.at(idx).company();
        if (reflection->HasField(c, field)) {
          // cerr << "Merge in singular field (signal #" << idx << "): " << reflection->GetMessage(c, field).DebugString();
          reflection->MutableMessage(&company, field)->CopyFrom(reflection->GetMessage(c, field));
          return;
        }
      }
    }
  }

  void MergeField(Company& company, const FieldDescriptor* field) const {
    if (field->is_repeated()) {
      MergeRepeatedField(company, field);
    } else {
      MergeSingularField(company, field);
    }
  }

 private:
  const Signals& signals_;
  const Providers& providers_;
  const PriorityToSignals priority_to_signals_;
};

}


namespace YellowPages {

  Company Merge(const Signals& signals, const Providers& providers) {
    return Merger(signals, providers).Merge();
  }

}
