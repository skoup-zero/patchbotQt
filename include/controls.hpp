#pragma once

#include <entity.hpp>
#include <terrain.hpp>

#include <qstring.h>
#include <qchar.h>
#include <qdebug.h>

#include <memory>
#include <string>
#include <stack>
#include <iostream>

namespace patchbot
{
	enum class direction
	{
		left,
		up,
		right,
		down,
		wait
	};

	class controls
	{
		bool until_wall_ = false;

		std::stack<direction> direction_;
		std::stack<std::uint8_t> frequency_;
		std::vector<tile *> doors_;
		terrain *terrain_;

	public:

		controls( terrain *terrain );
		controls( QString &instructions, terrain *terrain );

		static direction direction_to_enum( const char dir );

		void update_world();
		void update_patchbot();
		void update_instruction();
		//void one_step_patchbot();
		void move_robot( unsigned int x, unsigned int y, const direction &d );
		bool dangerous_tile( unsigned int x, unsigned int y ); /* done */
		bool obstacle( unsigned int x, unsigned int y ); /* done */
		bool wall( unsigned int x, unsigned int y, robot_type r_type ); /* done */
		bool wall_next_tile( unsigned int x, unsigned int y, direction d); /* done */

		void update_doors(); /* done? */

		bool until_wall() const noexcept;
	};
}
