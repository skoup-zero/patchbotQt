#include <pusher_type.hpp>

#include "bugger.hpp"

using namespace patchbot;

pusher_type_ai::pusher_type_ai( terrain &terrain, std::shared_ptr<robot> &self )
	:state_machine( terrain, self )
{}

void pusher_type_ai::action()
{
	( this->*state_ )( );
}

void pusher_type_ai::horizontal()
{
	if( self_->x_ < terrain_.patchbot_->x_ )
		d_current_ = direction::right;

	else if( self_->x_ > terrain_.patchbot_->x_ )
		d_current_ = direction::left;

	else
	{
		state_ = &pusher_type_ai::vertical;
		return;
	}

	if( terrain_.wall_next_tile( self_->x_, self_->y_, d_current_ ) )
	{
		state_ = &pusher_type_ai::vertical;
		return;
	}

	next_move();

	if( d_current_ == direction::undefined )
		state_ = &pusher_type_ai::vertical;
}

void pusher_type_ai::vertical()
{
	if( self_->y_ < terrain_.patchbot_->y_ )
		d_current_ = direction::down;

	else if( self_->y_ > terrain_.patchbot_->y_ )
		d_current_ = direction::up;

	else
	{
		state_ = &pusher_type_ai::horizontal;
		return;
	}

	if( terrain_.wall_next_tile( self_->x_, self_->y_, d_current_ ) )
	{
		state_ = &pusher_type_ai::horizontal;
		return;
	}

	next_move();

	if( d_current_ == direction::undefined )
		state_ = &pusher_type_ai::horizontal;
}

void pusher_type_ai::push_robot()
{
	std::shared_ptr<robot> target_r;

	switch( d_current_ )
	{
		case direction::up:
			target_r = terrain_.at( self_->x_, self_->y_ - 1 ).occupant_; break;

		case direction::right:
			target_r = terrain_.at( self_->x_ + 1, self_->y_ ).occupant_; break;

		case direction::down:
			target_r = terrain_.at( self_->x_, self_->y_ + 1 ).occupant_; break;

		case direction::left:
			target_r = terrain_.at( self_->x_ - 1, self_->y_ ).occupant_; break;

		default: d_current_ = direction::undefined; return;
	}

	if( !target_r )
		return;
	
	if( terrain_.wall_next_tile( target_r->x_, target_r->y_, d_current_ ) ||
		terrain_.robot_next_tile( target_r->x_, target_r->y_, d_current_ ) )
	{
		d_current_ = direction::undefined;
		return;
	}
	terrain_.move_robot( target_r->x_, target_r->y_, d_current_ );
	
	if( terrain_.dangerous_tile( target_r->x_, target_r->y_ ) )
		terrain_.add_grave( target_r->x_, target_r->y_ );	
}

void pusher_type_ai::next_move()
{
	if( self_->r_type_ == robot_type::digger )
		break_wall();
	
	if( terrain_.robot_next_tile( self_->x_, self_->y_, d_current_ ) )
		push_robot();

	if( !terrain_.wall_next_tile( self_->x_, self_->y_, d_current_ ) &&
		!terrain_.robot_next_tile( self_->x_, self_->y_, d_current_ ) &&
		d_current_ != direction::undefined )
		terrain_.move_robot( self_->x_, self_->y_, d_current_ );
	
	if( terrain_.dangerous_tile( self_->x_, self_->y_ ) )
		terrain_.add_grave( self_->x_, self_->y_ );
}

void pusher_type_ai::break_wall()
{
	switch( d_current_ )
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
				terrain_.at( self_->x_, self_->y_ + 1 ).break_wall(); break;

		case direction::left:
			if( self_->x_ > 0 &&
				terrain_.at( self_->x_ - 1, self_->y_ ).type() == tile_type::rock_wall )
				terrain_.at( self_->x_ - 1, self_->y_ ).break_wall();
			break;

		default: d_current_ = direction::undefined; return;
	}
}
