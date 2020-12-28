#include <entity.hpp>
#include <map>
#include <iostream>

using namespace patchbot;

/// CLASS ROBOT
robot::robot( robot_type type )
	:robot_type_{ type }
{}

/// CLASS TILE
tile::tile( tile_type type )
	: tile_type_{ type }
{}

/// GETTER
tile_type tile::type() const
{
	return tile_type_;
}