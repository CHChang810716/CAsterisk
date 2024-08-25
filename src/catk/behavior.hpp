#pragma once
namespace catk {

namespace semantics {
class Module; 
} // namespace semantics

namespace behavior {
class Unit; 
} // namespace behavior 

catk::behavior::Unit* get_unit_behavior(const catk::semantics::Module* ctx);

} // namespace catk
