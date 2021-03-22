#include <bugger.hpp>

using namespace patchbot;

bugger_ai::bugger_ai( terrain &terrain, std::shared_ptr<robot> &self )
	:state_machine( terrain, self )
	, x_start_{ self->x_ }
	, y_start_{ self->y_ }
{}

void bugger_ai::process()
{
	( this->*state_ )( );
}

void bugger_ai::follow_wall()
{
	/* if your starting point is reached move perpendicular */
	if( current_d_ != direction::wait && self_->x_ == x_start_ && self_->y_ == y_start_ )
	{
		turn_clockwise();
		state_ = &bugger_ai::find_wall;
		find_wall();
		return;
	}
	
	/* look for a wall */
	if( current_d_ == direction::wait )
		current_d_ = wall_in_vicinity();

	/* find a wall */
	if( current_d_ == direction::wait )
	{
		state_ = &bugger_ai::find_wall;
		current_d_ = direction::wait; /* next state chooses direction randomly */
		find_wall();
		return;
	}

	/* bugger follows walls around corners */
	if( !wall_on_left() )
		turn_counterclockwise();

	/* follow next wall */
	if( terrain_.wall_next_tile( self_->x_, self_->y_, current_d_ ) )
		turn_clockwise();
	
	/* wait for robot */
	if( terrain_.robot_next_tile( self_->x_, self_->y_, current_d_ ) )
	{
		state_ = &bugger_ai::wait;
		return;
	}

	move();
}

void bugger_ai::find_wall()
{
	/* Go to a random direction if no wall in vicinity */
	if( current_d_ == direction::wait )
		switch( rand() % 4 )
		{
			case 0: current_d_ = direction::up; break;
			case 1: current_d_ = direction::right; break;
			case 2: current_d_ = direction::down; break;
			case 3: current_d_ = direction::left; break;
		}

	/* wait if a robot is blocking your way */
	if( terrain_.robot_next_tile( self_->x_, self_->y_, current_d_ ) )
		return;

	/* if you reached a wall turn right and save it as your new start */
	if( terrain_.wall_next_tile( self_->x_, self_->y_, current_d_ ) )
	{
		turn_clockwise();
		state_ = &bugger_ai::follow_wall;
		x_start_ = self_->x_;
		y_start_ = self_->y_;
	}

	move();
}

void bugger_ai::wait()
{
	if( !terrain_.robot_next_tile( self_->x_, self_->y_, current_d_ ) )
	{
		state_ = &bugger_ai::follow_wall;
		move();
	}
}

void bugger_ai::turn_clockwise()
{
	switch( current_d_ )
	{
		case direction::up:		current_d_ = direction::right; break;
		case direction::right:	current_d_ = direction::down; break;
		case direction::down:	current_d_ = direction::left; break;
		case direction::left:	current_d_ = direction::up; break;
		default:				current_d_ = direction::wait;
	}
}

void bugger_ai::turn_counterclockwise()
{
	switch( current_d_ )
	{
		case direction::up:		current_d_ = direction::left; break;
		case direction::right:	current_d_ = direction::up; break;
		case direction::down:	current_d_ = direction::right; break;
		case direction::left:	current_d_ = direction::down; break;
		default:				current_d_ = direction::wait;
	}
}


void bugger_ai::move() const
{ 
	if( !terrain_.wall_next_tile( self_->x_, self_->y_, current_d_ ) &&
		!terrain_.robot_next_tile( self_->x_, self_->y_, current_d_ ) )
		terrain_.move_robot( self_->x_, self_->y_, current_d_ );
}

bool bugger_ai::next_tile_is_start() const
{
	switch( current_d_ )
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

	return direction::wait;
}

bool bugger_ai::wall_on_left() const
{
	switch( current_d_ )
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