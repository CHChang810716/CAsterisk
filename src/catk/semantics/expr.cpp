#include <cstdint>
#include <catk/syntax/ast.hpp>
#include <catk/syntax/utils.hpp>
#include <catk/utils.hpp>
#include <catk/semantics/module.hpp>
#include <catk/semantics/context.hpp>
#include <catk/semantics/utils.hpp>
#include <catk/semantics/memory.hpp>
#include <catk/semantics/functional_expr.hpp>
#include <unordered_map>
#include <string_view>
#include <variant>

namespace catk::semantics {

Context* from_lambda_expr(catk::syntax::AST& ast) {
  std::vector<catk::syntax::AST*> params = catk::syntax::LambdaLiteral::params(ast);
  Context* ctx = Context::from_ast(catk::syntax::LambdaLiteral::body(ast), params);
  return ctx;
}

Expr* Expr::from_ast(catk::syntax::AST& ast) {
  if (ast.is_root()) {
    return Module::from_ast(ast);
  } else if (ast.is<catk::syntax::RetContext>()) {
    return Context::from_ast(ast, {});
  } else if (ast.is<catk::syntax::AssignStmt>()) {
    return Symbol::from_ast(ast);
  } else if (is_functional_expr(ast)) {
    return FunctionalExpr::from_ast(ast);
  } else if (ast.is<catk::syntax::Identifier>()) {
    return Symbol::from_ast(ast); 
  } else if (ast.is<catk::syntax::LambdaLiteral>()) {
    return from_lambda_expr(ast);
  } else if (ast.is<catk::syntax::IntLiteral>()) {
    return Constant::from_ast(ast);
  } else if (ast.is<catk::syntax::FPLiteral>()) {
    return Constant::from_ast(ast);
  } else if (ast.is<catk::syntax::StringLiteral>()) {
    rt_assert(false, "NYI: string");
  } else if (ast.is<catk::syntax::ArrayLiteral>()) {
    rt_assert(false, "NYI: array");
  } else {
    rt_assert(false, fmt::format("This AST has no semantics: <{}> {}", ast.name(), ast.content()));
  }
}

template<class FExpr>
struct FunctionalExprMaker {
  static FunctionalExpr* eval(catk::syntax::AST& ast) {
    FunctionalExpr& fexpr = DB::get().alloc<FunctionalExpr>();
    std::vector<Expr*> opnds;
    auto ast_opnds = FExpr::opnds(ast);
    Function func = Function::from_expr<FExpr>(FExpr::function(ast));
    if (ast.is<catk::syntax::FCallExpr>() && catk::syntax::FCallExpr::is_param_bind_fcall(ast)) {
      auto passed_param_names = catk::syntax::FCallExpr::opnd_labels(ast);
      Symbol*& ufunc = std::get<Symbol*>(func.get_variant());
      Context* uf_ctx = dynamic_cast<Context*>(ufunc->rhs());
      auto& params = uf_ctx->get_params();
      rt_assert(params.size() == passed_param_names.size(), "not yet support default parameter"
        " so the passed and defined parameter should be same");
      std::unordered_map<std::string_view, int> index;
      for (unsigned i = 0; i < params.size(); ++i) {
        auto& p = params[i];
        index[p->get_name()] = i;
      } 
      opnds.resize(ast_opnds.size());
      for (unsigned i = 0; i < passed_param_names.size(); ++i) {
        auto& ppn = passed_param_names[i];
        int defined_param_pos = index.at(ppn);
        opnds[defined_param_pos] = Expr::from_ast(*ast_opnds[i]);
      }
    } else {
      for (auto&& ast_opnd : ast_opnds) {
        opnds.push_back(Expr::from_ast(*ast_opnd));
      }
    }
    fexpr.set_function(std::move(func));
    fexpr.set_opnds(std::move(opnds));
    return &fexpr;
  }
};

FunctionalExpr* FunctionalExpr::from_ast(catk::syntax::AST& ast) {
  rt_assert(is_functional_expr(ast), "semantic parse error, not functional expr ast: " + ast.content());
  return catk::syntax::visit<FunctionalExprASTs, FunctionalExprMaker>(ast);
}

std::vector<Expr*> FunctionalExpr::dependencies() const {
  std::vector<Expr*> res;
  res.resize(opnds_.size());
  for(size_t i = 0; i < opnds_.size(); ++i) {
    assert(opnds_[i]);
    res[i] = opnds_[i];
  }
  Symbol* const* uf_sym = std::get_if<Symbol*>(&func_.get_variant());
  if (uf_sym) {
    assert(*uf_sym);
    res.push_back(*uf_sym);
  }
  return res;
}

Expr* Symbol::rhs() const {
  return rhs_;
}

std::string Symbol::get_name_from_assign_stmt(catk::syntax::AST& ast) {
  rt_assert(ast.is<catk::syntax::AssignStmt>(), "must assign statement: " + ast.content());
  auto& lhs_syms_ast = catk::syntax::AssignStmt::left_list(ast);
  auto& lhs_syms = lhs_syms_ast.children;
  rt_assert(lhs_syms.size() == 1, "NYI: multiple LHS: " + lhs_syms_ast.content());
  auto& lhs_sym = *lhs_syms.at(0);
  std::string sym_name = lhs_sym.string();
  return sym_name;
}

Symbol* Symbol::from_ast(catk::syntax::AST& ast) {
  // search symbol from current context
  if (ast.is<catk::syntax::Identifier>()) {
    auto* ctx = get_current_context();
    if (ctx) {
      auto *res = ctx->get_symbol(ast.string());
      if (res) return res;
    }
    rt_assert(false, "identifier not defined: " + ast.string());
  }
  Symbol& sym = DB::get().alloc<Symbol>();
  if (ast.is<catk::syntax::Param>()) {
    sym.name_ = catk::syntax::Param::id(ast).string();
  } else {
    sym.name_ = get_name_from_assign_stmt(ast);
    auto& rhs_ast = catk::syntax::AssignStmt::right_expr(ast);
    sym.rhs_ = Expr::from_ast(rhs_ast);
  }
  return &sym;
}

void Symbol::dump(catk::io::FmtStream& out) const {
  out << name_; // << " = ";
  // if (rhs_) {
  //   out << " = ";
  //   rhs_->dump(out);
  // }
  out.flush();
}

std::vector<Expr*> Symbol::dependencies() const {
  if (!rhs_) return {};
  return {rhs_};
}

RetExpr* RetExpr::from_ast(catk::syntax::AST& ast) {
  rt_assert(ast.is<catk::syntax::RetStmt>(), "must return statement: " + ast.content());
  RetExpr& ret = DB::get().alloc<RetExpr>();
  catk::syntax::AST& r_expr = catk::syntax::RetStmt::expr(ast);
  ret.rhs_ = Expr::from_ast(r_expr);
  return &ret;
}

void RetExpr::dump(catk::io::FmtStream& out) const {
  out << "ret ";
  rhs_->dump(out);
};

std::vector<Expr*> RetExpr::dependencies() const {
  if (!rhs_) return {};
  return {rhs_};
}

using ConstantASTs = avalon::mpl::TypeList<
  catk::syntax::IntLiteral,
  catk::syntax::FPLiteral
  // catk::syntax::StringLiteral
>;

template<class SyntaxExpr>
struct ConstantMaker {
  static Constant* eval(catk::syntax::AST& ast) {
    Constant& cv = DB::get().alloc<Constant>();
    SyntaxExpr::interpret(ast, cv.get_storage());
    return &cv;
  }
};

Constant* Constant::from_ast(catk::syntax::AST& ast) {
  rt_assert(catk::syntax::is_in<ConstantASTs>(ast), "must constant value: " + ast.content());
  return catk::syntax::visit<ConstantASTs, ConstantMaker>(ast);
}

std::vector<Expr*> Constant::dependencies() const {
  return {};
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

void FunctionalExpr::dump(catk::io::FmtStream& out) const {
  std::visit(overloaded {
    [&](Symbol* uf) {
      out << uf->get_name() << '(';
      bool is_first = true;
      for (auto& opnd : opnds_) {
        if (!is_first) {
          out << ", ";
        } else {
          is_first = false;
        }
        opnd->dump(out);
      }
      out << ")";
      out.flush();
    },
    [&](BinOp bop) {
      rt_assert(opnds_.size() == 2, "binary operator has 2 operands");
      opnds_[0]->dump(out);
      out << ' ' << std::to_string(bop) << ' ';
      opnds_[1]->dump(out);
    }, 
    [&](UnaryOp uop) {
      rt_assert(opnds_.size() == 1, "unary operator has 1 operands");
      out << std::to_string(uop);
      opnds_[0]->dump(out);
    },
    [&](IfElseOp ifelse) {
      rt_assert(opnds_.size() == 3, "if else has 3 operands");
      out << "if(";
      opnds_[0]->dump(out);
      out << ") ";
      opnds_[1]->dump(out);
      out << " else ";
      opnds_[2]->dump(out);
    }
  }, func_.get_variant());
}

}