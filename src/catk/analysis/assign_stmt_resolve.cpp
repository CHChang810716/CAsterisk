#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
namespace catk::analysis {
void assign_stmt_resolve(const syntax::AST& ast, symdb::Symbol* parent) {
  auto& sym_db = catk::get_sym_db();
  auto& sym = *sym_db.alloc();
  sym.ast = &ast; 

}
}