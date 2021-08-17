#include "symdb.hpp"
namespace catk {
SymDB<>& get_sym_db() {
  static SymDB<> symdb;
  return symdb;
}
TypeDB<>& get_type_db() {
  static auto typedb = [](){
    TypeDB<> res;
    if(res.size() != 0) {
      throw std::runtime_error(
        "BUG: typedb should be empty at this point."
      );
    }
    for(const auto& ept : symdb::PrimaryTypes()) {
      symdb::Type pt;
      pt.bits   = symdb::primary_type_bits(ept);
      // pt.id     = ept;
      pt.set_primary(true);
      pt.set_pointer(false);
      pt.set_mutable(false);
      pt.set_aggregate(false);
      pt.set_complete(true);
      pt.set_name(symdb::primary_type_names[ept]);
      res.push_back(pt);
    }
    return res;
  }();
  return typedb;
}

FuncDB& get_func_db() {
  static FuncDB db = [](){
    FuncDB fdb;
    // TODO: initial fdb
    return fdb;
  }();
  return db; 
}

}