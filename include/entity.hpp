#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <map>

namespace patchbot
{
	enum class direction
	{
		left,
		up,
		right,
		down,
		wait,
		undefined
	};

	enum class robot_type
	{
		patchbot,
		bugger,
		pusher,
		digger,
		swimmer,
		follower,
		hunter,
		sniffer,
		dead
	};

	/// map to assign symbols to enums
	const std::map<char, robot_type> robot_map{
		{ 'p', robot_type::patchbot },
		{ '1', robot_type::bugger },
		{ '2', robot_type::pusher },
		{ '3', robot_type::digger },
		{ '4', robot_type::swimmer },
		{ '5', robot_type::follower },
		{ '6', robot_type::hunter },
		{ '7', robot_type::sniffer },
		{ 'X', robot_type::dead }
	};

	/// @class	robot represents a robot that interacts with the player
	///			and the environment
	class robot
	{
		bool obstructed_ = false;
		robot_type r_type_;
	
	public:
		unsigned int x_, y_;

		robot( robot_type type );

		/// @brief		Swtiches obstructed_ to opposite bool
		void update_obstructed();

		/// Getter
		robot_type type() const noexcept;
		bool obstructed() const noexcept;
	};

	enum class tile_type
	{
		patchbot_start,
		enemy_start,
		steel_plates,
		precipice,
		water,
		server,
		alien_weed,
		gravel,
		secret_path,
		manual_door,
		manual_door_open,
		automatic_door,
		automatic_door_open,
		concrete_wall,
		rock_wall
	};

	/// map to assign symbols to enums
	const std::map<char, tile_type> tile_map{
		{ ' ', tile_type::steel_plates },
		{ 'O', tile_type::precipice },
		{ '~', tile_type::water },
		{ 'P', tile_type::server },
		{ 'g', tile_type::alien_weed },
		{ '.', tile_type::gravel },
		{ 'x', tile_type::secret_path },
		{ 'd', tile_type::manual_door },
		{ 'D', tile_type::automatic_door },
		{ '#', tile_type::concrete_wall },
		{ 'M', tile_type::rock_wall }
	};

	/// @class	tile represents a tile from a map which might have an occupant 
	class tile
	{
		std::uint8_t door_timer_ = 0;
		std::uint8_t grave_timer_ = 0;
		tile_type t_type_;
		bool grave_ = false;

	public:
		const bool door_;
		std::shared_ptr<robot> occupant_;

		tile( tile_type type, bool door = false );

		/// @brief		Sets timer of door to 10.
		void door_set_timer();
		void door_decrement_timer();

		///	@brief		Sets timer of grave to 5.
		void grave_set_timer();
		void grave_decrement_timer();

		///	@brief		tile type changes from rock_wall to gravel.
		void break_wall();

		///	@brief		value of each tile for dijkstra.
		///	@return		0 if robot, danger or wall.
		///				1 if it isn't an obstacle.
		///				2 if it is an obstacle.
		int node_cost() const;

		///	GETTER 
		tile_type type() const noexcept;
		bool door_is_open() const;
		bool door_is_automatic() const;
		bool is_grave() const noexcept;
	};
}