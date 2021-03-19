#include <state_machine.hpp>

using namespace patchbot;

state_machine::state_machine( terrain &terrain, std::shared_ptr<robot> &robot )
	:terrain_{ terrain }
	,robot_{ robot }
	,target_{direction::undefined} 
{}

direction state_machine::target() const noexcept
{
	return target_;
}

void state_machine::action()
{
	std::cout << "stateM" << std::endl;
}


bugger_ki::bugger_ki( terrain &terrain, std::shared_ptr<robot> &robot )
	: state_machine( terrain, robot )
{}

void bugger_ki::action()
{
	(this->*state_)();
}

void bugger_ki::find_wall()
{
	std::cout << "find wall" << std::endl;
}

void bugger_ki::wait()
{
	std::cout << "wait" << std::endl;
}

void bugger_ki::follow_wall()
{
	std::cout << "follow_wall" << std::endl;
}