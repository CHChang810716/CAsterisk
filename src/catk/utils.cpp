#include <catk/utils.hpp>
#include <fmt/format.h>
namespace catk::detail {

void rt_assert_internal(bool b, const std::string& msg, const std::string& file, int line) {
  if(!b) {
    std::string full_msg = fmt::format("Fail: {}\nLocation: {}:{}", msg, file, line);
    throw InternalBug(full_msg);
  }
}
  
} // namespace catk::detail
