#pragma once
namespace catk {

namespace semantics {
class Module; 
} // namespace semantics

namespace behavior {
class Unit; 
} // namespace behavior 

catk::behavior::Unit* make_unit(const catk::semantics::Module* ctx);

} // namespace catk
