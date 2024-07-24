#pragma once

#include <boost/iostreams/stream.hpp>
#include <vector>

namespace catk::io {

class FmtDevice {
  enum State {
    DirectPrint,
    NeedIndent
  };
public:
  using char_type = char;
  using category = boost::iostreams::sink_tag;
  explicit FmtDevice(std::ostream& out)
  : indent_(0)
  , indent_symbol_(" ")
  , line_delim_('\n')
  , state_(NeedIndent)
  , out_(out)
  {}
  void set_indent(int n) { indent_ = n; }
  void add_indent(int n) { indent_ += n; }
  void set_indent_symbol(const std::string& s) { indent_symbol_ = s; }
  void set_line_delim(char c) { line_delim_ = c; }

  std::streamsize write(const char* s, std::streamsize n) {
    std::streamsize written = 0;
    if (NeedIndent) {
      written += write_indent();
    }
    std::vector<const char*> line_delim_poss;
    for (const char* ts = s; ts < (s + n); ++ts) {
      if (*ts == line_delim_) {
        line_delim_poss.push_back(ts);
      }
    } 
    const char* last_beg = s;
    bool need_indent = false;
    for (const char* ptr : line_delim_poss) {
      if (need_indent) {
        written += write_indent();
        need_indent = false;
      }
      std::streamsize line_size = ptr - last_beg + 1;
      out_.write(last_beg, line_size);
      last_beg = last_beg + line_size;
      need_indent = true;
      written += line_size;
    }
    const char* end = s + n;
    if (last_beg != end) {
      if (need_indent) {
        written += write_indent();
        need_indent = false;
      }
      auto line_size = end - last_beg;
      out_.write(last_beg, line_size);
      written += line_size;
    }
    state_ = need_indent ? NeedIndent : DirectPrint;
    return written;
  }
private:
  std::streamsize write_indent() {
    for (int i = 0; i < indent_; ++i) {
      out_ << indent_symbol_;
    }
    return indent_ * indent_symbol_.size();
  };
  int indent_;
  std::string indent_symbol_;
  char line_delim_;
  State state_;
  std::ostream& out_;
};

using FmtStream = boost::iostreams::stream<FmtDevice>;

}