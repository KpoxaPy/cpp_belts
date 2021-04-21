#pragma once

#include <string>
#include <memory>

#include "database.h"

enum class CommandType {
    ADD,
    PRINT,
    DEL,
    FIND,
    LAST,
    EMPTY
};

class Command {
public:
    Command(CommandType t);
    virtual ~Command() = default;
    virtual void Evaluate(Database& db) const = 0;

private:
    const CommandType t_; 
};

std::shared_ptr<Command> ParseCommand(const std::string& line);