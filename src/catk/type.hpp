#pragma once
#include <catk/semantics/expr.hpp>
#include <catk/type/type.hpp>
namespace catk {

using Type = catk::type::Type;

Type* getType(const catk::semantics::Expr* expr);
const catk::semantics::Context* getTypedContext(const catk::semantics::Context* ctx, const std::vector<Type*>& opnd_tys);

} // namespace catk
