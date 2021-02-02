#include <controls.hpp>

using namespace patchbot;

controls::controls( terrain *terrain )
	:terrain_{ terrain }
{}

controls::controls( QString &instructions, terrain *terrain )
	: terrain_{ terrain }
{
	std::string instructions_q = instructions.toLocal8Bit();
	for( int i = instructions.size() - 1; i >= 0; i -= 2 )
	{
		/* until wall equals 0 in stack */
		if( static_cast<unsigned char>( instructions_q.at( i ) ) == 'X' )
			frequency_.push( 0 );
		else
			frequency_.push( atoi( &instructions_q.at( i ) ) );

		direction_.push( direction_to_enum( instructions_q.at( i - 1 ) ) );
	}
}

direction controls::direction_to_enum( const char dir )
{
	switch( dir )
	{
		case 'O': return direction::up; break;
		case 'U': return direction::down; break;
		case 'L': return direction::left; break;
		case 'R': return direction::right; break;
		case 'W': return direction::wait; break;
		default: throw std::invalid_argument( "ERROR: reading Robot direction" );
	}
}

void controls::update_world()
{
	update_patchbot();
	//update enemies();
	//update doors();
}

void controls::update_patchbot()
{
	if( !frequency_.size() )
		throw std::out_of_range( "ERROR: empty stack" );

	auto &patchbot = terrain_->robots_[0];
	auto x = terrain_->robots_[0]->x_, y = terrain_->robots_[0]->y_;

	if( dangerous_tile( x, y ) )
	{
		patchbot->kill_robot();
		return;

	} else if( obstacle( x, y ) || wall_next_tile( x, y, direction_.top() ) )
	{
		update_instruction();
		return;

	} else
		move_robot( x, y, direction_.top() );

	update_instruction();
}

void controls::update_instruction()
{
	auto const patchbot_reached_wall =
		( wall_next_tile( terrain_->robots_[0]->x_, terrain_->robots_[0]->y_, direction_.top() ) ) ?
		true : false;

	if( frequency_.top() == 0 && !until_wall_ )
		until_wall_ = true;

	if( frequency_.top() == 1 || ( frequency_.top() == 0 && patchbot_reached_wall ) )
	{
		until_wall_ = false;
		frequency_.pop();
		direction_.pop();
	} else
		frequency_.top()--;
}

//void controls::one_step_patchbot()
//{
//	if( !frequency_.size() )
//		throw std::out_of_range( "ERROR: empty stack" );
//
//	if( obstacle( terrain_->robots_[0]->x_, terrain_->robots_[0]->y_ ) && !wait_ )
//	{
//		wait_ = true;
//		return;
//	} else
//		wait_ = false;
//
//	if( frequency_.top() == 0 )
//	{
//		until_wall_ = true;
//
//		if( wall_next_tile( terrain_->robots_[0]->x_, terrain_->robots_[0]->y_, direction_.top() )
//			&& until_wall_ )
//		{
//			until_wall_ = false;
//			frequency_.pop();
//			direction_.pop();
//
//			if( !direction_.empty() )
//				move_robot( terrain_->robots_[0]->x_, terrain_->robots_[0]->y_, 0, direction_.top() );
//		} else
//		{
//			move_robot( terrain_->robots_[0]->x_, terrain_->robots_[0]->y_, 0, direction_.top() );
//		}
//	} else
//	{
//		move_robot( terrain_->robots_[0]->x_, terrain_->robots_[0]->y_, 0, direction_.top() );
//
//		if( frequency_.top() <= 1 )
//		{
//			frequency_.pop();
//			direction_.pop();
//		}
//		if( !frequency_.empty() )
//			( frequency_.top() == 0 ) ?
//			until_wall_ = true : frequency_.top()--;
//	}
//
//	for( auto &door : doors_ )
//	{
//		door->door_decrement_timer();
//	}
//
//	doors_.erase( std::remove_if( doors_.begin(), doors_.end(),
//		[]( auto &tile ) { return !tile->door_is_open(); } ), doors_.end() );
//}

void controls::move_robot( unsigned int x, unsigned int y, const direction &d )
{
	auto &robot = terrain_->at( x, y ).occupant_;
	switch( d )
	{
		case direction::up:
			terrain_->at( x, y - 1 ).occupant_.swap( terrain_->at( x, y ).occupant_ );
			robot->y_ = y - 1; break;

		case direction::down:
			terrain_->at( x, y + 1 ).occupant_.swap( terrain_->at( x, y ).occupant_ );
			robot->y_ = y + 1; break;

		case direction::left:
			terrain_->at( x - 1, y ).occupant_.swap( terrain_->at( x, y ).occupant_ );
			robot->x_ = x - 1; break;

		case direction::right:
			terrain_->at( x + 1, y ).occupant_.swap( terrain_->at( x, y ).occupant_ );
			robot->x_ = x + 1; break;

		case direction::wait: break;
		default: throw std::invalid_argument( "ERROR: reading Robot direction" );
	}
}

