#pragma once
#include <catk/semantics/expr.hpp>
#include <catk/type/type.hpp>
namespace catk {

using Type = catk::type::Type;

Type*& getType(catk::semantics::Expr* expr);
catk::semantics::Context*& getTypedContext(const catk::semantics::Context* ctx, const std::vector<Type*>& opnd_tys);

} // namespace catk
