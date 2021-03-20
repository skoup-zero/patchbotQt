#pragma once

#include <terrain.hpp>

namespace patchbot
{
	///	@class as base for all AI's distributet in derived classes depending on context.
	class state_machine
	{
	protected:
		terrain &terrain_;
		std::shared_ptr<robot> self_;

		state_machine( terrain &terrain, std::shared_ptr<robot> &self );

	public:

		virtual void action();
		bool is_alive() const noexcept;
	};
}