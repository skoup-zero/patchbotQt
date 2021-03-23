#pragma once

#include <state_machine.hpp>

namespace patchbot
{
	///	@class FSM for Bugger AI
	class bugger_ai: public state_machine
	{
		/* function represents state */
		void ( bugger_ai:: *state_ )( ) = &bugger_ai::follow_wall;
		
		/* coordinates of first FOUND wall to follow */
		unsigned int x_start_, y_start_;
		
		/* states */
		///	@brief		Follow a wall on left.
		void follow_wall();
		///	@brief		Find a wall if starting point is reached
		///				or there is no wall nearby.
		void find_wall();
		///	@brief		Wait if Robot is blocking your way.
		void wait();
		

		
		///	@brief		Change Buggers moving direction.
		void turn_clockwise();
		void turn_counterclockwise();
		
		///	@brief		Move in current_d_ direction.
		void move() const;

		bool next_tile_is_start() const;
		direction wall_in_vicinity() const;
		bool wall_on_left() const;

	public:
		bugger_ai( terrain &terrain, std::shared_ptr<robot> &self );
		~bugger_ai() override = default;

		/* execute current state */
		void process() override;
	};
}