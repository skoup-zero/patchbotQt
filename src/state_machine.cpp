#include <state_machine.hpp>

using namespace patchbot;

state_machine::state_machine( terrain &terrain, std::shared_ptr<robot> &self )
	:terrain_{ terrain }
	,self_{ self }
{}

void state_machine::action()
{
	std::cout << "THIS SHOULD NOT BE INVOKED" << std::endl;
}

bool state_machine::is_alive() const noexcept
{
	return self_->alive();
}