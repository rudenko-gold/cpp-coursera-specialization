#include <string>
#include <list>
#include "test_runner.h"
#include <algorithm>

using namespace std;

class Editor {
public:
    Editor() {
        editor.push_back('|');
        cursor = editor.begin();
    }

    void Left() {
        if (cursor != editor.begin()) {
            *cursor = *prev(cursor);
            *prev(cursor) = '|';
            cursor = prev(cursor);
        }
    }

    void Right() {
        auto end = editor.end();
        end--;
        if (cursor != end) {
            *cursor = *next(cursor);
            *next(cursor) = '|';
            cursor = next(cursor);
        }
    }

    void Insert(char token) {
        editor.insert(cursor, token);
    }

    void Cut(size_t tokens = 1) {
        buffer.clear();

        if (tokens == 0) {
            return;
        }

        auto curEndIt = cursor;
        auto curStartIt = cursor;
        curStartIt++;
        while (tokens + 1 > 0 && curEndIt != editor.end()) {
            curEndIt++;
            tokens--;
        }
        editor.splice(buffer.begin(), editor, curStartIt, curEndIt);
    }

    void Copy(size_t tokens = 1) {
        buffer.clear();

        if (tokens == 0) {
            return;
        }

        auto curEndIt = cursor;
        auto curStartIt = cursor;
        curStartIt++;
        while (tokens + 1 > 0 && curEndIt != editor.end()) {
            curEndIt++;
            tokens--;
        }
        buffer.insert(buffer.begin(), curStartIt, curEndIt);
    }

    void Paste() {
        editor.insert(cursor, buffer.begin(), buffer.end());
    }

    string GetText() const {
        string text = "";

        auto it = editor.begin();
        while (it != editor.end()) {
            if (it != cursor) {
                text += *it;
            }
            it++;
        }

        return text;
    }

    string GetBuffer() const {
        string text(buffer.begin(), buffer.end());
        return text;
    }

private:
    list<char> editor;
    list<char>::iterator cursor;
    list<char> buffer;
};

void TypeText(Editor& editor, const string& text) {
    for(char c : text) {
        editor.Insert(c);
    }
}

void TestEditing() {
    {
        Editor editor;

        const size_t text_len = 12;
        const size_t first_part_len = 7;
        TypeText(editor, "hello, world");
        for(size_t i = 0; i < text_len; ++i) {
            editor.Left();
        }
        editor.Cut(first_part_len);
        for(size_t i = 0; i < text_len - first_part_len; ++i) {
            editor.Right();
        }
        TypeText(editor, ", ");
        editor.Paste();
        editor.Left();
        editor.Left();
        editor.Cut(3);
        ASSERT_EQUAL(editor.GetText(), "world, hello");
    }
    {
        Editor editor;

        TypeText(editor, "misprnit");
        editor.Left();
        editor.Left();
        editor.Left();
        editor.Cut(1);
        editor.Right();
        editor.Paste();

        ASSERT_EQUAL(editor.GetText(), "misprint");
    }
}

void TestReverse() {
    Editor editor;

    const string text = "esreveR";
    for(char c : text) {
        editor.Insert(c);
        editor.Left();
    }

    ASSERT_EQUAL(editor.GetText(), "Reverse");
}

void TestNoText() {
    Editor editor;
    ASSERT_EQUAL(editor.GetText(), "");

    editor.Left();
    editor.Left();
    editor.Right();
    editor.Right();
    editor.Copy(0);
    editor.Cut(0);
    editor.Paste();

    ASSERT_EQUAL(editor.GetText(), "");
}

void TestEmptyBuffer() {
    Editor editor;

    editor.Paste();
    TypeText(editor, "example");
    editor.Left();
    editor.Left();
    editor.Paste();
    editor.Right();
    editor.Paste();
    editor.Copy(0);
    editor.Paste();
    editor.Left();
    editor.Cut(0);
    editor.Paste();

    ASSERT_EQUAL(editor.GetText(), "example");
}

void SimpleTest() {
    string text = "abcdef";
    Editor editor;

    for (char c : text) {
        editor.Insert(c);
        cout << editor.GetText() << endl;
    }
    editor.Left();
    editor.Left();
    editor.Left();
    editor.Left();
    editor.Left();
    editor.Left();
    editor.Left();
    editor.Cut(3);
    cout << editor.GetText() << endl;
    editor.Cut(2);
    cout << editor.GetText() << endl;

    cin >> text;
}

int main() {
    //SimpleTest();

    TestRunner tr;
    RUN_TEST(tr, TestEditing);
    RUN_TEST(tr, TestReverse);
    RUN_TEST(tr, TestNoText);
    RUN_TEST(tr, TestEmptyBuffer);
    return 0;
}
