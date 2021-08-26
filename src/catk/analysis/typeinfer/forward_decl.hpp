#pragma once
#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
namespace catk::analysis::typeinfer {
using CapturedSymbols = std::unordered_map<std::string, symdb::Symbol*>;

void root();
void module_(symdb::Symbol& sym);
symdb::Type* context(symdb::Symbol& sym);

}