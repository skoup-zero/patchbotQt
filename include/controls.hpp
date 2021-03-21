#pragma once

#include <terrain.hpp>
#include <bugger.hpp>
#include <pusher_type.hpp>
#include <follower_type.hpp>

namespace patchbot
{

	/// @class updates all objects that can change in map.
	class controls
	{
		terrain &terrain_;
		std::vector<direction> direction_;
		std::vector<int> frequency_;
		std::vector<tile *> open_doors_;
		bool until_wall_ = false;

		std::vector<std::unique_ptr<state_machine>> enemy_ais_;

	public:

		///	@brief		Constructor for first initialization.
		///	@param		terrain pointer to operate on.
		controls( terrain &t );
		controls &operator=( const controls & );

		///	@brief		adds and removes instructions.
		void add_instruction( direction d, int frequency );
		void remove_instruction();

		///	@brief		Updates Map in following order: patchbot, enemies, doors.
		void update_world();

		///	@brief		Updates Patchbot.
		///	@details	Checks for obstacle and moves Patchbot accordingly.
		///				Kills Patchbot if it's on a deadly tile.
		///	@throws		std_out_of_range if stack is empty.
		void update_patchbot();

		///	@brief		Updates Patchbot's instructions.
		void update_instruction();

		///	@brief		Add and update all enemy ai's in the game.
		void init_enemies();
		void update_enemies();

		///	@brief		Check if Patchbot reached the server.
		bool check_win() const;

		/// GETTER
		bool patchbot_dead() const;
		bool until_wall() const noexcept;
		bool patchbot_obstructed() const noexcept;
		bool instructions_empty() const noexcept;
		terrain terrain() const noexcept;
	};
}