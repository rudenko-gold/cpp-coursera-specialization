#include "Common.h"
#include "test_runner.h"

#include <sstream>

using namespace std;

using ExpressionPtr = std::unique_ptr<Expression>;

class ValueExpression : public Expression {
public:
    explicit ValueExpression(int value) : value(value) {}

    int Evaluate() const override {
        return value;
    }

    string ToString() const override {
        return to_string(value);
    }

private:
    int value = 0;
};

enum Operation {
    SUM = '+',
    DIFFERENCE = '-',
    PRODUCT = '*',
    DIVISION = '/'
};

class OperationExpression : public Expression {
public:
    explicit OperationExpression(Operation op, ExpressionPtr left, ExpressionPtr right) :
        operation(op),
        leftExpression(move(left)),
        rightExpression(move(right)) {

    }

    int Evaluate() const override {
        int leftValue = leftExpression.get()->Evaluate();
        int rightValue = rightExpression.get()->Evaluate();
        switch (operation) {
            case SUM:
                return leftValue + rightValue;
                break;
            case PRODUCT:
                return leftValue * rightValue;
                break;
            case DIFFERENCE:
                return leftValue - rightValue;
                break;
            case DIVISION:
                return leftValue / rightValue;
                break;
        }
    }

    string ToString() const override {
        return "(" + leftExpression.get()->ToString() + ")" +
               (char)operation +
               "(" + rightExpression.get()->ToString() + ")";
    }

private:
    ExpressionPtr leftExpression, rightExpression;
    Operation operation;
};

ExpressionPtr Value(int value) {
    return make_unique<ValueExpression>(value);
}

ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
    return make_unique<OperationExpression>(Operation::SUM, move(left), move(right));
}

ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
    return make_unique<OperationExpression>(Operation::PRODUCT, move(left), move(right));
}

string Print(const Expression* e) {
    if (!e) {
        return "Null expression provided";
    }
    stringstream output;
    output << e->ToString() << " = " << e->Evaluate();
    return output.str();
}

void Test() {
    ExpressionPtr e1 = Product(Value(2), Sum(Value(3), Value(4)));
    ASSERT_EQUAL(Print(e1.get()), "(2)*((3)+(4)) = 14");

    ExpressionPtr e2 = Sum(move(e1), Value(5));
    ASSERT_EQUAL(Print(e2.get()), "((2)*((3)+(4)))+(5) = 19");

    ASSERT_EQUAL(Print(e1.get()), "Null expression provided");
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, Test);
    return 0;
}
