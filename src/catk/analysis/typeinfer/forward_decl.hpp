#pragma once
#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
namespace catk::analysis::typeinfer {
using CapturedSymbols = std::unordered_map<std::string, symdb::Symbol*>;

void root(const syntax::AST& ast);
void module_(const syntax::AST& ast, symdb::Symbol& sym);
symdb::Type* context(const syntax::AST& ast, symdb::Symbol& sym);

}