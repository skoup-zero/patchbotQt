#pragma once

#include <terrain.hpp>

namespace patchbot
{
	class state_machine
	{
	protected:
		terrain &terrain_;
		std::shared_ptr<robot> robot_;
		
		state_machine( terrain &terrain, std::shared_ptr<robot> &robot );
	
	public:
		
		virtual void action();
	};
	
	class bugger_ki : public state_machine
	{
		/* function represents state */
		void ( bugger_ki::*state_ )( ) = &bugger_ki::follow_wall;
		direction d_current_ = direction::undefined;
		unsigned int x_start_, y_start_;
	
	public:
		
		bugger_ki( terrain &terrain, std::shared_ptr<robot> &robot );

		/* execute current state */
		void action() override;

		/* states */
		void find_wall();
		void wait();
		void follow_wall();

		direction wall_in_vicinity();
		bool wall_on_left();
		bool next_tile_is_start();
		void turn();
	};
}