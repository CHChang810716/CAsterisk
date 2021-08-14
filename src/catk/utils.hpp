#pragma once
#include <tao/pegtl/contrib/parse_tree.hpp>
#include "syntax/ast.hpp"
namespace catk {

using ASTPtr = syntax::AST*;
struct InternalBug : std::runtime_error {
   using std::runtime_error::runtime_error;
};
constexpr auto rt_assert = [](bool b, const std::string& string) {
  if(!b) {
    throw InternalBug(string);
  }
};
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

}