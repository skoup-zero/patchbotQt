#include <controls.hpp>

using namespace patchbot;

controls::controls( terrain &terrain )
	:terrain_{ terrain }
{}

controls &controls::operator=( const controls &other )
{
	direction_ = other.direction_;
	frequency_ = other.frequency_;
	open_doors_ = other.open_doors_;
	until_wall_ = other.until_wall_;

	return *this;
}

void controls::add_instruction( const direction d, const int frequency )
{
	direction_.push_back( d );
	frequency_.push_back( frequency );
}

void controls::remove_instruction()
{
	frequency_.pop_back();
	direction_.pop_back();
}


void controls::update_world()
{
	update_patchbot();
	//update_enemies();
	update_doors();
}

void controls::update_patchbot()
{
	if( !frequency_.size() )
		throw std::out_of_range( "ERROR: no Instructions" );

	/* Patchbot doesn't move if the next tile is a wall */
	if( wall_next_tile( terrain_.patchbot_->x_, terrain_.patchbot_->y_ ) )
	{
		update_instruction();
		return;
	}
	/* Patchbot waits at first contact with an obstacle */
	if( obstacle( terrain_.patchbot_->x_, terrain_.patchbot_->y_ ) )
		return;

	move_robot( terrain_.patchbot_->x_, terrain_.patchbot_->y_ );

	if( dangerous_tile( terrain_.patchbot_->x_, terrain_.patchbot_->y_ ) )
	{
		terrain_.patchbot_->kill_robot();
		return;
	}

	update_instruction();
}

void controls::update_instruction()
{
	auto const patchbot_reached_wall =
		wall_next_tile( terrain_.patchbot_->x_, terrain_.patchbot_->y_ );

	if( frequency_[0] == 0 && !until_wall_ )
		until_wall_ = true;

	if( frequency_[0] == 1 || frequency_[0] == 0 && patchbot_reached_wall )
	{
		until_wall_ = false;
		frequency_.erase( frequency_.begin() );
		direction_.erase( direction_.begin() );
	}
	else if( frequency_[0] != 0 )
		frequency_[0]--;
}

void controls::move_robot( unsigned int const x, unsigned int const y ) const
{
	if( !terrain_.at( x, y ).occupant_ )
		throw std::invalid_argument( "ERROR: no robot at tile" );

	switch( direction_[0] )
	{
		case direction::up:
			terrain_.at( x, y - 1 ).occupant_.swap( terrain_.at( x, y ).occupant_ );
			terrain_.patchbot_->y_--; break;

		case direction::down:
			terrain_.at( x, y + 1 ).occupant_.swap( terrain_.at( x, y ).occupant_ );
			terrain_.patchbot_->y_++; break;

		case direction::left:
			terrain_.at( x - 1, y ).occupant_.swap( terrain_.at( x, y ).occupant_ );
			terrain_.patchbot_->x_--; break;

		case direction::right:
			terrain_.at( x + 1, y ).occupant_.swap( terrain_.at( x, y ).occupant_ );
			terrain_.patchbot_->x_++; break;

		case direction::wait: break;
		default: throw std::invalid_argument( "ERROR: reading Robot direction" );
	}
}

bool controls::dangerous_tile( unsigned const int  x, unsigned const int y ) const
{
	auto const &tile = terrain_.at( x, y );

	if( tile.type() == tile_type::precipice )
		return true;

	return tile.type() == tile_type::water && tile.occupant_->r_type_ != robot_type::swimmer;
}

bool controls::obstacle( unsigned const int x, unsigned const int y )
{
	if( !terrain_.at( x, y ).occupant_ )
		throw std::invalid_argument( "ERROR: no robot at tile" );

	auto &tile = terrain_.at( x, y );
	auto &robot = tile.occupant_;

	/* obstructed robots can move again */
	if( robot->obstructed() )
	{
		robot->update_obstructed();
		return false;
	}

	auto const has_wheels =
		robot->r_type_ == robot_type::patchbot ||
		robot->r_type_ == robot_type::pusher ||
		robot->r_type_ == robot_type::digger ||
		robot->r_type_ == robot_type::swimmer;

	/* obsacle for robots with wheels */
	if( tile.type() == tile_type::alien_weed && has_wheels )
	{
		robot->update_obstructed();
		return true;
	}

	/* obstacle for robots with legs */
	if( tile.type() == tile_type::gravel && !has_wheels )
	{
		robot->update_obstructed();
		return true;
	}

	return door_next_tile( x, y );
}

