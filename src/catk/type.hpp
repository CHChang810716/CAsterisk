#pragma once
#include <catk/semantics/expr.hpp>
#include <catk/type/type.hpp>
namespace catk {

using Type = catk::type::Type;

Type*& getType(catk::semantics::Expr* expr);

} // namespace catk
