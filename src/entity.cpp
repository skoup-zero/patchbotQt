#include <entity.hpp>
#include <map>
#include <iostream>

using namespace patchbot;

/////////////////////////* CLASS ROBOT */////////////////////////
robot::robot( robot_type type )
	:r_type_{ type }
{
	id_ = ( type == robot_type::patchbot ) ? 0 : id_counter_++;
}

void robot::kill_robot()
{
	alive_ = false;
}

void robot::update_obstructed()
{
	obstructed_ = ( obstructed_ ) ? false : true;
}

/// GETTER
bool robot::alive() const noexcept
{
	return alive_;
}

bool robot::obstructed() const noexcept
{
	return obstructed_;
}


/////////////////////////* CLASS TILE */////////////////////////
tile::tile( tile_type type, const bool door )
	: t_type_{ type }
	, door_{ door }
{}

void tile::door_set_timer()
{
	if( !door_ )
		throw std::invalid_argument( " Tile is not a door " );

	if( !door_is_open() )
		timer_ = 10;
}

void patchbot::tile::door_decrement_timer()
{
	if( !door_ )
		throw std::invalid_argument( " Tile is not a door " );

	if( timer_ == 1 && occupant_ )
		return;

	if( timer_ > 0 )
		timer_--;
}

/// GETTER
tile_type tile::type() const noexcept
{
	return t_type_;
}

bool tile::door_is_open() const
{
	if( !door_ )
		throw std::invalid_argument( " Tile is not a door " );

	return ( timer_ <= 0 ) ? false : true;
}

bool tile::door_is_automatic() const
{
	if( !door_ )
		throw std::invalid_argument( " Tile is not a door " );

	return ( t_type_ == tile_type::automatic_door ) ? true : false;
}
