#pragma once

#include <state_machine.hpp>

namespace patchbot
{

	class bugger_ai: public state_machine
	{
		/* function represents state */
		void ( bugger_ai:: *state_ )( ) = &bugger_ai::follow_wall;
		direction d_current_ = direction::undefined;
		unsigned int x_start_, y_start_;

		/* states */
		void find_wall();
		void wait();
		void follow_wall();

		void turn();
		void move() const;
		
		direction wall_in_vicinity() const;
		bool wall_on_left() const;
		bool next_tile_is_start() const;

	public:
		bugger_ai( terrain &terrain, std::shared_ptr<robot> &self );

		/* execute current state */
		void action() override;
	};
}