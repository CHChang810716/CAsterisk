#include "symbol.hpp"
#include <catk/utils.hpp>
namespace catk::symdb {

Symbol::Symbol()
: is_locatable_   (0)
, is_identifier_  (0)
, is_function_    (0)
, is_context_     (0)
, is_literal_     (0)
, is_string_      (0)
, is_solid_       (0)
, is_module_      (0)
, is_expr_        (0)
, db              (nullptr)
, children        ()
, ref_by          ()
, ref_to          ()
, parent          (nullptr)
, accessable      ()
, type            (nullptr)
, ast             (nullptr)
, name            ("")
{}
template<class INT>
static bool is_what(const INT& i) {
  return i == 0x1;
}
bool Symbol::is_locatable()   const { return is_what(is_locatable_);  }
bool Symbol::is_identifier()  const { return is_what(is_identifier_); }
bool Symbol::is_function()    const { return is_what(is_function_);   }
bool Symbol::is_context()     const { return is_what(is_context_);    }
bool Symbol::is_literal()     const { return is_what(is_literal_);    }
bool Symbol::is_string()      const { return is_what(is_string_);     }
bool Symbol::is_solid()       const { return is_what(is_solid_);      }
bool Symbol::is_module()      const { return is_what(is_module_);     }
bool Symbol::is_expr()        const { return is_what(is_expr_);       }

void Symbol::set_locatable(bool b)   { if(b) is_locatable_  = 0x1; else is_locatable_  = 0x0;  }
void Symbol::set_identifier(bool b)  { if(b) is_identifier_ = 0x1; else is_identifier_ = 0x0;  }
void Symbol::set_function(bool b)    { if(b) is_function_   = 0x1; else is_function_   = 0x0;  }
void Symbol::set_context(bool b)     { if(b) is_context_    = 0x1; else is_context_    = 0x0;  }
void Symbol::set_literal(bool b)     { if(b) is_literal_    = 0x1; else is_literal_    = 0x0;  }
void Symbol::set_string(bool b)      { if(b) is_string_     = 0x1; else is_string_     = 0x0;  }
void Symbol::set_solid(bool b)       { if(b) is_solid_      = 0x1; else is_solid_      = 0x0;  }
void Symbol::set_module(bool b)      { if(b) is_module_     = 0x1; else is_module_     = 0x0;  }
void Symbol::set_expr(bool b)        { if(b) is_expr_       = 0x1; else is_expr_       = 0x0;  }

void Symbol::add_parent(Symbol* p) {
  this->parent_ = p;
  if(p) {
    p->children_.push_back(this);
  } 
}

void Symbol::ref_to(Symbol* p) {
  if(p) {
    this->ref_to_.push_back(p);
    p->ref_by_.push_back(this);
  }
}

}