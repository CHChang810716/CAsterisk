#include <gtest/gtest.h>
#include <avalon/app/path.hpp>
#include <catk/io/fmt_stream.hpp>
#include <sstream>
namespace io = catk::io;

TEST(io_test, fmt_stream) {
  std::stringstream stream;
  io::FmtStream out(stream);
  out << io::add_indent(2);
  out << "xxx";
  out << "yyy";
  out << "mmm\n";
  out << "kkk";
  out.flush();
  std::cout << stream.str() << std::endl;;
  EXPECT_EQ(stream.str(), "  xxxyyymmm\n  kkk");
}