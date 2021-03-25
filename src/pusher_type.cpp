#include <pusher_type.hpp>

using namespace patchbot;

pusher_type_ai::pusher_type_ai( terrain &terrain, std::shared_ptr<robot> &self )
	:state_machine( terrain, self )
{}

void pusher_type_ai::process()
{
	( this->*state_ )( );
}

void pusher_type_ai::horizontal()
{
	if( self_->x_ < terrain_.patchbot_->x_ )
		current_d_ = direction::right;

	else if( self_->x_ > terrain_.patchbot_->x_ )
		current_d_ = direction::left;

	else
	{	/* same x-axis */
		switch_state();
		return;
	}
	action();
}

void pusher_type_ai::vertical()
{
	if( self_->y_ < terrain_.patchbot_->y_ )
		current_d_ = direction::down;

	else if( self_->y_ > terrain_.patchbot_->y_ )
		current_d_ = direction::up;

	else
	{	/* same y-axis */
		switch_state();
		return;
	}
	action();
}

void pusher_type_ai::action()
{
	/* switch on wall */
	if( terrain_.wall_next_tile( self_->x_, self_->y_, current_d_ ) )
	{
		switch_state();
		return;
	}

	/* digger destroys rock walls */
	if( self_->type() == robot_type::digger )
		break_wall();

	/* wait on obstacle */
	if( terrain_.obstacle( self_->x_, self_->y_, current_d_ ) )
		return;

	/* push robot on target tile */
	if( terrain_.robot_next_tile( self_->x_, self_->y_, current_d_ ) )
		terrain_.push_robot( self_->x_, self_->y_, current_d_ );

	/* wait if robot can't be pushed */
	if( !terrain_.robot_next_tile( self_->x_, self_->y_, current_d_ ) )
		terrain_.move_robot( self_->x_, self_->y_, current_d_ );
	else
	{
		switch_state();
		return;
	}

	/* destroy this AI if it drives on dangerous tile */
	if( terrain_.dangerous_tile( self_->x_, self_->y_ ) )
		terrain_.kill_robot( self_->x_, self_->y_ );
}

void pusher_type_ai::break_wall() const
{
	switch( current_d_ )
	{
		case direction::up:
			if( self_->y_ > 0 &&
				terrain_.at( self_->x_, self_->y_ - 1 ).type() == tile_type::rock_wall )
				terrain_.at( self_->x_, self_->y_ - 1 ).break_wall();
			break;

		case direction::right:
			if( self_->x_ < terrain_.width() - 1 &&
				terrain_.at( self_->x_ + 1, self_->y_ ).type() == tile_type::rock_wall )
				terrain_.at( self_->x_ + 1, self_->y_ ).break_wall();
			break;

		case direction::down:
			if( self_->y_ < terrain_.height() - 1 &&
				terrain_.at( self_->x_, self_->y_ + 1 ).type() == tile_type::rock_wall )
				terrain_.at( self_->x_, self_->y_ + 1 ).break_wall();
			break;

		case direction::left:
			if( self_->x_ > 0 &&
				terrain_.at( self_->x_ - 1, self_->y_ ).type() == tile_type::rock_wall )
				terrain_.at( self_->x_ - 1, self_->y_ ).break_wall();
			break;
	}
}

void pusher_type_ai::switch_state()
{
	state_ = ( state_ == &pusher_type_ai::horizontal ) ?
		&pusher_type_ai::vertical : &pusher_type_ai::horizontal;
}