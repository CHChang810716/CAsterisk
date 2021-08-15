#pragma once
#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
namespace catk::analysis::alloc_sym_dep {
using CapturedSymbols = std::unordered_map<std::string, symdb::Symbol*>;

void root(const syntax::AST& ast);
void module(const syntax::AST& ast, symdb::Symbol* parent);
void context(
    const syntax::AST& ast, 
    symdb::Symbol* parent,
    const CapturedSymbols& captured_symbol
);
void assign_stmt(const syntax::AST& ast, symdb::Symbol* parent);

}