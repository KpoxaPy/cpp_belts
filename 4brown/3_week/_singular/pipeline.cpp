#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;


struct Email {
  string from;
  string to;
  string body;
};

istream& operator>>(istream& s, Email& e) {
  getline(s, e.from);
  getline(s, e.to);
  getline(s, e.body);
  return s;
}

ostream& operator<<(ostream& s, const Email& e) {
  return s << e.from << endl << e.to << endl << e.body << endl;
}


class Worker {
public:
  virtual ~Worker() = default;

  virtual void Process(unique_ptr<Email> email) = 0;

  virtual void Run() {
    throw logic_error("Unimplemented");
  }

protected:
  void PassOn(unique_ptr<Email> email) const {
    if (next_) {
      next_->Process(move(email));
    }
  }

public:
  void SetNext(unique_ptr<Worker> next) {
    next_ = move(next);
  }

private:
  unique_ptr<Worker> next_;
};


class Reader : public Worker {
public:
  Reader(istream& s)
    : s_(s)
  {}

  void Run() override {
    while (s_) {
      auto e = make_unique<Email>();
      if (s_ >> *e) {
        Process(move(e));
      }
    }
  }

  void Process(unique_ptr<Email> email) override {
    PassOn(move(email));
  }

private:
  istream& s_;
};


class Filter : public Worker {
public:
  using Function = function<bool(const Email&)>;

public:
  Filter(Function filter)
    : filter_(move(filter))
  {}

  void Process(unique_ptr<Email> email) override {
    if (filter_(*email)) {
      PassOn(move(email));
    }
  }

private:
  Function filter_;
};


class Copier : public Worker {
public:
  Copier(string recipient)
    : recipient_(move(recipient))
  {}

  void Process(unique_ptr<Email> email) override {
    unique_ptr<Email> copy;
    if (email->to != recipient_) {
      copy = make_unique<Email>(*email);
      copy->to = recipient_;
    }
    PassOn(move(email));
    if (copy) {
      PassOn(move(copy));
    }
  }

private:
  string recipient_;
};


class Sender : public Worker {
public:
  Sender(ostream& s)
    : s_(s)
  {}

  void Process(unique_ptr<Email> email) override {
    s_ << *email;
    PassOn(move(email));
  }

private:
  ostream& s_;
};


class PipelineBuilder {
public:
  explicit PipelineBuilder(istream& in) {
    AppendWorker(make_unique<Reader>(in));
  }

  PipelineBuilder& FilterBy(Filter::Function filter) {
    return AppendWorker(make_unique<Filter>(move(filter)));
  }

  PipelineBuilder& CopyTo(string recipient) {
    return AppendWorker(make_unique<Copier>(move(recipient)));
  }

  PipelineBuilder& Send(ostream& out) {
    return AppendWorker(make_unique<Sender>(out));
  }

  unique_ptr<Worker> Build() {
    last_ = nullptr;
    return move(first_);
  }

private:
  unique_ptr<Worker> first_;
  Worker* last_ = nullptr;

  PipelineBuilder& AppendWorker(unique_ptr<Worker> next) {
    if (!last_) {
      first_ = move(next);
      last_ = first_.get();
    } else {
      auto new_last = next.get();
      last_->SetNext(move(next));
      last_ = new_last;
    }
    return *this;
  }
};


void TestSanity() {
  string input = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "ralph@example.com\n"
    "erich@example.com\n"
    "I do not make mistakes of that kind\n"
  );
  istringstream inStream(input);
  ostringstream outStream;

  PipelineBuilder builder(inStream);
  builder.FilterBy([](const Email& email) {
    return email.from == "erich@example.com";
  });
  builder.CopyTo("richard@example.com");
  builder.Send(outStream);
  auto pipeline = builder.Build();

  pipeline->Run();

  string expectedOutput = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "erich@example.com\n"
    "richard@example.com\n"
    "Are you sure you pressed the right button?\n"
  );

  ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSanity);
  return 0;
}
