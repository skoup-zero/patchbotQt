#include <state_machine.hpp>

using namespace patchbot;

state_machine::state_machine( terrain &terrain, std::shared_ptr<robot> &self )
	:terrain_{ terrain }
	,self_{ self }
{}

void state_machine::process()
{}

bool state_machine::is_alive() const
{
	return !(terrain_.at(self_->x_, self_->y_).occupant_ == nullptr);
}