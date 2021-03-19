#include <state_machine.hpp>

using namespace patchbot;

state_machine::state_machine( terrain &terrain, std::shared_ptr<robot> &robot )
	:terrain_{ terrain }
	, robot_{ robot }
{}

void state_machine::action()
{
	std::cout << "THIS SHOULD NOT BE INVOKED" << std::endl;
}


bugger_ki::bugger_ki( terrain &terrain, std::shared_ptr<robot> &robot )
	: state_machine( terrain, robot )
	, x_start_{ robot->x_ }
	, y_start_{ robot->y_ }
{}

void bugger_ki::action()
{
	( this->*state_ )( );
}

void bugger_ki::find_wall()
{
	if( d_current_ == direction::undefined )
		switch( rand() % 4 )
		{
			case 0: d_current_ = direction::up; break;
			case 1: d_current_ = direction::right; break;
			case 2: d_current_ = direction::down; break;
			case 3: d_current_ = direction::left; break;
		}

	if( terrain_.robot_next_tile( robot_->x_, robot_->y_, d_current_ ) )
	{
		//turn();
		return;
	}

	if( terrain_.wall_next_tile( robot_->x_, robot_->y_, d_current_ ) )
	{
		turn();
		state_ = &bugger_ki::follow_wall;
		x_start_ = robot_->x_;
		y_start_ = robot_->y_;
		//follow_wall();
	}
	else if( !terrain_.robot_next_tile( robot_->x_, robot_->y_, d_current_ ) )
		terrain_.move_robot( robot_->x_, robot_->y_, d_current_ );
}

void bugger_ki::wait()
{
	if( !terrain_.robot_next_tile( robot_->x_, robot_->y_, d_current_ ) )
	{
		state_ = &bugger_ki::follow_wall;
		//follow_wall();
	}
}

void bugger_ki::follow_wall()
{
	if( d_current_ == direction::undefined )
		d_current_ = wall_in_vicinity();

	if( d_current_ == direction::undefined || !wall_on_left() )
	{
		state_ = &bugger_ki::find_wall;
		d_current_ = direction::undefined;
		//find_wall();
		return;
	}
	if(next_tile_is_start())
	{
		turn();
		state_ = &bugger_ki::find_wall;
	}

	/* Patchbot doesn't move if the next tile is a wall */
	if( terrain_.robot_next_tile( robot_->x_, robot_->y_, d_current_ ) )
	{
		state_ = &bugger_ki::wait;
		//wait();
		return;
	}

		if( terrain_.wall_next_tile( robot_->x_, robot_->y_, d_current_ ) )
			turn();

	if( !terrain_.wall_next_tile( robot_->x_, robot_->y_, d_current_ ) &&
		!terrain_.robot_next_tile( robot_->x_, robot_->y_, d_current_ ))
		terrain_.move_robot( robot_->x_, robot_->y_, d_current_ );

}

direction bugger_ki::wall_in_vicinity()
{
	/* choose direction to go if wall is nearby  */
	if( terrain_.wall_next_tile( robot_->x_, robot_->y_, direction::up ) )
		return direction::right;

	if( terrain_.wall_next_tile( robot_->x_, robot_->y_, direction::right ) )
		return direction::down;

	if( terrain_.wall_next_tile( robot_->x_, robot_->y_, direction::down ) )
		return direction::left;

	if( terrain_.wall_next_tile( robot_->x_, robot_->y_, direction::left ) )
		return direction::up;

	return direction::undefined;
}

bool bugger_ki::wall_on_left()
{
	switch( d_current_ )
	{
		case direction::up:
			return terrain_.wall_next_tile( robot_->x_, robot_->y_, direction::left );

		case direction::right:
			return terrain_.wall_next_tile( robot_->x_, robot_->y_, direction::up );

		case direction::down:
			return terrain_.wall_next_tile( robot_->x_, robot_->y_, direction::right );

		case direction::left:
			return terrain_.wall_next_tile( robot_->x_, robot_->y_, direction::down );
	}
	return false;
}


bool bugger_ki::next_tile_is_start()
{
	switch( d_current_ )
	{
		case direction::up:
			return robot_->x_ == x_start_ && robot_->y_ - 1 == y_start_;

		case direction::right:
			return robot_->x_ + 1 == x_start_ && robot_->y_ == y_start_;

		case direction::down:
			return robot_->x_ == x_start_ && robot_->y_ + 1 == y_start_;

		case direction::left:
			return robot_->x_ == x_start_ - 1 && robot_->y_ - 1 == y_start_;
	}
	return false;
}


void bugger_ki::turn()
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
