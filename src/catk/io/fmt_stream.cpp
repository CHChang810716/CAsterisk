#include <iostream>
#include <catk/io/fmt_stream.hpp>

namespace catk::io {

std::streamsize FmtDevice::write(const char* s, std::streamsize n) {
  // std::streamsize written = 0;
  if (state_ == NeedIndent) {
    // written += write_indent();
    write_indent();
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
      // written += write_indent();
      write_indent();
      need_indent = false;
    }
    std::streamsize line_size = ptr - last_beg + 1;
    out_.write(last_beg, line_size);
    last_beg = last_beg + line_size;
    need_indent = true;
    // written += line_size;
  }
  const char* end = s + n;
  if (last_beg != end) {
    if (need_indent) {
      // written += write_indent();
      write_indent();
      need_indent = false;
    }
    auto line_size = end - last_beg;
    out_.write(last_beg, line_size);
    // written += line_size;
  }
  state_ = need_indent ? NeedIndent : DirectPrint;
  // return written;
  return n;
}

FmtStream fmout(std::cout);

} // namespace catk::io
