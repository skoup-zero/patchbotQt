#pragma once

#include <terrain.hpp>

namespace patchbot
{
	///	@class as base for all AI's distributed in derived classes depending on context.
	class state_machine
	{
	protected:
		terrain &terrain_;
		std::shared_ptr<robot> self_;
		direction current_d_ = direction::wait;

		state_machine( terrain &terrain, std::shared_ptr<robot> &self );

	public:
		virtual void process();
		bool is_alive();
	};
}