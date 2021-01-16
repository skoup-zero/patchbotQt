#include <entity.hpp>
#include <map>
#include <iostream>

using namespace patchbot;

/// CLASS ROBOT
robot::robot( robot_type type )
	:robot_type_{ type }
{
	id_ = ( type == robot_type::patchbot ) ? 0 : id_counter_++;
}

void robot::kill_robot()
{
	alive_ = false;
}
/// GETTER
bool robot::alive() const noexcept
{
	return alive_;
}

/// CLASS TILE
tile::tile( tile_type type )
	: tile_type_{ type }
{}

/// GETTER
tile_type tile::type() const noexcept
{
	return tile_type_;
}