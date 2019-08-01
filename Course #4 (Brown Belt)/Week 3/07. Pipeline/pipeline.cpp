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

ostream& operator<<(ostream& os, Email& email) {
    os << email.from << endl << email.to << endl << email.body << endl;
    return os;
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
        if (nextWorker) {
            nextWorker.get()->Process(move(email));
        }
    }
    unique_ptr<Worker> nextWorker;
public:
    void SetNext(unique_ptr<Worker> next) {
        nextWorker = move(next);
    }
};


class Reader : public Worker {
public:
    explicit Reader(istream& is)  {
        while (is) {
            Email email = {"invalid", "invalid", "invalid"};
            if (!getline(is, email.from)) {
                break;
            }
            if (!getline(is, email.to)) {
                break;
            }
            if (!getline(is, email.body)) {
                break;
            }

            emails.push_back(email);
        }
    }

    void Run() override  {
        for (auto email : emails) {
            PassOn(move(make_unique<Email>(email)));
        }
    }

    void Process(unique_ptr<Email> email) override {
        PassOn(move(email));
    }
private:
    vector<Email> emails;
};


class Filter : public Worker {
public:
    using Function = function<bool(const Email&)>;

    explicit Filter(Function func) : func(func) {}
    void Process(unique_ptr<Email> email) override {
        if (func(*email.get())) {
            PassOn(move(email));
        }
    }

private:
    Function func;
};


class Copier : public Worker {
public:
    explicit Copier(const string& address) : recipient(address) {}

    void Process(unique_ptr<Email> email) override {
        if (email.get()->to != recipient) {
            unique_ptr<Email> copy = make_unique<Email>();

            copy.get()->body = email.get()->body;
            copy.get()->from = email.get()->from;
            copy.get()->to = recipient;

            PassOn(move(email));
            PassOn(move(copy));
        } else {
            PassOn(move(email));
        }
    }
private:
    string recipient;
};


class Sender : public Worker {
public:
    explicit Sender(ostream& os) : os(os) {}

    void Process(unique_ptr<Email> email) override {
        os << *email.get();
        PassOn(move(email));
    }
private:
    ostream& os;
};

class PipelineBuilder {
public:
    explicit PipelineBuilder(istream& in) {
        workers.push_back(make_unique<Reader>(in));
    }

    PipelineBuilder& FilterBy(Filter::Function filter) {
        workers.push_back(make_unique<Filter>(filter));
        return *this;
    }

    PipelineBuilder& CopyTo(string recipient) {
        workers.push_back(make_unique<Copier>(recipient));
        return *this;
    }

    PipelineBuilder& Send(ostream& out) {
        workers.push_back(make_unique<Sender>(out));
        return *this;
    }

    unique_ptr<Worker> Build() {
        for (auto it = workers.rbegin() + 1; it != workers.rend(); ++it) {
            it->get()->SetNext(move(*(it - 1)));
        }

        return move(workers[0]);
    }

private:
    vector<unique_ptr<Worker>> workers;
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
