#include <avalon/mpl/type_list.hpp>
#include <catk/semantics/constant.hpp>
#include <catk/syntax/expr.hpp>
#include <catk/syntax/utils.hpp>
#include <catk/utils.hpp>
#include <catk/semantics/ast_visitor.hpp>
#include <catk/semantics/memory.hpp>

namespace catk::semantics {

using ConstantASTs = avalon::mpl::TypeList<
  catk::syntax::IntLiteral,
  catk::syntax::FPLiteral
  // catk::syntax::StringLiteral
>;

template<class SyntaxExpr>
struct ConstantMaker : public detail::AstVisitor<SyntaxExpr> {
  Constant* operator()(catk::syntax::AST& ast) const {
    Constant& cv = DB::get().alloc<Constant>();
    SyntaxExpr::interpret(ast, cv.get_storage());
    return &cv;
  }
  Constant* operator()() const {
    rt_assert(false, "should not be here");
    return nullptr;
  }
};

Constant* Constant::from_ast(catk::syntax::AST& ast) {
  rt_assert(catk::syntax::is_in<ConstantASTs>(ast), "must constant value: " + ast.content());
  return catk::syntax::visit<ConstantASTs, ConstantMaker>(ast);
}

std::vector<Expr*> Constant::dependencies() const {
  return {};
}

PrimaryType Constant::get_type() const {
  std::visit(overloaded {
    [&](std::uint8_t v)       { return CATK_UINT8;  },
    [&](std::uint16_t v)      { return CATK_UINT16; },
    [&](std::uint32_t v)      { return CATK_UINT32; },
    [&](std::uint64_t v)      { return CATK_UINT64; },
    [&](std::int8_t v)        { return CATK_INT8;   },
    [&](std::int16_t v)       { return CATK_INT16;  },
    [&](std::int32_t v)       { return CATK_INT32;  },
    [&](std::int64_t v)       { return CATK_INT64;  },
    [&](float v)              { return CATK_FLOAT32;},
    [&](double v)             { return CATK_FLOAT64;},
    [&](const std::string& s) { return CATK_STRING; },
  }, value_);
}

void Constant::dump(catk::io::FmtStream& out) const {
  std::visit(overloaded {
    [&](std::uint8_t v) { out << v << "u8"; },
    [&](std::uint16_t v) { out << v << "u16"; },
    [&](std::uint32_t v) { out << v << "u32"; },
    [&](std::uint64_t v) { out << v << "u64"; },
    [&](std::int8_t v) { out << v << "i8"; },
    [&](std::int16_t v) { out << v << "i16"; },
    [&](std::int32_t v) { out << v << "i32"; },
    [&](std::int64_t v) { out << v << "i64"; },
    [&](float v) { out << v << "f32"; },
    [&](double v) { out << v << "f64"; },
    [&](const std::string& s) { out << '"' << s << '"'; },
  }, value_);
  out.flush();
};

Expr* Constant::clone() const {
  return Expr::clone_impl<Constant>();
}

Expr* Constant::deep_clone(SymbolTable& st) const {
  return clone();
}

}