bool controls::wall( const unsigned int x, const unsigned int y, const robot_type r_type ) const
{
	/* map boundaries as walls */
	if( x >= terrain_.width() || y >= terrain_.height() )
		return true;

	auto const &tile = terrain_.at( x, y );

	/* environment as walls */

	/* actual walls */
	if( tile.type() == tile_type::concrete_wall )
		return true;
	/* breakable walls */
	if( tile.type() == tile_type::rock_wall && r_type != robot_type::digger )
		return true;
	/* server (wall) */
	if( tile.type() == tile_type::server )
		return true;
	/* secret door (wall) for KI */
	if( tile.type() == tile_type::secret_path && r_type != robot_type::patchbot )
		return true;
	/* automatic door (wall) for Patchbot */
	if( tile.type() == tile_type::automatic_door && r_type == robot_type::patchbot )
		return true;

	/* robots as walls */
	/* this will be moved into another class when KI is implemented */

	if( tile.occupant_ )
	{	/* patchbot and bugger interpret all other robots as walls */
		if( tile.occupant_ && ( r_type == robot_type::patchbot || r_type == robot_type::bugger ) )
			return true;
		/* follower, hunter, sniffer interpret other enemies as walls */
		if( tile.occupant_->r_type_ != robot_type::patchbot &&
			( r_type == robot_type::follower || r_type == robot_type::hunter
				|| r_type == robot_type::sniffer ) )
			return true;
	}

	return false;
}

bool controls::wall_next_tile( const unsigned int x, const unsigned int y ) const
{
	if( !terrain_.at( x, y ).occupant_ )
		throw std::invalid_argument( "ERROR: no robot at tile" );

	switch( direction_[0] )
	{
		case direction::up:
			return wall( x, y - 1, terrain_.at( x, y ).occupant_->r_type_ );

		case direction::down:
			return wall( x, y + 1, terrain_.at( x, y ).occupant_->r_type_ );

		case direction::left:
			return wall( x - 1, y, terrain_.at( x, y ).occupant_->r_type_ );

		case direction::right:
			return wall( x + 1, y, terrain_.at( x, y ).occupant_->r_type_ );

		case direction::wait: return false;

		default: throw std::invalid_argument( "ERROR: reading Robot direction" );
	}
}

bool controls::door_next_tile( unsigned const int x, unsigned const int y )
{
	if( !terrain_.at( x, y ).occupant_ )
		throw std::invalid_argument( "ERROR: no robot at tile" );

	tile *tile;

	switch( direction_[0] )
	{
		case direction::up:
			tile = &terrain_.at( x, y - 1 ); break;

		case direction::down:
			tile = &terrain_.at( x, y + 1 ); break;

		case direction::left:
			tile = &terrain_.at( x - 1, y ); break;

		case direction::right:
			tile = &terrain_.at( x + 1, y ); break;

		case direction::wait: return false;

		default: throw std::invalid_argument( "ERROR: reading Robot direction" );
	}
	/* review this */
	if( !tile->door_ )
		return false;

	if( tile->door_is_open() )
		return false;

	auto &robot = terrain_.at( x, y ).occupant_;

	if( !tile->door_is_automatic() && robot->r_type_ == robot_type::patchbot )
	{
		robot->update_obstructed();
		tile->door_set_timer();		/* open door */
		open_doors_.push_back( tile );	/* saving opened doors */
		return true;
	}

	if( tile->door_is_automatic() && robot->r_type_ != robot_type::patchbot )
	{
		robot->update_obstructed();
		tile->door_set_timer();		/* open door */
		open_doors_.push_back( tile );	/* saving opened doors */
		return true;
	}

	return false;
}

void controls::update_doors()
{
	for( auto i = 0; i < open_doors_.size(); i++ )
	{
		open_doors_[i]->door_decrement_timer();

		if( !open_doors_[i]->door_is_open() )
			open_doors_.erase( open_doors_.begin() + i );
	}
}

bool controls::check_win() const
{
	const auto x = terrain_.patchbot_->x_;
	const auto y = terrain_.patchbot_->y_;

	if( direction_.size() == 0 )
		return false;

	switch( direction_[0] )
	{
		case direction::up:
			return ( y <= 0 ) ?
				false : terrain_.at( x, y - 1 ).type() == tile_type::server;

		case direction::down:
			return ( y >= terrain_.height() - 1 ) ?
				false : terrain_.at( x, y + 1 ).type() == tile_type::server;

		case direction::left:
			return ( x <= 0 ) ? false :
				terrain_.at( x - 1, y ).type() == tile_type::server;

		case direction::right:
			return ( x >= terrain_.width() - 1) ?
				false : terrain_.at( x + 1, y ).type() == tile_type::server;

		case direction::wait: return false;

		default: throw std::invalid_argument( "ERROR: reading Robot direction" );
	}

}

/// GETTER

bool controls::until_wall() const noexcept
{
	return until_wall_;
}