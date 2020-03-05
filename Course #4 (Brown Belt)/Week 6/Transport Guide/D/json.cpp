#include <iostream>
#include "json.h"

using namespace std;

namespace Json {

    Document::Document(Node root) : root(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root;
    }

    Node LoadNode(istream& input);

    Node LoadArray(istream& input) {
        vector<Node> result;

        for (char c; input >> c && c != ']'; ) {
            if (c != ',') {
                input.putback(c);
            }
            result.push_back(LoadNode(input));
        }

        return Node(move(result));
    }

    Node LoadInt(istream& input) {
        int result = 0;
        if (input.peek() == '-') {
            input.ignore(1);
            while (isdigit(input.peek())) {
                result *= 10;
                result += input.get() - '0';
            }
            result *= -1;
        } else {
            while (isdigit(input.peek())) {
                result *= 10;
                result += input.get() - '0';
            }
        }
        //std::cerr << result << std::endl;
        return Node(result);
    }

    Node LoadBool(istream& input) {
        string result;
        while (isalpha(input.peek())) {
            result += input.get();
        }
        //std::cerr << result << std::endl;
        if (result == "true") {
            return Node(true);
        } else {
            return Node(false);
        }
    }

    Node LoadDouble(istream& input) {
        double result = 0.0;
        input >> result;
        //std::cerr << result << std::endl;
        return Node(result);
    }

    Node LoadString(istream& input) {
        string line;
        getline(input, line, '"');
        return Node(move(line));
    }

    Node LoadDict(istream& input) {
        map<string, Node> result;

        for (char c; input >> c && c != '}'; ) {
            if (c == ',') {
                input >> c;
            }

            string key = LoadString(input).AsString();
            input >> c;
            result.emplace(move(key), LoadNode(input));
        }

        return Node(move(result));
    }

    Node LoadNode(istream& input) {
        char c;
        input >> c;

        if (c == '[') {
            return LoadArray(input);
        } else if (c == '{') {
            return LoadDict(input);
        } else if (c == '"') {
            return LoadString(input);
        } else if (c == 't' || c == 'f') {
            input.putback(c);
            return LoadBool(input);
        } else {
            input.putback(c);
            stringstream ss;
            bool is_int = true;

            while (isdigit(input.peek()) || input.peek() == '.' || input.peek() == '-') {
                if (input.peek() == '.') {
                    is_int = false;
                }
                char c = input.get();
                ss << c;
                //std::cerr << c;
            }
            //std::cerr << " " << is_int <<  " \n";

            if (is_int) {
                return LoadInt(ss);
            } else {
                return LoadDouble(ss);
            }
        }
    }

    Document Load(istream& input) {
        return Document{LoadNode(input)};
    }

}