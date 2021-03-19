#include <controls.hpp>
#include <path_finding.hpp>

using namespace patchbot;

controls::controls( patchbot::terrain &t )
	:terrain_{ t }
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
	if( frequency_.empty() || direction_.empty() )
		throw std::out_of_range( "ERROR: no Instructions" );

	frequency_.pop_back();
	direction_.pop_back();
}

void controls::update_world()
{
	update_patchbot();
	update_enemies();
	terrain_.update_doors();
}

void controls::update_patchbot()
{
	if( frequency_.empty() || direction_.empty() )
		throw std::out_of_range( "ERROR: no Instructions" );

	const unsigned int x = terrain_.patchbot_->x_, y = terrain_.patchbot_->y_;

	/* Patchbot doesn't move if the next tile is a wall */
	if( terrain_.wall_next_tile( x, y, direction_[0] ) )
	{
		update_instruction();
		return;
	}

	/* Patchbot waits at first contact with an obstacle */
	if( terrain_.obstacle( x, y, direction_[0] ))
		return;

	terrain_.move_robot( x, y, direction_[0] );

	if( terrain_.dangerous_tile( x, y ) )
	{
		terrain_.patchbot_->kill_robot();
		return;
	}

	update_instruction();
}

void controls::update_instruction()
{
	const bool patchbot_reached_wall =
		terrain_.wall_next_tile( terrain_.patchbot_->x_, terrain_.patchbot_->y_, direction_[0] );

	if( frequency_[0] == 0 && !until_wall_ )
		until_wall_ = true;

	if( frequency_[0] == 1 || frequency_[0] == 0 && patchbot_reached_wall )
	{
		until_wall_ = false;
		frequency_.erase( frequency_.begin() );
		direction_.erase( direction_.begin() );
	}
	else if( frequency_[0] > 0 )
		frequency_[0]--;
}

void controls::init_enemies()
{
	enemy_kis_.clear();
	
	for( auto t : terrain_.robots_ )
	{
		if( t->r_type_ == robot_type::bugger )
		{
			enemy_kis_.push_back( std::make_unique<bugger_ki>( terrain_, t ) );
		}
	}
} 

void controls::update_enemies()
{
	for ( auto it = enemy_kis_.begin(); it != enemy_kis_.end(); ++it)
	{
		(*it)->action();
	}
}


bool controls::check_win() const
{
	const unsigned int x = terrain_.patchbot_->x_;
	const unsigned int y = terrain_.patchbot_->y_;
	
	if( direction_.empty() )
		return false;

	/* if statement to ensure coordinates are in map bounds */
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
			return ( x >= terrain_.width() - 1 ) ?
				false : terrain_.at( x + 1, y ).type() == tile_type::server;

		case direction::wait: return false;

		default: throw std::invalid_argument( "ERROR: reading Robot direction" );
	}
}

void controls::load_dijkstra_path() const
{
	terrain_.set_dijkstra_path( dijkstra::calculate_paths( terrain_ ) );
}

/// GETTER

bool controls::until_wall() const noexcept
{
	return until_wall_;
}

bool controls::patchbot_dead() const noexcept
{
	return !terrain_.patchbot_->alive();
}

bool controls::patchbot_obstructed() const noexcept
{
	return terrain_.patchbot_->obstructed();
}

bool controls::instructions_empty() const noexcept
{
	return frequency_.empty();
}

terrain controls::terrain() const noexcept
{
	return terrain_;
}