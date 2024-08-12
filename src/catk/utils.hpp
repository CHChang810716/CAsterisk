#pragma once
#include <tao/pegtl/contrib/parse_tree.hpp>
#include "syntax/ast.hpp"
#include <llvm/IR/IRBuilder.h>
namespace catk {

using ASTPtr = syntax::AST*;
struct InternalBug : std::runtime_error {
   using std::runtime_error::runtime_error;
};
namespace detail {
void rt_assert_internal(bool b, const std::string& msg, const std::string& file, int line);
}

#define rt_assert(f, msg) catk::detail::rt_assert_internal(f, msg, __FILE__, __LINE__)

template<class OS>
void escape( OS& os, const char* p, const std::size_t s ) {
   static const char* h = "0123456789abcdef";

   const char* l = p;
   const char* const e = p + s;
   while( p != e ) {
      const unsigned char c = *p;
      if( c == '\\' ) {
         os.write( l, p - l );
         l = ++p;
         os << "\\\\";
      }
      else if( c == '"' ) {
         os.write( l, p - l );
         l = ++p;
         os << "\\\"";
      }
      else if( c < 32 ) {
         os.write( l, p - l );
         l = ++p;
         switch( c ) {
            case '\b':
               os << "\\b";
               break;
            case '\f':
               os << "\\f";
               break;
            case '\n':
               os << "\\n";
               break;
            case '\r':
               os << "\\r";
               break;
            case '\t':
               os << "\\t";
               break;
            default:
               os << "\\u00" << h[ ( c & 0xf0 ) >> 4 ] << h[ c & 0x0f ];
         }
      }
      else if( c == 127 ) {
         os.write( l, p - l );
         l = ++p;
         os << "\\u007f";
      }
      else {
         ++p;
      }
   }
   os.write( l, p - l );
}

template<class OS>
void escape( OS& os, const std::string& s ) {
   escape( os, s.data(), s.size() );
}
template<class Node>
void print_dot_node( std::ostream& os, const Node& n, const std::string& s )
{
   os << "  x" << &n << " [ label=\"";
   escape( os, s );
   if( n.has_content() ) {
      os << "\\n";
      escape( os, n.m_begin.data, n.m_end.data - n.m_begin.data );
   }
   os << "\" ]\n";
   if( !n.children.empty() ) {
      os << "  x" << &n << " -> { ";
      for( auto& up : n.children ) {
         os << "x" << up.get() << ( ( up == n.children.back() ) ? " }\n" : ", " );
      }
      for( auto& up : n.children ) {
         print_dot_node( os, *up, up->name() );
      }
   }
}
template<class Node>
void print_dot( std::ostream& os, const Node& n )
{
   assert( n.is_root() );
   os << "digraph parse_tree\n{\n";
   print_dot_node( os, n, "ROOT" );
   os << "}\n";
}

inline static auto& get_llvm_context() {
   static thread_local llvm::LLVMContext context;
   return (context);
}

// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template <class T>
constexpr
std::string_view
type_name()
{
    using namespace std;
#ifdef __clang__
    string_view p = __PRETTY_FUNCTION__;
    return string_view(p.data() + 34, p.size() - 34 - 1);
#elif defined(__GNUC__)
    string_view p = __PRETTY_FUNCTION__;
#  if __cplusplus < 201402
    return string_view(p.data() + 36, p.size() - 36 - 1);
#  else
    return string_view(p.data() + 49, p.find(';', 49) - 49);
#  endif
#elif defined(_MSC_VER)
    string_view p = __FUNCSIG__;
    return string_view(p.data() + 84, p.size() - 84 - 7);
#endif
}

}