bool controls::dangerous_tile( unsigned int x, unsigned int y )
{
	auto const &tile = terrain_->at( x, y );

	if( tile.type() == tile_type::precipice )
	{
		tile.occupant_->kill_robot();
		return true;
	} else if( tile.type() == tile_type::water && tile.occupant_->robot_type_ != robot_type::swimmer )
	{
		tile.occupant_->kill_robot();
		return true;
	} else
		return false;
}

bool controls::obstacle( unsigned int x, unsigned int y )
{
	auto &tile = terrain_->at( x, y );
	auto const &robot = tile.occupant_;

	/* obstructed robots can move again */
	if( robot->obstructed() )
	{
		robot->update_obstructed();
		return false;
	}

	auto const has_wheels = ( robot->robot_type_ == robot_type::follower ||
		robot->robot_type_ == robot_type::hunter || robot->robot_type_ == robot_type::sniffer ) ?
		false : true;

	auto const is_closed_door = ( terrain_->at( x, y ).door_ && !tile.door_is_open() ) ?
		true : false;

	/* obsacle for robots with wheels */
	if( tile.type() == tile_type::alien_weed && has_wheels )
	{
		robot->update_obstructed();
		return true;
	}

	/* obstacle for robots with legs */
	else if( tile.type() == tile_type::gravel && !has_wheels )
	{
		robot->update_obstructed();
		return true;
	}

	/* closed manual door as obstacle for patchbot */
	else if( is_closed_door && !tile.door_is_automatic() &&
		robot->robot_type_ == robot_type::patchbot )
	{
		robot->update_obstructed();
		tile.door_set_timer();
		doors_.push_back( &tile );
		return true;
	}

	/* closed automatic door as obstacle for enemy robots */
	else if( is_closed_door && tile.door_is_automatic() &&
		robot->robot_type_ != robot_type::patchbot )
	{
		robot->update_obstructed();
		tile.door_set_timer();
		doors_.push_back( &tile );
		return true;
	}

	return false;
}

bool controls::wall( unsigned int x, unsigned int y, robot_type r_type )
{
	/* map boundaries as walls */
	if( x >= terrain_->width() - 1 || y >= terrain_->height() - 1 )
		return true;

	auto const &tile = terrain_->at( x, y );

	/* environment as walls */

	/* actual walls */
	if( tile.type() == tile_type::concrete_wall )
		return true;
	/* breakable walls */
	else if( tile.type() == tile_type::rock_wall && r_type != robot_type::digger )
		return true;
	/* server (wall) for KI */
	else if( tile.type() == tile_type::server && r_type != robot_type::patchbot )
		return true;
	/* secret door (wall) for KI */
	else if( tile.type() == tile_type::secret_path && r_type != robot_type::patchbot )
		return true;
	/* automatic door (wall) for Patchbot */
	else if( tile.type() == tile_type::automatic_door && r_type == robot_type::patchbot )
		return true;

	/* this will be moved into another class when KI is implemented */
	/* robots as walls */
	if( tile.occupant_ )
	{	/* patchbot and bugger interpret all other robots as walls */
		if( tile.occupant_ && ( r_type == robot_type::patchbot || r_type == robot_type::bugger ) )
			return true;
		/* follower, hunter, sniffer interpret other enemies as walls */
		else if( tile.occupant_->robot_type_ != robot_type::patchbot &&
			( r_type == robot_type::follower || r_type == robot_type::hunter
				|| r_type == robot_type::sniffer ) )
			return true;
	}

	return false;
}

bool controls::wall_next_tile( unsigned int x, unsigned int y, direction d )
{
	switch( d )
	{
		case direction::up:
			return ( wall( x, y - 1, terrain_->at( x, y ).occupant_->robot_type_ ) ) ?
				true : false; break;

		case direction::down:
			return ( wall( x, y + 1, terrain_->at( x, y ).occupant_->robot_type_ ) ) ?
				true : false; break;

		case direction::left:
			return ( wall( x - 1, y, terrain_->at( x, y ).occupant_->robot_type_ ) ) ?
				true : false; break;

		case direction::right:
			return ( wall( x + 1, y, terrain_->at( x, y ).occupant_->robot_type_ ) ) ?
				true : false; break;

		case direction::wait: return false; break;

		default: throw std::invalid_argument( "ERROR: reading Robot direction" );
	}
}

void controls::update_doors()
{
	for( auto &door : doors_ )
		door->door_decrement_timer();

	/* remove all closed doors */
	//doors_.erase( std::remove_if( doors_.begin(), doors_.end(),
	//	[]( auto &tile ) { return !tile->door_is_open(); } ),
	//	doors_.end() );
}

bool controls::until_wall() const noexcept
{
	return until_wall_;
}