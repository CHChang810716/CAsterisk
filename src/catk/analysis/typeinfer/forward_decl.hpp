#pragma once
#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
namespace catk::analysis::typeinfer {
using CapturedSymbols = std::unordered_map<std::string, symdb::Symbol*>;

void root(const syntax::AST& ast);
void module_(const syntax::AST& ast, symdb::Symbol& sym);
void context(const syntax::AST& ast, symdb::Symbol& sym);
void assign_stmt(const syntax::AST& ast);

}