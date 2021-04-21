#pragma once

#include "memory"

#include "date.h"

enum class LogicalOperation {
    And,
    Or
};

enum class Comparison {
    Less,
    LessOrEqual,
    Greater,
    GreaterOrEqual,
    Equal,
    NotEqual
};

class Node {
public:
    virtual ~Node() = default;
    virtual bool Evaluate(const Date& date, const std::string& event) = 0;
};

using NodePtr = std::shared_ptr<Node>;

class EmptyNode : public Node {
public:
    bool Evaluate(const Date& date, const std::string& event) override;
};

class DateComparisonNode : public Node {
public:
    DateComparisonNode(Comparison cmp, const Date& date);
    bool Evaluate(const Date& date, const std::string& event) override;

private:
    const Comparison cmp_;
    const Date date_;
};

class EventComparisonNode : public Node {
public:
    EventComparisonNode(Comparison cmp, const std::string& event);
    bool Evaluate(const Date& date, const std::string& event) override;

private:
    const Comparison cmp_;
    const std::string event_;
};

class LogicalOperationNode : public Node {
public:
    LogicalOperationNode(LogicalOperation op, NodePtr lhs, NodePtr rhs);
    bool Evaluate(const Date& date, const std::string& event) override;

private:
    const LogicalOperation op_;
    const NodePtr lhs_;
    const NodePtr rhs_;
};