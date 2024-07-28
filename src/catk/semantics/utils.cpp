#include <catk/semantics/context.hpp>
namespace catk::semantics {

static Context* current_context_ = nullptr;
Context* get_current_context() { return current_context_; }
void Context::set_current_context(Context* ctx) {
  current_context_ = ctx;
}
}