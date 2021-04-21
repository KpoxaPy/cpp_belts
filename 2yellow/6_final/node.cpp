#include "node.h"

bool EmptyNode::Evaluate(const Date& date, const std::string& event) {
    return true;
}

DateComparisonNode::DateComparisonNode(Comparison cmp, const Date& date)
    : cmp_(cmp)
    , date_(date)
{}

bool DateComparisonNode::Evaluate(const Date& date, const std::string& event) {
    switch (cmp_) {
    case Comparison::Equal:
        return date == date_;
    case Comparison::NotEqual:
        return date != date_;
    case Comparison::Less:
        return date < date_;
    case Comparison::LessOrEqual:
        return date <= date_;
    case Comparison::Greater:
        return date > date_;
    case Comparison::GreaterOrEqual:
        return date >= date_;
    }
    return false;
}

EventComparisonNode::EventComparisonNode(Comparison cmp, const std::string& event)
    : cmp_(cmp)
    , event_(event)
{}

bool EventComparisonNode::Evaluate(const Date& date, const std::string& event) {
    switch (cmp_) {
    case Comparison::Equal:
        return event == event_;
    case Comparison::NotEqual:
        return event != event_;
    case Comparison::Less:
        return event < event_;
    case Comparison::LessOrEqual:
        return event <= event_;
    case Comparison::Greater:
        return event > event_;
    case Comparison::GreaterOrEqual:
        return event >= event_;
    }
    return false;
}

LogicalOperationNode::LogicalOperationNode(LogicalOperation op, NodePtr lhs, NodePtr rhs)
    : op_(op)
    , lhs_(lhs)
    , rhs_(rhs)
{}

bool LogicalOperationNode::Evaluate(const Date& date, const std::string& event) {
    if (op_ == LogicalOperation::And) {
        return lhs_->Evaluate(date, event) && rhs_->Evaluate(date, event);
    } else if (op_ == LogicalOperation::Or) {
        return lhs_->Evaluate(date, event) || rhs_->Evaluate(date, event);
    }
    return false;
}