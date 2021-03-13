#include <entity.hpp>

using namespace patchbot;

/////////////////////////* CLASS ROBOT */////////////////////////
robot::robot( robot_type type )
	:r_type_{ type }
{}

void robot::update_obstructed()
{
	obstructed_ = ( obstructed_ ) ? false : true;
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

bool robot::obstructed() const noexcept
{
	return obstructed_;
}



/////////////////////////* CLASS TILE */////////////////////////
tile::tile( tile_type type, const bool door )
	: t_type_{ type }
	, door_{ door }
{}

void tile::door_decrement_timer()
{
	if( !door_ )
		throw std::invalid_argument( " Tile is not a door " );

	/* don't close the door if a robot is on it */
	if( timer_ == 1 && occupant_ )
		return;

	if( timer_ > 0 )
		timer_--;
}

void tile::door_set_timer()
{
	if( !door_ )
		throw std::invalid_argument( " Tile is not a door " );

	if( !door_is_open() )
		timer_ = 10;
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

	return  timer_ > 0;
}

bool tile::door_is_automatic() const
{
	if( !door_ )
		throw std::invalid_argument( " Tile is not a door " );

	return  t_type_ == tile_type::automatic_door;
}

int tile::node_cost() const
{ /* !NOTE: after enemies moved enemy start is accessible */
	if( t_type_ == tile_type::alien_weed || t_type_ == tile_type::steel_plates )
		return 1;

	if( t_type_ == tile_type::gravel )
		return 2;

	/* review this */
	if( door_ )
		if( !door_is_open() )
			return 2;

	return 0;
}