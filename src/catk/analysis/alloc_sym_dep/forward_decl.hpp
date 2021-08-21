#pragma once
#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
namespace catk::analysis::alloc_sym_dep {
using CapturedSymbols = std::vector<std::string>;
using ParamSymbols = std::vector<std::string>;

void root(syntax::AST& ast);
void module_(syntax::AST& ast, symdb::Symbol* parent);
symdb::Symbol* context(
    syntax::AST& ast, 
    symdb::Symbol* parent,
    const CapturedSymbols& captured_symbols,
    const ParamSymbols& param_symbols
);
void assign_stmt(syntax::AST& ast, symdb::Symbol* parent);
symdb::Symbol* expr(syntax::AST& ast, symdb::Symbol* up_dep, symdb::Symbol* context_sym);

}