#include "lexer.h"
#include "test_runner.h"

#include <string>
#include <sstream>

using namespace std;

namespace Parse {

    void TestSimpleAssignment() {
        istringstream input("x = 42\n");
        Lexer lexer(input);

        ASSERT_EQUAL(lexer.CurrentToken(), Token(TokenType::Id{"x"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'='}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Number{42}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Eof{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Eof{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Eof{}));
    }

    void TestKeywords() {
        istringstream input("class return if else def print or None and not True False");
        Lexer lexer(input);

        ASSERT_EQUAL(lexer.CurrentToken(), Token(TokenType::Class{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Return{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::If{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Else{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Def{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Print{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Or{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::None{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::And{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Not{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::True{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::False{}));
    }

    void TestNumbers() {
        istringstream input("42 15 -53");
        Lexer lexer(input);

        ASSERT_EQUAL(lexer.CurrentToken(), Token(TokenType::Number{42}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Number{15}));
        // РћС‚СЂРёС†Р°С‚РµР»СЊРЅС‹Рµ С‡РёСЃР»Р° С„РѕСЂРјРёСЂСѓСЋС‚СЃСЏ РЅР° СЌС‚Р°РїРµ СЃРёРЅС‚Р°РєСЃРёС‡РµСЃРєРѕРіРѕ Р°РЅР°Р»РёР·Р°
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'-'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Number{53}));
    }

    void TestIds() {
        istringstream input("x    _42 big_number   Return Class  dEf");
        Lexer lexer(input);

        ASSERT_EQUAL(lexer.CurrentToken(), Token(TokenType::Id{"x"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"_42"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"big_number"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"Return"})); // keywords are case-sensitive
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"Class"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"dEf"}));
    }

    void TestStrings() {
        istringstream input(R"('word' "two words" 'long string with a double quote " inside' "another long string with single quote ' inside")");
        Lexer lexer(input);

        ASSERT_EQUAL(lexer.CurrentToken(), Token(TokenType::String{"word"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::String{"two words"}));
        ASSERT_EQUAL(
                lexer.NextToken(), Token(TokenType::String{"long string with a double quote \" inside"})
        );
        ASSERT_EQUAL(
                lexer.NextToken(), Token(TokenType::String{"another long string with single quote ' inside"})
        );
    }

    void TestOperations() {
        istringstream input("+-*/= > < != == <> <= >=");
        Lexer lexer(input);

        ASSERT_EQUAL(lexer.CurrentToken(), Token(TokenType::Char{'+'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'-'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'*'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'/'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'='}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'>'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'<'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::NotEq{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Eq{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'<'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'>'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::LessOrEq{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::GreaterOrEq{}));
    }

    void TestIndentsAndNewlines() {
        istringstream input(R"(
no_indent
  indent_one
    indent_two
      indent_three
      indent_three
      indent_three
    indent_two
  indent_one
    indent_two
no_indent
)");

        Lexer lexer(input);

        ASSERT_EQUAL(lexer.CurrentToken(), Token(TokenType::Id{"no_indent"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Indent{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"indent_one"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Indent{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"indent_two"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Indent{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"indent_three"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"indent_three"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"indent_three"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Dedent{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"indent_two"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Dedent{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"indent_one"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Indent{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"indent_two"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Dedent{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Dedent{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"no_indent"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Eof{}));
    }

    void TestEmptyLinesAreIgnored() {
        istringstream input(R"(
x = 1
  y = 2

  z = 3


)");
        Lexer lexer(input);

        ASSERT_EQUAL(lexer.CurrentToken(), Token(TokenType::Id{"x"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'='}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Number{1}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Indent{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"y"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'='}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Number{2}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        // РџСѓСЃС‚Р°СЏ СЃС‚СЂРѕРєР°, СЃРѕСЃС‚РѕСЏС‰Р°СЏ С‚РѕР»СЊРєРѕ РёР· РїСЂРѕР±РµР»СЊРЅС‹С… СЃРёРјРІРѕР»РѕРІ РЅРµ РјРµРЅСЏРµС‚ С‚РµРєСѓС‰РёР№ РѕС‚СЃС‚СѓРї,
        // РїРѕСЌС‚РѕРјСѓ СЃР»РµРґСѓСЋС‰Р°СЏ Р»РµРєСЃРµРјР° вЂ” СЌС‚Рѕ Id, Р° РЅРµ Dedent
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"z"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'='}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Number{3}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Dedent{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Eof{}));
    }

    void TestMythonProgram() {
        istringstream input(R"(
x = 4
y = "hello"

class Point:
  def __init__(self, x, y):
    self.x = x
    self.y = y

  def __str__(self):
    return str(x) + ' ' + str(y)

p = Point(1, 2)
print str(p)
)");
        Lexer lexer(input);

        using namespace TokenType;

        ASSERT_EQUAL(lexer.CurrentToken(), Token(TokenType::Id{"x"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'='}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Number{4}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"y"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'='}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::String{"hello"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Class{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"Point"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{':'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Indent{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Def{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"__init__"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'('}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"self"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{','}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"x"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{','}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"y"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{')'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{':'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Indent{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"self"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'.'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"x"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'='}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"x"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"self"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'.'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"y"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'='}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"y"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Dedent{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Def{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"__str__"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'('}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"self"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{')'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{':'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Indent{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Return{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"str"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'('}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"x"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{')'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'+'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::String{" "}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'+'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"str"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'('}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"y"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{')'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Dedent{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Dedent{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"p"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'='}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"Point"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'('}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Number{1}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{','}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Number{2}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{')'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Print{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"str"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{'('}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"p"}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Char{')'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Eof{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Eof{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Eof{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Eof{}));
    }

    void TestExpect() {
        istringstream is("bugaga");
        Lexer lex(is);

        ASSERT_DOESNT_THROW(lex.Expect<TokenType::Id>());
        ASSERT_EQUAL(lex.Expect<TokenType::Id>().value, "bugaga");
        ASSERT_DOESNT_THROW(lex.Expect<TokenType::Id>("bugaga"));
        ASSERT_THROWS(lex.Expect<TokenType::Id>("widget"), LexerError);
        ASSERT_THROWS(lex.Expect<TokenType::Return>(), LexerError);
    }

    void TestExpectNext() {
        istringstream is("+ bugaga + def 52");
        Lexer lex(is);

        ASSERT_EQUAL(lex.CurrentToken(), Token(TokenType::Char{'+'}));
        ASSERT_DOESNT_THROW(lex.ExpectNext<TokenType::Id>());
        ASSERT_DOESNT_THROW(lex.ExpectNext<TokenType::Char>('+'));
        ASSERT_THROWS(lex.ExpectNext<TokenType::Newline>(), LexerError);
        ASSERT_THROWS(lex.ExpectNext<TokenType::Number>(57), LexerError);
    }

    void TestAlwaysEmitsNewlineAtTheEndOfNonemptyLine() {
        {
            istringstream is("a b");
            Lexer lexer(is);

            ASSERT_EQUAL(lexer.CurrentToken(), Token(TokenType::Id{"a"}));
            ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Id{"b"}));
            ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
            ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Eof{}));
            ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Eof{}));
            ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Eof{}));
        }
        {
            istringstream is("+");
            Lexer lexer(is);

            ASSERT_EQUAL(lexer.CurrentToken(), Token(TokenType::Char{'+'}));
            ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Newline{}));
            ASSERT_EQUAL(lexer.NextToken(), Token(TokenType::Eof{}));
        }
    }

    void RunLexerTests(TestRunner& tr) {
        RUN_TEST(tr, Parse::TestSimpleAssignment);
        RUN_TEST(tr, Parse::TestKeywords);
        RUN_TEST(tr, Parse::TestNumbers);
        RUN_TEST(tr, Parse::TestIds);
        RUN_TEST(tr, Parse::TestStrings);
        RUN_TEST(tr, Parse::TestOperations);
        RUN_TEST(tr, Parse::TestIndentsAndNewlines);
        RUN_TEST(tr, Parse::TestEmptyLinesAreIgnored);
        RUN_TEST(tr, Parse::TestExpect);
        RUN_TEST(tr, Parse::TestExpectNext);
        RUN_TEST(tr, Parse::TestMythonProgram);
        RUN_TEST(tr, Parse::TestAlwaysEmitsNewlineAtTheEndOfNonemptyLine);
    }

} /* namespace Parse */

int main() {
    TestRunner tr;
    Parse::RunLexerTests(tr);
    return 0;
}
