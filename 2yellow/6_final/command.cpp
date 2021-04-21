#include "command.h"

#include <sstream>
#include <stdexcept>

#include "date.h"
#include "condition_parser.h"
#include "node.h"
#include "event.h"

using namespace std;

Command::Command(CommandType t)
    : t_(t)
{}


class EmptyCommand : public Command {
public:
    EmptyCommand()
        : Command(CommandType::EMPTY)
    {}

    void Evaluate(Database& db) const override
    {}
};


class PrintCommand : public Command {
public:
    PrintCommand()
        : Command(CommandType::PRINT)
    {}

    void Evaluate(Database& db) const override
    {
        db.Print(cout);
    }
};


class AddCommand : public Command {
public:
    AddCommand(const Date& date, const string& event)
        : Command(CommandType::ADD)
        , date_(date)
        , event_(event)
    {}

    void Evaluate(Database& db) const override
    {
        db.Add(date_, event_);
    }

private:
    const Date date_;
    const string event_;
};


class RemoveCommand : public Command {
public:
    RemoveCommand(shared_ptr<Node> condition)
        : Command(CommandType::DEL)
        , condition_(condition)
    {}

    void Evaluate(Database& db) const override
    {
        auto predicate = [*this](const Date &date, const string &event) {
            return condition_->Evaluate(date, event);
        };
        int count = db.RemoveIf(predicate);
        cout << "Removed " << count << " entries" << endl;
    }

private:
    const shared_ptr<Node> condition_;
};


class FindCommand : public Command {
public:
    FindCommand(shared_ptr<Node> condition)
        : Command(CommandType::FIND)
        , condition_(condition)
    {}

    void Evaluate(Database& db) const override
    {
        auto predicate = [*this](const Date &date, const string &event) {
            return condition_->Evaluate(date, event);
        };

        const auto entries = db.FindIf(predicate);
        for (const auto &entry : entries)
        {
            cout << entry << endl;
        }
        cout << "Found " << entries.size() << " entries" << endl;
    }

private:
    const shared_ptr<Node> condition_;
};


class LastCommand : public Command {
public:
    LastCommand(const Date& date)
        : Command(CommandType::LAST)
        , date_(date)
    {}

    void Evaluate(Database& db) const override
    {
        bool has_entry = date_.IsValid();
        Entry e;
        if (has_entry) {
            e = db.Last(date_);
            has_entry = e.date.IsValid();
        }

        if (!has_entry) {
            cout << "No entries" << endl;
        } else {
            cout << e << endl;
        }
    }

private:
    const Date date_;
};


shared_ptr<Command> ParseCommand(const string &line) {
    istringstream is(line);

    string command;
    is >> command;
    if (command == "Add") {
        const auto date = ParseDate(is);
        const auto event = ParseEvent(is);
        return make_shared<AddCommand>(date, event);
    } else if (command == "Print") {
        return make_shared<PrintCommand>();
    } else if (command == "Del") {
        auto condition = ParseCondition(is);
        return make_shared<RemoveCommand>(condition);
    } else if (command == "Find") {
        auto condition = ParseCondition(is);
        return make_shared<FindCommand>(condition);
    } else if (command == "Last") {
        Date date;
        try {
            date = ParseDate(is);
        } catch (invalid_argument &) {}
        return make_shared<LastCommand>(date);
    } else if (command.empty()) {
        return make_shared<EmptyCommand>();
    }
    throw logic_error("Unknown command: " + command);
}