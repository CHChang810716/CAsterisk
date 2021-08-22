#pragma once
#include <cstdint>
#include <atomic>
namespace avalon {

#ifdef NDEBUG
template<class T>
struct DebugID {
  std::uint64_t debug_id() const { return 0; }
};
#else
template<class T>
struct DebugID {
  std::uint64_t debug_id() const { return debug_id_; }
private:
  static std::uint64_t next_debug_id() {
    static std::atomic<std::uint64_t> id = 0;
    return ++id;
  }
  std::uint64_t debug_id_ {next_debug_id()};
};
#endif
    
} // namespace avalon
