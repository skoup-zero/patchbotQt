#include <follower_type.hpp>

using namespace patchbot;

follower_type::follower_type( terrain &terrain, std::shared_ptr<robot> &self )
	:state_machine( terrain, self )
{}

void follower_type::process()
{
	( this->*state_ )( );
}

void follower_type::wait()
{
	update_direction();

	/* all types wait if patchbot is unreachable */
	if( current_d_ == direction::wait )
		return;

	/* follower and hunter wait until patchbot is in sight */
	if( self_->type() != robot_type::sniffer && line_of_sight_blocked() )
		return;

	/* hunter saves path to patchbot */
	if( self_->type() == robot_type::hunter )
		save_path();

	state_ = &follower_type::follow;
}

void follower_type::follow()
{
	update_direction();

	/* follower, sniffer wait and hunter hunts if patchbot is not reachable */
	if( current_d_ == direction::wait )
	{
		state_ = ( self_->type() == robot_type::hunter ) ? &follower_type::hunt : &follower_type::wait;
		return;
	}

	/* follower waits and hunter hunts if patchbot is not in sight */
	if( line_of_sight_blocked() && self_->type() != robot_type::sniffer )
	{
		state_ = ( self_->type() == robot_type::hunter ) ? &follower_type::hunt : &follower_type::wait;
		return;
	}
	action();
	
	/* hunter moves twice and saves path */
	if( self_->type() == robot_type::hunter )
	{
		action();
		save_path();
	}
}

void follower_type::hunt()
{
	/* follow Patchbot again if you see him */
	if(!line_of_sight_blocked() )
	{
		state_ = &follower_type::follow;
		save_path();
		return;
	}
	
	/* hunting specific action */
	auto follow_track = [&]()
	{
		current_d_ = path_[path_pos_];

		/* wait if a robot is on next tile */
		if( terrain_.robot_next_tile( self_->x_, self_->y_, current_d_ ) )
			return;

		if( terrain_.obstacle( self_->x_, self_->y_, current_d_ ) )
			return;

		path_pos_++;
		terrain_.move_robot( self_->x_, self_->y_, current_d_ );
	};
	
	if( path_pos_ < path_.size() - 1 )
	{
		follow_track();
		follow_track();
		return;
	}

	if( path_pos_ == path_.size() - 1 )
		follow_track();

	state_ = &follower_type::wait;
}

void follower_type::action()
{
	current_d_ = terrain_.dijkstra_at( self_->x_, self_->y_ );

	/* wait and reload dijkstra path if a robot is on next tile */
	if( terrain_.robot_next_tile( self_->x_, self_->y_, current_d_ ) )
	{
		/* corrupt patchbot if its on the next tile */
		terrain_.corrupt_patchbot( self_->x_, self_->y_, current_d_ );

		current_d_ = direction::wait;
		return;
	}

	if( terrain_.obstacle( self_->x_, self_->y_, current_d_ ) )
		return;

	terrain_.move_robot( self_->x_, self_->y_, current_d_ );
}

void follower_type::update_direction()
{
	/* load dijkstra path if AI can't reach patchbot */
	if( current_d_ == direction::wait )
		terrain_.load_dijkstra_path();

	current_d_ = terrain_.dijkstra_at( self_->x_, self_->y_ );
}


bool follower_type::line_of_sight_blocked()
{
	/* AI coordinates */
	int x0 = static_cast<int>( self_->x_ );
	int y0 = static_cast<int>( self_->y_ );

	/* Patchbot coordinates */
	const int x1 = static_cast<int>( terrain_.patchbot_->x_ );
	const int y1 = static_cast<int>( terrain_.patchbot_->y_ );

	/* lambda function returns true if AI can't see through that tile */
	auto blocked_sight = [&]( const unsigned int x, const unsigned int y )
	{	
		if( x >= terrain_.width() || y >= terrain_.height() )
			return true;

		const tile &tile = terrain_.at( x, y );

		if( terrain_.wall( x, y, self_->type() ) )
			return true;

		if( tile.door_ && !tile.door_is_open() )
			return true;

		return false;
	};

	/* distance on axis */
	const int dx = abs( x1 - x0 );
	const int dy = -abs( y1 - y0 );

	/* slope on axis */
	const int	sx = ( x0 < x1 ) ? 1 : -1; 
	const int	sy = ( y0 < y1 ) ? 1 : -1;

	int err = dx + dy; /* error value */

	while( !( x0 == x1 && y0 == y1 ) ) /* until patchbot is reached */
	{
		const int e2 = 2 * err;

		if( e2 > dy )
		{
			err += dy;
			x0 += sx;
		}

		if( e2 < dx )
		{
			err += dx;
			y0 += sy;
		}
		
		if( blocked_sight( x0, y0 ) )
			return true;
	}
	
	return false;
}

void follower_type::save_path()
{
	unsigned int current_x = self_->x_;
	unsigned int current_y = self_->y_;
	bool searching_pb = true;

	auto update_coordinates = [&]( unsigned int &x, unsigned int &y, const direction d )
	{
		switch( d )
		{
			case direction::up: y--; break;
			case direction::right: x++; break;
			case direction::down: y++; break;
			case direction::left: x--; break;
		}
	};

	/* reset path */
	path_pos_ = 0;
	path_.clear();

	while( searching_pb )
	{
		const direction &current = terrain_.dijkstra_at( current_x, current_y );

		path_.push_back( current );
		update_coordinates( current_x, current_y, current );

		/* stop if patchbot reached  */
		if(  current_x == terrain_.patchbot_->x_ && current_y == terrain_.patchbot_->y_
			|| terrain_.wall( current_x, current_y, self_->type() ) ) /* to prevent deadlocks */
			searching_pb = false;
	}
}