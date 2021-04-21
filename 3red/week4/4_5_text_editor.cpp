#include <string>
#include <string_view>
#include <algorithm>
#include <list>
#include "test_runner.h"
using namespace std;

class Editor {
 public:
  // Реализуйте конструктор по умолчанию и объявленные методы
  Editor()
    : pos_it_(cbegin(d_))
  {}

  void Left() {
    if (pos_it_ != cbegin(d_)) {
      --pos_it_;
    }
  }

  void Right() {
    if (pos_it_ != cend(d_)) {
      ++pos_it_;
    }
  }

  void Insert(char token) {
    PasteImpl(string_view(&token, 1));
  }

  void Cut(size_t tokens = 1) {
    tokens = min(tokens, static_cast<size_t>(distance(pos_it_, cend(d_))));
    auto end = next(pos_it_, tokens);
    buffer_.assign(pos_it_, end);
    pos_it_ = d_.erase(pos_it_, end);
  }

  void Copy(size_t tokens = 1) {
    tokens = min(tokens, static_cast<size_t>(distance(pos_it_, cend(d_))));
    buffer_.assign(pos_it_, next(pos_it_, tokens));
  }

  void Paste() {
    PasteImpl(buffer_);
  }

  string GetText() const {
    return {begin(d_), end(d_)};
  }

  void Debug() const {
    cout << "E = " << string{begin(d_), pos_it_} << "|" << string{pos_it_, end(d_)} << endl;
  }

private:
  using Data = list<string::value_type>;
  Data d_;
  Data::const_iterator pos_it_;
  string buffer_;

  void PasteImpl(string_view s) {
    pos_it_ = next(d_.insert(pos_it_, s.begin(), s.end()), s.size());
  }
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

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestEditing);
  RUN_TEST(tr, TestReverse);
  RUN_TEST(tr, TestNoText);
  RUN_TEST(tr, TestEmptyBuffer);
  return 0;
}