#include <catk/semantics/functional_expr.hpp>
#include <catk/semantics/symbol.hpp>
#include <catk/semantics/ast_visitor.hpp>
#include <catk/semantics/memory.hpp>
#include <catk/semantics/context.hpp>
#include <catk/semantics/symbol.hpp>

namespace catk::semantics {

template<class FExpr>
struct FunctionalExprMaker : public detail::AstVisitor<FExpr>{
  FunctionalExpr* operator()(catk::syntax::AST& ast) const {
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
  FunctionalExpr* operator()() const {
    rt_assert(false, "should not be here.");
    return nullptr;
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

Expr* FunctionalExpr::clone() const {
  return clone_impl<FunctionalExpr>();
}

Expr* FunctionalExpr::deep_clone(SymbolTable& st) const {
  FunctionalExpr* fe = &DB::get().alloc<FunctionalExpr>();
  if (is_user_function_call()) {
    auto* uf_sym = get_user_function();
    fe->set_function(Function(st.at(uf_sym->get_name())));
  } else {
    fe->func_ = func_;
  }
  for (auto& opnd : opnds_) {
    Expr* cloned_opnd = nullptr;
    if (opnd->has_name()) {
      // get from symbol table
      cloned_opnd = st.at(opnd->get_name());
    } else {
      // clone opnd
      cloned_opnd = opnd->deep_clone(st);
    }
    fe->opnds_.push_back(cloned_opnd);
  }
  return fe;
}

bool FunctionalExpr::is_user_function_call() const {
  return func_.is_user_function_call();
}

Symbol* FunctionalExpr::get_user_function() const {
  return std::get<Symbol*>(func_.get_variant());
}

}