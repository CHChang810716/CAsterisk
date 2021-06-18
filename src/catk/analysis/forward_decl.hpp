#pragma once
#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
namespace catk::analysis {

void root_resolve(const syntax::AST& ast);
void module_resolve(const syntax::AST& ast, symdb::Symbol* parent);
void context_resolve(const syntax::AST& ast, symdb::Symbol* parent);
void value_assign_stmt_resolve(const syntax::AST& ast, symdb::Symbol* parent);
void lambda_assign_stmt_resolve(const syntax::AST& ast, symdb::Symbol* parent);
void lambda_def_resolve(const syntax::AST& decl, const syntax::AST& expr, symdb::Symbol* parent);
void int_def_resolve(const syntax::AST& left, const syntax::AST& right, symdb::Symbol* parent);
void float_def_resolve(const syntax::AST& left, const syntax::AST& right, symdb::Symbol* parent);
void string_def_resolve(const syntax::AST& left, const syntax::AST& right, symdb::Symbol* parent);

symdb::Symbol* expr_resolve(const syntax::AST& ast, symdb::Symbol* parent);
}