#include <controls.hpp>
#include <path_finding.hpp>


using namespace patchbot;

controls::controls( patchbot::terrain &t )
	:terrain_{ t }
{
	terrain_.load_dijkstra_path();
}

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
	terrain_.update_graves();
}

void controls::update_patchbot()
{
	if( frequency_.empty() || direction_.empty() )
		throw std::out_of_range( "ERROR: no Instructions" );

	/* Patchbot doesn't move if the next tile is a wall */
	if( terrain_.wall_next_tile( terrain_.patchbot_->x_, terrain_.patchbot_->y_, direction_[0] ) )
	{
		update_instruction();
		return;
	}

	/* Patchbot waits at first contact with an obstacle */
	if( terrain_.obstacle( terrain_.patchbot_->x_, terrain_.patchbot_->y_, direction_[0] ) )
		return;

	if( direction_[0] != direction::wait )
		terrain_.load_dijkstra_path();

	terrain_.move_robot( terrain_.patchbot_->x_, terrain_.patchbot_->y_, direction_[0] );

	if( terrain_.dangerous_tile( terrain_.patchbot_->x_, terrain_.patchbot_->y_ ) )
	{
		terrain_.kill_robot_at( terrain_.patchbot_->x_, terrain_.patchbot_->y_ );
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
	/* reset ai's after new start */
	enemy_ais_.clear();

	for( auto t : terrain_.robots_ )
	{
		if( t->r_type_ == robot_type::bugger )
			enemy_ais_.push_back( std::make_unique<bugger_ai>( terrain_, t ) );

		else if( t->r_type_ == robot_type::pusher || t->r_type_ == robot_type::digger ||
			t->r_type_ == robot_type::swimmer )
			enemy_ais_.push_back( std::make_unique<pusher_type_ai>( terrain_, t ) );

		else
			enemy_ais_.push_back( std::make_unique<follower_type>( terrain_, t ) );
	}
}

void controls::update_enemies()
{
	auto it = enemy_ais_.begin();
	while( it != enemy_ais_.end() )
	{
		if( ( *it )->is_alive() )
		{
			( *it )->action();
			++it;
		}
		else
			it = enemy_ais_.erase( it );
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

/// GETTER
bool controls::patchbot_dead() const
{
	return terrain_.at
	( terrain_.patchbot_->x_, terrain_.patchbot_->y_ ).occupant_ == nullptr;
}

bool controls::until_wall() const noexcept
{
	return until_wall_;
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