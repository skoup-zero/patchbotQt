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
		door_timer_ = 10;
}

void tile::door_decrement_timer()
{
	if( !door_ )
		throw std::invalid_argument( " Tile is not a door " );

	/* don't close the door if a robot is on it */
	if( door_timer_ == 1 && occupant_ )
		return;

	if( door_timer_ > 0 )
		door_timer_--;
}

void tile::grave_set_timer()
{
	grave_timer_ = 5;
	grave_ = true;
}

void tile::grave_decrement_timer()
{
	if( grave_timer_ <= 0 )
		grave_ = false;
	else
		grave_timer_--;
}

void tile::break_wall()
{
	if( t_type_ != tile_type::rock_wall )
		throw std::invalid_argument( " only rock wall is breakable " );

	t_type_ = tile_type::gravel;
}

int tile::node_cost() const
{
	if( occupant_ )
		return 0;

	if( t_type_ == tile_type::gravel || door_ && !door_is_open() )
		return 2;

	if( t_type_ == tile_type::patchbot_start || t_type_ == tile_type::enemy_start ||
		t_type_ == tile_type::steel_plates || t_type_ == tile_type::alien_weed || door_ )
		return 1;

	return -1;
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

	return  door_timer_ > 0;
}

bool tile::door_is_automatic() const
{
	if( !door_ )
		throw std::invalid_argument( " Tile is not a door " );

	return  t_type_ == tile_type::automatic_door;
}

bool tile::is_grave() const noexcept
{
	return grave_;
}
