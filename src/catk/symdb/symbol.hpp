#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include "type.hpp"
namespace catk::symdb {

struct Symbol {

  bool is_locatable()   const { return is_what(is_locatable_);  }
  bool is_identifier()  const { return is_what(is_identifier_); }
  bool is_function()    const { return is_what(is_function_);   }
  bool is_context()     const { return is_what(is_context_);    }
  bool is_literal()     const { return is_what(is_literal_);    }
  bool is_string()      const { return is_what(is_string_);     }
  bool is_solid()       const { return is_what(is_solid_);      }
  bool is_module()      const { return is_what(is_module_);     }

  void set_locatable(bool b)   { if(b) is_locatable_  = 1; else is_locatable_  = 0;  }
  void set_identifier(bool b)  { if(b) is_identifier_ = 1; else is_identifier_ = 0;  }
  void set_function(bool b)    { if(b) is_function_   = 1; else is_function_   = 0;  }
  void set_context(bool b)     { if(b) is_context_    = 1; else is_context_    = 0;  }
  void set_literal(bool b)     { if(b) is_literal_    = 1; else is_literal_    = 0;  }
  void set_string(bool b)      { if(b) is_string_     = 1; else is_string_     = 0;  }
  void set_solid(bool b)       { if(b) is_solid_      = 1; else is_solid_      = 0;  }
  void set_module(bool b)      { if(b) is_module_     = 1; else is_module_     = 0;  }
  
  std::string           name       ;
  std::vector<Symbol*>  children   ;
  std::vector<Symbol*>  related    ;
  Symbol*               parent     ;
  std::map<
    std::string,
    Symbol*
  >                     accessable ;
  Type*                 type       ;
private:
  bool is_what(const std::int64_t& what) const {
    return what == 1;
  }
  std::int64_t 
    is_locatable_   : 1,
    is_identifier_  : 1,
    is_function_    : 1,
    is_context_     : 1,
    
    is_literal_     : 1,
    is_string_      : 1,
    is_solid_       : 1, // executable
    is_module_      : 1
  ;
};

}