#pragma once
#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
namespace catk::analysis::alloc_sym_dep {
using CapturedSymbols = std::unordered_map<std::string, symdb::Symbol*>;

void root(syntax::AST& ast);
void module_(syntax::AST& ast, symdb::Symbol* parent);
void context(
    syntax::AST& ast, 
    symdb::Symbol* parent,
    const CapturedSymbols& captured_symbol
);
void assign_stmt(syntax::AST& ast, symdb::Symbol* parent);

}