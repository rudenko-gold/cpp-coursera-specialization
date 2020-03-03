#pragma once

#include <chrono>
#include <iostream>
#include <string>

using namespace std;
using namespace std::chrono;

class LogDuration {
public:
    explicit LogDuration(const string& message = "")
            : message_(message + ": ")
            , start_(steady_clock::now())
    {
    }

    ~LogDuration() {
        auto finish = steady_clock::now();
        auto dur = finish - start_;
        cerr << message_
             << duration_cast<milliseconds>(dur).count()
             << " ms" << endl;
    }
private:
    string message_;
    steady_clock::time_point start_;
};

#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define LOG_DURATION(message) \
  LogDuration UNIQ_ID(__LINE__){message};
