#include "lexer.h"

#include <algorithm>
#include <charconv>
#include <unordered_map>

using namespace std;

namespace Parse {

    bool operator == (const Token& lhs, const Token& rhs) {
        using namespace TokenType;

        if (lhs.index() != rhs.index()) {
            return false;
        }
        if (lhs.Is<Char>()) {
            return lhs.As<Char>().value == rhs.As<Char>().value;
        } else if (lhs.Is<Number>()) {
            return lhs.As<Number>().value == rhs.As<Number>().value;
        } else if (lhs.Is<String>()) {
            return lhs.As<String>().value == rhs.As<String>().value;
        } else if (lhs.Is<Id>()) {
            return lhs.As<Id>().value == rhs.As<Id>().value;
        } else {
            return true;
        }
    }

    std::ostream& operator << (std::ostream& os, const Token& rhs) {
        using namespace TokenType;

#define VALUED_OUTPUT(type) \
  if (auto p = rhs.TryAs<type>()) return os << #type << '{' << p->value << '}';

        VALUED_OUTPUT(Number);
        VALUED_OUTPUT(Id);
        VALUED_OUTPUT(String);
        VALUED_OUTPUT(Char);

#undef VALUED_OUTPUT

#define UNVALUED_OUTPUT(type) \
    if (rhs.Is<type>()) return os << #type;

        UNVALUED_OUTPUT(Class);
        UNVALUED_OUTPUT(Return);
        UNVALUED_OUTPUT(If);
        UNVALUED_OUTPUT(Else);
        UNVALUED_OUTPUT(Def);
        UNVALUED_OUTPUT(Newline);
        UNVALUED_OUTPUT(Print);
        UNVALUED_OUTPUT(Indent);
        UNVALUED_OUTPUT(Dedent);
        UNVALUED_OUTPUT(And);
        UNVALUED_OUTPUT(Or);
        UNVALUED_OUTPUT(Not);
        UNVALUED_OUTPUT(Eq);
        UNVALUED_OUTPUT(NotEq);
        UNVALUED_OUTPUT(LessOrEq);
        UNVALUED_OUTPUT(GreaterOrEq);
        UNVALUED_OUTPUT(None);
        UNVALUED_OUTPUT(True);
        UNVALUED_OUTPUT(False);
        UNVALUED_OUTPUT(Eof);

#undef UNVALUED_OUTPUT

        return os << "Unknown token :(";
    }


    Lexer::IndentController::IndentController() {
        indent_next_ = true;
        current_indents_ = 0;

        needed_indents_ = 0;
        needed_dedents_ = 0;

        need_new_line_ = true;
    }

    Lexer::Lexer(std::istream& input) : stream(input) {
        current_token = readToken();
        indentController.notIndentNext();
    }

    const Token& Lexer::CurrentToken() const {
        return current_token;
    }

    Token Lexer::NextToken() {
        current_token = readToken();
        return current_token;
    }

    Token Lexer::readToken() {
        char c = stream.get();

        if (indentController.isIndentNext()) {
            stream.putback(c);
            const auto result = indentController.updateIndent(stream);
            if (result)
                return *result;
            return readToken();
        }

        if (c == -1) {
            if (indentController.needNewLine()) {
                indentController.setEof();
                return TokenType::Newline{};
            }
            return TokenType::Eof{};
        }

        if (c == '\n') {
            indentController.indentNext();
            indentController.setNewLine();
            return TokenType::Newline{};
        }

        if (std::isdigit(c)) {
            stream.putback(c);
            return readNumber();
        }

        if (is_char_token(c)) {
            return TokenType::Char{c};
        }

        if (c == '\'' || c == '\"') {
            stream.putback(c);
            return readString();
        }

        if (c == ' ') {
            return readToken();
        }

        stream.putback(c);
        return readIdOrDeclaredToken();
    }

    TokenType::Number Lexer::readNumber() {
        int result = 0;

        while (std::isdigit(stream.peek())) {
            result *= 10;
            result += stream.get() - '0';
        }

        return TokenType::Number{result};
    }

    bool Lexer::is_char_token(const char &c) const {
        return char_tokens.find(c) != char_tokens.end() && !isBoolToken(c);
    }

    bool Lexer::isBoolToken(const char &c) const {
        return
                (c == '!' && stream.peek() == '=') ||
                (c == '=' && stream.peek() == '=') ||
                (c == '<' && stream.peek() == '=') ||
                (c == '>' && stream.peek() == '=');
    }

    TokenType::String Lexer::readString() {
        return TokenType::String{readStringToEnd(stream.get())};
    }

    std::string Lexer::readStringToEnd(const char &end) {
        std::string result;
        std::getline(stream, result, end);

        return result;
    }

    Token Lexer::readIdOrDeclaredToken() {
        char c = stream.get();

        if (isBoolToken(c)) {
            stream.putback(c);
            return readBoolToken();
        }

        stream.putback(c);

        std::string token_str = "";

        while (
                stream.peek() != ' ' &&
                stream.peek() != '\n' &&
                stream.peek() != -1 &&
                !is_char_token(stream.peek())
                ) {
            token_str += stream.get();
        }

        const auto it = declared_tokens_.find(token_str);
        if (it != declared_tokens_.end()) {
            return it->second;
        }

        return TokenType::Id{token_str};
    }


    Token Lexer::readBoolToken() {
        std::string token_str;
        token_str += stream.get();
        token_str += stream.get();

        return bool_tokens_.at(token_str);
    }

    std::optional<Token> Lexer::IndentController::updateIndent(std::istream& in) {
        if (needed_indents_ > 0) {
            --needed_indents_;
            ++current_indents_;
            update_next();
            return TokenType::Indent{};
        }

        if (needed_dedents_ > 0) {
            --needed_dedents_;
            --current_indents_;
            update_next();
            return TokenType::Dedent{};
        }

        unsigned spaces = 0;
        while (in.peek() == ' ') {
            in.get();
            ++spaces;
        }

        if (in.peek() == '\n') {
            in.get();
            return std::nullopt;
        } else if (in.peek() != -1) {
            need_new_line_ = true;
        }

        spaces = spaces % 2 == 0 ? spaces / 2 : spaces / 2 + 1;

        if (spaces == current_indents_) {
            indent_next_ = false;
            return std::nullopt;
        } else if (spaces > current_indents_) {
            needed_indents_ = spaces - current_indents_;
            --needed_indents_;
            ++current_indents_;
            update_next();
            return TokenType::Indent{};
        } else {
            needed_dedents_ = current_indents_ - spaces;
            --needed_dedents_;
            --current_indents_;
            update_next();
            return TokenType::Dedent{};
        }
    }

} /* namespace Parse */
