#pragma once
#include <catk/semantics/expr.hpp>
#include <catk/type/type.hpp>
namespace catk {

using Type = catk::type::Type;

Type* get_type(const catk::semantics::Expr* expr);
const catk::semantics::Context* get_typed_context(const catk::semantics::Context* ctx, const std::vector<Type*>& opnd_tys);

} // namespace catk
