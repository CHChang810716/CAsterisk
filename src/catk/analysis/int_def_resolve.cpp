#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
#include "forward_decl.hpp"
#include <catk/utils.hpp>
#include <boost/lexical_cast.hpp>
namespace catk::analysis {

static auto int_type_resolve(const syntax::AST& right) {
  using Func   = std::function<void(symdb::PrimaryUnion&, const syntax::AST&)>;
  using IntMap = std::unordered_map<
    std::string, std::tuple<
      symdb::Type*, Func
    >
  >;
  auto& type_db = catk::get_type_db();
  static IntMap tag_to_type = [&type_db](){
    IntMap res;
    Func func;
    func = [](auto& pu, const auto& right){ 
      auto& literal = syntax::IntLiteral::literal(right);
      pu = boost::lexical_cast<std::uint8_t>(literal.content()); 
    };
    res["ui8" ] = std::make_tuple(&type_db[symdb::CATK_UINT8] , std::move(func));
    func = [](auto& pu, const auto& right){ 
      auto& literal = syntax::IntLiteral::literal(right);
      pu = boost::lexical_cast<std::uint16_t>(literal.content()); 
    };
    res["ui16"] = std::make_tuple(&type_db[symdb::CATK_UINT16], std::move(func));
    func = [](auto& pu, const auto& right){ 
      auto& literal = syntax::IntLiteral::literal(right);
      pu = boost::lexical_cast<std::uint32_t>(literal.content()); 
    };
    res["ui32"] = std::make_tuple(&type_db[symdb::CATK_UINT32], std::move(func));
    func = [](auto& pu, const auto& right){ 
      auto& literal = syntax::IntLiteral::literal(right);
      pu = boost::lexical_cast<std::uint64_t>(literal.content()); 
    };
    res["ui64"] = std::make_tuple(&type_db[symdb::CATK_UINT64], std::move(func));
    func = [](auto& pu, const auto& right){ 
      auto& literal = syntax::IntLiteral::literal(right);
      pu = boost::lexical_cast<std::int8_t>(literal.content()); 
    };
    res["i8"  ] = std::make_tuple(&type_db[symdb::CATK_INT8]  , std::move(func));
    func = [](auto& pu, const auto& right){ 
      auto& literal = syntax::IntLiteral::literal(right);
      pu = boost::lexical_cast<std::int16_t>(literal.content()); 
    };
    res["i16" ] = std::make_tuple(&type_db[symdb::CATK_INT16] , std::move(func));
    func = [](auto& pu, const auto& right){ 
      auto& literal = syntax::IntLiteral::literal(right);
      pu = boost::lexical_cast<std::int32_t>(literal.content()); 
    };
    res["i32" ] = std::make_tuple(&type_db[symdb::CATK_INT32] , std::move(func));
    func = [](auto& pu, const auto& right){ 
      auto& literal = syntax::IntLiteral::literal(right);
      pu = boost::lexical_cast<std::int64_t>(literal.content()); 
    };
    res["i64" ] = std::make_tuple(&type_db[symdb::CATK_INT64] , std::move(func));
    return res;
  }();

  catk::rt_assert(right.is<syntax::IntLiteral>(), "BUG: literal should be integer.");
  auto& tag = syntax::IntLiteral::tag(right);
  return tag_to_type.at(tag.content());
}

void int_def_resolve(const syntax::AST& left, const syntax::AST& right, symdb::Symbol* parent) {
  auto& sym_db = catk::get_sym_db();
  auto& type_db = catk::get_type_db();
  auto& id = *sym_db.alloc(); 
  auto [ptype, caster] = int_type_resolve(right);
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