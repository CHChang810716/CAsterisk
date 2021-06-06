#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include "type.hpp"
#include <catk/syntax/ast.hpp>
#include "forward_decl.hpp"

namespace catk::symdb {

struct Symbol {
  Symbol();
  bool is_locatable()   const; 
  bool is_identifier()  const; 
  bool is_function()    const; 
  bool is_context()     const; 
  bool is_literal()     const; 
  bool is_string()      const; 
  bool is_solid()       const; 
  bool is_module()      const; 
  bool is_expr()        const; 

  void set_locatable(bool b)  ; 
  void set_identifier(bool b) ; 
  void set_function(bool b)   ; 
  void set_context(bool b)    ; 
  void set_literal(bool b)    ; 
  void set_string(bool b)     ; 
  void set_solid(bool b)      ; 
  void set_module(bool b)     ; 
  void set_expr(bool b)       ; 

private:
  std::uint64_t 
    is_locatable_   : 1,
    is_identifier_  : 1,
    is_function_    : 1,
    is_context_     : 1,
    
    is_literal_     : 1,
    is_string_      : 1,
    is_solid_       : 1, // executable
    is_module_      : 1,

    is_expr_        : 1
  ;


public:
  SymDB<>*                    db                  ;
  std::vector<Symbol*>        children            ;
  std::vector<Symbol*>        related             ;
  Symbol*                     parent              ;
  std::unordered_map<
    std::string,
    Symbol*
  >                           accessable          ;
  Type*                       type                ;
  const catk::syntax::AST*    ast                 ;
  std::string                 name                ;
  PrimaryUnion                content             ;
};

}