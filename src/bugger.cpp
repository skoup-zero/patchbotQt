#include <bugger.hpp>

using namespace patchbot;

bugger_ai::bugger_ai( terrain &terrain, std::shared_ptr<robot> &self )
	:state_machine( terrain, self )
	, x_start_{ self->x_ }
	, y_start_{ self->y_ }
{}

void bugger_ai::action()
{
	( this->*state_ )( );
}

void bugger_ai::find_wall()
{
	if( terrain_.dangerous_tile( self_->x_, self_->y_ ) )
		self_->kill_robot();
	
	if( d_current_ == direction::undefined )
		switch( rand() % 4 )
		{
			case 0: d_current_ = direction::up; break;
			case 1: d_current_ = direction::right; break;
			case 2: d_current_ = direction::down; break;
			case 3: d_current_ = direction::left; break;
		}

	if( terrain_.robot_next_tile( self_->x_, self_->y_, d_current_ ) )
		return;

	if( terrain_.wall_next_tile( self_->x_, self_->y_, d_current_ ) )
	{
		turn();
		state_ = &bugger_ai::follow_wall;
		x_start_ = self_->x_;
		y_start_ = self_->y_;
	}

	move();
}

void bugger_ai::wait()
{
	if( terrain_.dangerous_tile( self_->x_, self_->y_ ) )
		self_->kill_robot();
	
	if( !terrain_.robot_next_tile( self_->x_, self_->y_, d_current_ ) )
	{
		state_ = &bugger_ai::follow_wall;
		//move();
	}
}

void bugger_ai::follow_wall()
{
	if( terrain_.dangerous_tile( self_->x_, self_->y_ ) )
		self_->kill_robot();
	
	if( d_current_ == direction::undefined )
		d_current_ = wall_in_vicinity();

	if( d_current_ == direction::undefined || !wall_on_left() )
	{
		state_ = &bugger_ai::find_wall;
		d_current_ = direction::undefined;
		find_wall();
		return;
	}

	if( next_tile_is_start() )
	{
		turn();
		state_ = &bugger_ai::find_wall;
	}

	if( terrain_.robot_next_tile( self_->x_, self_->y_, d_current_ ) )
	{
		state_ = &bugger_ai::wait;
		return;
	}

	if( terrain_.wall_next_tile( self_->x_, self_->y_, d_current_ ) )
		turn();

	move();
}

void bugger_ai::turn()
{
	switch( d_current_ )
	{
		case direction::up:		d_current_ = direction::right; break;
		case direction::right:	d_current_ = direction::down; break;
		case direction::down:	d_current_ = direction::left; break;
		case direction::left:	d_current_ = direction::up; break;
		default:				d_current_ = direction::wait;
	}
}

void bugger_ai::move() const
{
	if( !terrain_.wall_next_tile( self_->x_, self_->y_, d_current_ ) &&
		!terrain_.robot_next_tile( self_->x_, self_->y_, d_current_ ) )
		terrain_.move_robot( self_->x_, self_->y_, d_current_ );
}


direction bugger_ai::wall_in_vicinity() const
{
	/* choose direction to go if wall is nearby  */
	if( terrain_.wall_next_tile( self_->x_, self_->y_, direction::up ) )
		return direction::right;

	if( terrain_.wall_next_tile( self_->x_, self_->y_, direction::right ) )
		return direction::down;

	if( terrain_.wall_next_tile( self_->x_, self_->y_, direction::down ) )
		return direction::left;

	if( terrain_.wall_next_tile( self_->x_, self_->y_, direction::left ) )
		return direction::up;

	return direction::undefined;
}

bool bugger_ai::wall_on_left() const
{
	switch( d_current_ )
	{
		case direction::up:
			return terrain_.wall_next_tile( self_->x_, self_->y_, direction::left );

		case direction::right:
			return terrain_.wall_next_tile( self_->x_, self_->y_, direction::up );

		case direction::down:
			return terrain_.wall_next_tile( self_->x_, self_->y_, direction::right );

		case direction::left:
			return terrain_.wall_next_tile( self_->x_, self_->y_, direction::down );
	}
	return false;
}


bool bugger_ai::next_tile_is_start() const
{
	switch( d_current_ )
	{
		case direction::up:
			return self_->x_ == x_start_ && self_->y_ - 1 == y_start_;

		case direction::right:
			return self_->x_ + 1 == x_start_ && self_->y_ == y_start_;

		case direction::down:
			return self_->x_ == x_start_ && self_->y_ + 1 == y_start_;

		case direction::left:
			return self_->x_ == x_start_ - 1 && self_->y_ - 1 == y_start_;
	}
	return false;
}