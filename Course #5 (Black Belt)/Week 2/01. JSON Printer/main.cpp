#include "test_runner.h"

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#include <utility>

class EmptyContext {};

void PrintJsonString(std::ostream& out, std::string_view str) {
    out << "\"";
    for (const char& ch : str) {
        if (ch == '"') {
            out << R"(\")";
        } else if (ch == '\\') {
            out << R"(\\)";
        } else {
            out << ch;
        }
    }
    out << "\"";
}

class JsonObject {
public:
    JsonObject(std::ostream& os) : out(os) {}
    virtual ~JsonObject() = default;

protected:
    std::ostream& out;
};

class JsonNumber : public JsonObject {
public:
    JsonNumber(std::ostream& os, int64_t number) : JsonObject(os), value(number) {}

    ~JsonNumber() override {
        out << value;
    }
private:
    int64_t value;
};

class JsonString : public JsonObject {
public:
    JsonString(std::ostream& os, std::string_view s) : JsonObject(os), value(s) {}

    ~JsonString() override {
        PrintJsonString(out, value);
}
private:
    std::string_view value;
};

class JsonBool : public JsonObject {
public:
    JsonBool(std::ostream& os, bool flag) : JsonObject(os), value(flag) {}

    ~JsonBool() override {
        if (value) {
            out << "true";
        } else {
            out << "false";
        }
    }
private:
    bool value;
};

class JsonNull : public JsonObject {
public:
    JsonNull(std::ostream& os) : JsonObject(os) {}
    ~JsonNull() override  {
        out << "null";
    }
};


class JsonMap : public JsonObject {
public:
    JsonMap(std::ostream& os) : JsonObject(os) {}

    std::pair<std::string, JsonObject*>& AddKey(const std::string& key) {
        JsonObject* null_value = new JsonNull(out);

        data.emplace_back(key, null_value);
        return data.back();
    }

    ~JsonMap() override {
        out << "{";
        bool first = true;
        for (const auto& [key, value] : data) {
            if (first) {
                PrintJsonString(out, key);
                out << ":";
                delete value;
                first = false;
            } else {
                out << ",";
                PrintJsonString(out, key);
                out << ":";
                delete value;
            }
        }

        out << "}";
    }

private:
    std::vector<std::pair<std::string, JsonObject*>> data;
};

class JsonArray : public JsonObject {
public:
    JsonArray(std::ostream& os) : JsonObject(os) {}

    void AddObj(JsonObject* obj) {
        data.push_back(obj);
    }

    ~JsonArray() override {
        bool first = true;
        out << "[";
        for (const auto& ptr : data) {
            if (first) {
                delete ptr;
                first = false;
            } else {
                out << ",";
                delete ptr;
            }
        }
        out << "]";
    }

private:
    std::vector<JsonObject*> data;
};

template <class Parent>
class InMapKeyOrEndExpected;
template <class Parent>
class InMapValueExpected;

template <class Parent>
class InArray : public EmptyContext {
public:
    InArray(std::ostream& os, Parent* p) : out(os) {
        data = new JsonArray(out);
        parent = p;
    }

    InArray(std::ostream& os, Parent* p, JsonArray* array) : out(os) {
        data = array;
        parent = p;
    }

    InArray& Number(int64_t number) {
        data->AddObj(new JsonNumber(out, number));
        return *this;
    }

    InArray& String(std::string_view str) {
        data->AddObj(new JsonString(out, str));
        return *this;
    }

    InArray& Boolean(bool flag) {
        data->AddObj(new JsonBool(out, flag));
        return *this;
    }

    InArray& Null() {
        data->AddObj(new JsonNull(out));
        return *this;
    }

    InMapKeyOrEndExpected<InArray> BeginObject() {
        auto* obj_ptr = new JsonMap(out);

        data->AddObj(obj_ptr);

        return InMapKeyOrEndExpected<InArray>(out, this, obj_ptr);
    }

    Parent& EndArray() {
        return *parent;
    }

    ~InArray() {
        if (typeid(parent) == typeid(EmptyContext*)) {
            delete data;
            delete parent;
        }
    }

