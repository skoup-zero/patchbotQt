#pragma once

#include <state_machine.hpp>

namespace patchbot
{
	///	@class FSM for Pusher, Swimmer, Digger AI
	class pusher_type_ai: public state_machine
	{
		/* function represents state */
		void ( pusher_type_ai:: *state_ )( ) = &pusher_type_ai::horizontal;

		/* states */
		///	@brief		current axis the AI is moving.
		void horizontal();
		void vertical();



		///	@brief		AI executes it's action.
		void action();

		///	@brief		Digger turns rock wall on target tile into gravel.
		void break_wall() const;

		///	@brief		Switch between both states.
		void switch_state();

	public:
		pusher_type_ai( terrain &terrain, std::shared_ptr<robot> &self );
		~pusher_type_ai() override = default;

		/* execute current state */
		void process() override;
	};
}