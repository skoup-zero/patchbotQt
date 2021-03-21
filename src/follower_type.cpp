#include <follower_type.hpp>

using namespace patchbot;

follower_type::follower_type( terrain &terrain, std::shared_ptr<robot> &self )
	:state_machine( terrain, self )
{}

void follower_type::action()
{
	( this->*state_ )( );
}

void follower_type::wait()
{
	if( terrain_.dijkstra_at( self_->x_, self_->y_ ) != direction::undefined
		&& self_->r_type_ == robot_type::sniffer )
		state_ = &follower_type::follow;

	if( line_of_sight() && self_->r_type_ == robot_type::follower )
		state_ = &follower_type::follow;

	if( line_of_sight() && self_->r_type_ == robot_type::hunter
		&& terrain_.dijkstra_at( self_->x_, self_->y_ ) != direction::undefined )
	{
		save_path();
		state_ = &follower_type::follow;
	}
}

void follower_type::follow()
{
	if( !line_of_sight() && self_->r_type_ == robot_type::follower )
	{
		state_ = &follower_type::wait;
		return;
	}

	if( !line_of_sight() && self_->r_type_ == robot_type::hunter )
	{
		state_ = &follower_type::hunt;
		return;
	}

	d_current_ = terrain_.dijkstra_at( self_->x_, self_->y_ );
	terrain_.move_robot( self_->x_, self_->y_, d_current_ );
}

void follower_type::hunt()
{
	if( path_.empty() )
	{
		state_ = &follower_type::wait;
		path_.clear();
		return;
	}

	if( path_pos_ == path_.size() - 2 )
	{
		state_ = &follower_type::wait;
		terrain_.move_robot( self_->x_, self_->y_, path_[path_pos_] );
		path_.clear();
	}
	else
	{
		terrain_.move_robot( self_->x_, self_->y_, path_[path_pos_] );
		path_pos_++;
		terrain_.move_robot( self_->x_, self_->y_, path_[path_pos_] );
	}
}

bool follower_type::line_of_sight()
{
	/* AI coordinates */
	int x0 = static_cast<int>( self_->x_ );
	int y0 = static_cast<int>( self_->y_ );

	/* Patchbot coordinates */
	int x1 = static_cast<int>( terrain_.patchbot_->x_ );
	int y1 = static_cast<int>( terrain_.patchbot_->y_ );

	int dx = abs( x1 - x0 ),
		sx = x0 < x1 ? 1 : -1;
	int dy = -abs( y1 - y0 ),
		sy = y0 < y1 ? 1 : -1;

	int err = dx + dy, e2; /* error value e_xy */

	const auto blocked_sight = [&]( const unsigned int x, const unsigned int y )
	{
		if( x >= terrain_.width() || y >= terrain_.height() )
			return true;

		const tile &tile = terrain_.at( x, y );

		if( self_->x_ == x && self_->y_ == y )
			return false;

		if( terrain_.wall( x, y, self_->r_type_ ) )
			return true;

		if( tile.door_ && !tile.door_is_open() )
			return true;

		return false;
	};

	while( !( x0 == x1 && y0 == y1 ) )
	{
		if( blocked_sight( x0, y0 ) )
			return false;

		e2 = 2 * err;

		if( e2 > dy )
		{
			err += dy; x0 += sx;
		} /* e_xy+e_x > 0 */

		if( e2 < dx )
		{
			err += dx; y0 += sy;
		} /* e_xy+e_y < 0 */
	}
	return true;
}

void follower_type::save_path()
{
	unsigned int current_x = self_->x_;
	unsigned int current_y = self_->y_;
	unsigned int pb_x = terrain_.patchbot_->x_;
	unsigned int pb_y = terrain_.patchbot_->y_;
	bool searching_pb = true;

	auto update_coordinates = [&]( unsigned int &x, unsigned int &y, direction d )
	{
		switch( d )
		{
			case direction::up: y--; break;
			case direction::right: x++; break;
			case direction::down: y++; break;
			case direction::left: x--; break;
		}
	};

	path_.clear();
	direction current_d;
	while( searching_pb )
	{
		current_d = terrain_.dijkstra_at( current_x, current_y );
		path_.push_back( current_d );
		update_coordinates( current_x, current_y, current_d );

		if( current_x == pb_x && current_y == pb_y )
			searching_pb = false;
	}
}