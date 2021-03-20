#pragma once

#include <state_machine.hpp>

namespace patchbot
{
	class pusher_type_ai: public state_machine
	{
		/* function represents state */
		void ( pusher_type_ai:: *state_ )( ) = &pusher_type_ai::horizontal;
		direction d_current_ = direction::undefined;

		/* states */
		void horizontal();
		void vertical();

		void push_robot();
		void next_move();

		/* digger specific */
		void break_wall();
		
	public:
		pusher_type_ai( terrain &terrain, std::shared_ptr<robot> &self );
		
		/* execute current state */
		void action() override;
	};
}