#pragma once

#include <terrain.hpp>

namespace patchbot
{
	class state_machine
	{
	protected:
		terrain &terrain_;
		std::shared_ptr<robot> robot_;
		direction target_ ;
		
		state_machine( terrain &terrain, std::shared_ptr<robot> &robot );
	
	public:
		
		virtual void action();
		direction target() const noexcept;
	};
	
	class bugger_ki : public state_machine
	{
		void ( bugger_ki::*state_ )( ) = &bugger_ki::follow_wall; /* function represents state */

	public:
		
		bugger_ki( terrain &terrain, std::shared_ptr<robot> &robot );

		void action() override;
		void find_wall();
		void wait();
		void follow_wall();
	};
}