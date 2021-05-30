#pragma once
#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
namespace catk::analysis {

void root_resolve(const syntax::AST& ast);
void module_resolve(const syntax::AST& ast, symdb::Symbol* parent);
void context_resolve(const syntax::AST& ast, symdb::Symbol* parent);
void assign_stmt_resolve(const syntax::AST& ast, symdb::Symbol* parent);

}