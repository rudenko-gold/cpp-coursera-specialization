#pragma once

#include <istream>
#include <map>
#include <string>
#include <variant>
#include <vector>
#include <sstream>
#include <iomanip>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& data) {
    os << "[ ";
    for (size_t i = 0; i < data.size(); ++i) {
        const auto& item = data[i];
        os << item;
        if (i != data.size() - 1) {
            os << ", ";
        }
    }
    os << " ]";
    return os;
}

template<typename Key, typename Value>
std::ostream& operator<<(std::ostream& os, const std::map<Key, Value>& data) {
    os << "{ ";
    auto it = data.cbegin();
    for (; it != --data.cend(); it = std::next(it)) {

        const auto& key = it->first;
        const auto& value = it->second;

        os << key << ": " << value << ", ";
    }
    const auto& key = it->first;
    const auto& value = it->second;

    os << key << ": " << value;

    os << " }";

    return os;
}

namespace Json {
    class Node : std::variant<std::vector<Node>,
            std::map<std::string, Node>,
            int,
            double,
            bool,
            std::string> {
    public:
        using variant::variant;

        const auto& AsArray() const {
            return std::get<std::vector<Node>>(*this);
        }
        const auto& AsMap() const {
            return std::get<std::map<std::string, Node>>(*this);
        }
        int AsInt() const {
            return std::get<int>(*this);
        }
        double AsDouble() const {
            return std::get<double>(*this);
        }
        bool AsBool() const {
            return std::get<bool>(*this);
        }
        const auto& AsString() const {
            return std::get<std::string>(*this);
        }

    };

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root;
    };

    Document Load(std::istream& input);

}