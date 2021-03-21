#pragma once

#include <state_machine.hpp>

namespace patchbot
{
	class follower_type: public state_machine
	{
		/* function represents state */
		void ( follower_type:: *state_ )( ) = &follower_type::wait;
		direction d_current_ = direction::undefined;
		std::vector<direction> path_;
		unsigned int path_pos_ = 0;

		/* states */
		void wait();
		void follow();
		void hunt(); /* Hunter exclusive */

		bool line_of_sight();
		void save_path();
		
	public:
		follower_type( terrain &terrain, std::shared_ptr<robot> &self );

		/* execute current state */
		void action() override;
	};
}