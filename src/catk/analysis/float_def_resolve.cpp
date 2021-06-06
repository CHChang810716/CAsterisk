#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
#include "forward_decl.hpp"
#include <catk/utils.hpp>
#include <boost/lexical_cast.hpp>
namespace catk::analysis {

static auto float_type_resolve(const syntax::AST& right) {
  using Func   = std::function<void(symdb::PrimaryUnion&, const syntax::AST&)>;
  using FloatMap = std::unordered_map<
    std::string, std::tuple<symdb::Type*, Func>
  >;
  auto& type_db = catk::get_type_db();
  static FloatMap tag_to_type = [&type_db](){
    FloatMap res;
    Func func;

    func = [](auto& pu, const auto& right){ 
      auto& literal = syntax::FPLiteral::literal(right);
      pu = boost::lexical_cast<float>(literal.content()); 
    };
    res["f32" ] = std::make_tuple(&type_db[symdb::CATK_FLOAT32] , std::move(func));

    func = [](auto& pu, const auto& right){ 
      auto& literal = syntax::FPLiteral::literal(right);
      pu = boost::lexical_cast<double>(literal.content()); 
    };
    res["f64"] = std::make_tuple(&type_db[symdb::CATK_FLOAT64], std::move(func));

    return res;
  }();

  catk::rt_assert(right.is<syntax::IntLiteral>(), "BUG: literal should be integer.");
  auto& tag = syntax::FPLiteral::tag(right);
  return tag_to_type.at(tag.content());
}

void float_def_resolve(
  const syntax::AST& left, 
  const syntax::AST& right, 
  symdb::Symbol* parent
) {
  auto& sym_db = catk::get_sym_db();
  auto& type_db = catk::get_type_db();
  auto& id = *sym_db.alloc(); 
  auto [ptype, caster] = float_type_resolve(right);
  auto& type = *ptype;

  id.set_locatable(true);
  id.set_identifier(true);
  id.set_solid(true);
  id.name = left.content();
  id.type = &type;
  parent->accessable[id.name] = &id;

  auto& literal = *sym_db.alloc();
  literal.set_literal(true);
  literal.set_solid(true);
  literal.type = &type; 
  caster(literal.content, right);

  id.related.push_back(&literal);
  left.set_symbol(id);
  right.set_symbol(literal);
}

}