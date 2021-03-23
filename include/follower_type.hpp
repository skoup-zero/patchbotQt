#pragma once

#include <state_machine.hpp>

namespace patchbot
{
	///	@class FSM for Follower, Hunter, Sniffer AI
	class follower_type: public state_machine
	{
		/* function represents state */
		void ( follower_type:: *state_ )( ) = &follower_type::wait;

		std::vector<direction> path_; /* saved path for Hunter */
		unsigned int path_pos_ = 0;	/* path position for Hunter */

		/* states */
		///	@brief		AI waits for opportunity to follow patchbot.
		void wait();

		///	@brief		AI follows Patchbot.
		void follow();

		///	@brief		Only Hunter can go to patchbots last seen position.
		void hunt();



		///	@brief		Move to patchbot and corrupt him if possible.
		void action();

		///	@brief		Get quickest direction to patchbot and reload
		///				dijkstra if necessary. 
		void update_direction();

		///	@brief		Follower and Hunters movement are affected
		///				by their vision.
		///	@return		true if line of sight of AI is blocked.
		bool line_of_sight_blocked();

		/// @brief		Hunter always saves patchbots last seen position.
		void save_path();

	public:
		follower_type( terrain &terrain, std::shared_ptr<robot> &self );
		~follower_type() override = default;

		/* execute current state */
		void process() override;
	};
}