    InArray<InArray> BeginArray() {
        auto* array = new JsonArray(out);
        data->AddObj(array);
        return InArray<InArray>(out, this, array);
    }

private:
    std::ostream& out;
    JsonArray* data;
    Parent* parent;
};

template <class Parent>
class InMapKeyOrEndExpected : public EmptyContext {
public:
    InMapKeyOrEndExpected(std::ostream& os, Parent* p) : out(os) {
        parent = p;
        data = new JsonMap(os);
    }

    InMapKeyOrEndExpected(std::ostream& os, Parent* p, JsonMap* data_ptr) : out(os) {
        parent = p;
        data = data_ptr;
    }

    InMapValueExpected<InMapKeyOrEndExpected> Key(std::string_view key) {
        std::pair<std::string, JsonObject*>& value = data->AddKey(std::string(key));
        return InMapValueExpected<InMapKeyOrEndExpected>(out, this, value);
    }

    Parent& EndObject() {
        return *parent;
    }

    ~InMapKeyOrEndExpected() {
        if (typeid(parent) == typeid(EmptyContext*)) {
            delete data;
            delete parent;
        }
    }
private:
    std::ostream& out;
    Parent* parent;
    JsonMap* data;
};

template <typename Parent>
class InMapValueExpected : public EmptyContext {
public:
    InMapValueExpected(std::ostream& os, Parent* p,  std::pair<std::string, JsonObject*>& obj) : out(os), value(obj) {
        parent = p;
    }

    Parent& Number(int64_t number) {
        value.second = new JsonNumber(out, number);
        return *parent;
    }

    Parent& String(std::string_view str) {
        value.second = new JsonString(out, str);
        return *parent;
    }

    Parent& Boolean(bool flag) {
        value.second = new JsonBool(out, flag);
        return *parent;
    }

    Parent& Null() {
        value.second = new JsonNull(out);
        return *parent;
    }

    auto BeginArray() {
        auto* array = new JsonArray(out);
        value.second = array;
        return InArray(out, parent, array);
    }

    auto BeginObject() {
        auto* map = new JsonMap(out);
        value.second = map;

        return InMapKeyOrEndExpected<Parent>(out, parent, map);
    }

private:
    std::ostream& out;
    std::pair<std::string, JsonObject*>& value;
    Parent* parent;
};

using ArrayContext = InArray<EmptyContext>;  // Р—Р°РјРµРЅРёС‚Рµ СЌС‚Рѕ РѕР±СЉСЏРІР»РµРЅРёРµ РЅР° РѕРїСЂРµРґРµР»РµРЅРёРµ С‚РёРїР° ArrayContext
ArrayContext PrintJsonArray(std::ostream& out) {
    return ArrayContext(out, new EmptyContext());
}

using ObjectContext = InMapKeyOrEndExpected<EmptyContext>;  // Р—Р°РјРµРЅРёС‚Рµ СЌС‚Рѕ РѕР±СЉСЏРІР»РµРЅРёРµ РЅР° РѕРїСЂРµРґРµР»РµРЅРёРµ С‚РёРїР° ObjectContext
ObjectContext PrintJsonObject(std::ostream& out) {
    return ObjectContext(out, new EmptyContext());
}

void TestArray() {
    std::ostringstream output;

    {
        auto json = PrintJsonArray(output);
        json
                .Number(5)
                .Number(6)
                .BeginArray()
                .Number(7)
                .EndArray()
                .Number(8)
                .String("bingo!");
    }

    ASSERT_EQUAL(output.str(), R"([5,6,[7],8,"bingo!"])");
}

void TestObject() {
    std::ostringstream output;

    {
        auto json = PrintJsonObject(output);
        json
                .Key("id1").Number(1234)
                .Key("id2").Boolean(false)
                .Key("").Null()
                .Key("\"").String("\\");
    }

    ASSERT_EQUAL(output.str(), R"({"id1":1234,"id2":false,"":null,"\"":"\\"})");
}

void TestAutoClose() {
    std::ostringstream output;

    {
        auto json = PrintJsonArray(output);
        json.BeginArray().BeginObject();
    }

    ASSERT_EQUAL(output.str(), R"([[{}]])");
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestArray);
    RUN_TEST(tr, TestObject);
    RUN_TEST(tr, TestAutoClose);

    return 0;
}
