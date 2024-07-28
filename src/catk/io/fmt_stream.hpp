#pragma once

#include <boost/iostreams/stream.hpp>
#include <vector>
#include <catk/utils.hpp>

namespace catk::io {

class FmtDevice {
  enum State {
    DirectPrint,
    NeedIndent
  };
public:
  struct category : boost::iostreams::sink_tag {};
  using char_type = char;
  explicit FmtDevice(std::ostream& out)
  : indent_(0)
  , indent_symbol_(" ")
  , line_delim_('\n')
  , state_(NeedIndent)
  , out_(out)
  {}

  void set_indent(int n) { indent_ = n; }
  void add_indent(int n) { indent_ += n; }
  void sub_indent(int n) { indent_ -= n; }
  void set_indent_symbol(const std::string& s) { indent_symbol_ = s; }
  void set_line_delim(char c) { line_delim_ = c; }
  std::streamsize write(const char* s, std::streamsize n);

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

namespace detail {

enum IndentOp{
  IDO_SetN, 
  IDO_AddN, 
  IDO_SubN,
  IDO_SetISym, 
  IDO_SetLDelim
};

template<class T>
struct Indent {
  Indent(IndentOp op, const T& i) 
  : op(op)
  , n(i)
  {}
  const IndentOp op;
  const T n;
};

}
inline static auto set_indent(int n) { return detail::Indent<int>(detail::IDO_SetN, n); }
inline static auto add_indent(int n) { return detail::Indent<int>(detail::IDO_AddN, n); }
inline static auto sub_indent(int n) { return detail::Indent<int>(detail::IDO_SubN, n); }
inline static auto set_indent_symbol(const std::string& s) { return detail::Indent<std::string>(detail::IDO_SetISym, s); }
inline static auto set_line_delim(char c) { return detail::Indent<char>(detail::IDO_SetLDelim, c); }

template<class T>
inline static std::ostream& operator<<(std::ostream& gout, detail::Indent<T> indent) {
  auto& out = *dynamic_cast<FmtStream*>(&gout);
  out.flush();
  if constexpr(std::is_same_v<T, std::string>) {
    rt_assert(indent.op == detail::IDO_SetISym, "Unknown indent op");
    out->set_indent_symbol(indent.n);
  } else if constexpr(std::is_same_v<T, char>) {
    rt_assert(indent.op == detail::IDO_SetLDelim, "Unknown indent op");
    out->set_line_delim(indent.n);
  } else {
    switch (indent.op) {
      case detail::IDO_SetN:
        out->set_indent(indent.n);
        break;
      case detail::IDO_AddN:
        out->add_indent(indent.n);
        break;
      case detail::IDO_SubN:
        out->sub_indent(indent.n);
        break;
      default:
        rt_assert(false, "Unknown indent op");
    }
  }
  return gout;
}

extern FmtStream fmout;

}