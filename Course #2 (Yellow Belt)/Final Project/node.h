#pragma once

#include "date.h"
#include <memory>

enum class Comparison {
    Less,
    LessOrEqual,
    Greater,
    GreaterOrEqual,
    Equal,
    NotEqual
};

enum class LogicalOperation {
    And,
    Or
};

class Node {
public:
    virtual bool Evaluate(const Date& date, const string& event) const = 0;
};

class EmptyNode : public Node {
    bool Evaluate(const Date& date, const string& event) const override;
};

class DateComparisonNode : public Node {
public:
    DateComparisonNode(const Comparison& cmp, const Date date) : cmp_(cmp), date_(date) {}
    bool Evaluate(const Date& date, const string& event) const override;

private:
    const Comparison cmp_;
    const Date date_;
};

class EventComparisonNode : public Node {
public:
    EventComparisonNode(const Comparison& cmp, const string& event) : cmp_(cmp), event_(event) {}
    bool Evaluate(const Date& date, const string& event) const override;

private:
    const Comparison cmp_;
    const string event_;
};

class LogicalOperationNode : public Node {
public:
    LogicalOperationNode(
        const LogicalOperation& op, 
        shared_ptr<const Node> left, 
        shared_ptr<const Node> right
    ) : op_(op), left_(left), right_(right) { }
    bool Evaluate(const Date& date, const string& event) const override;

private:
    shared_ptr<const Node> left_, right_;
    const LogicalOperation op_;
};