#pragma once

#include <terrain.hpp>

#include <QString>
#include <QChar>

#include <stack>

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

	/// @class updates all objects that can change in map.
	class controls
	{
		terrain *terrain_;
		std::stack<direction> direction_;
		std::stack<std::uint8_t> frequency_;
		std::vector<tile *> open_doors_;
		bool until_wall_ = false;

	public:

		///	@brief		Constructor for first initialization.
		///	@param		Terrain pointer to operate on.
		controls( terrain *terrain );

		///	@brief		Constructor with instructions.
		///	@details	Separates given instruction into two stacks frequency and direction.
		///	@param		instructions for Patchbot.
		///	@param		terrain pointer to operate on.
		controls( QString &instructions, terrain *terrain );

		///	@brief		Converts char commands to related direction enums.
		///	@throws		invalid_argument if direction char couldn't be converted.
		static direction direction_to_enum( char dir );

		///	@brief		Updates Map in following order: patchbot, enemies, doors.
		void update_world();

		///	@brief		Updates Patchbot.
		///	@details	Checks for obstacle and moves Patchbot accordingly.
		///				Kills Patchbot if it's on a deadly tile.
		///	@throws		std_out_of_range if stack is empty.
		void update_patchbot();

		///	@brief		Updates Patchbot's instructions.
		void update_instruction();

		///	@brief		Moves a robot to another tile.
		///	@details	Swaps the occupant pointer of two tiles.
		///	@param		x, y robot coordinates.
		///	@param		d direction to move.
		///	@throws		invalid_argument if tile has no robot or direction is invalid.
		void move_robot( unsigned int x, unsigned int y, const direction &d );

		///	@brief		Checks if tile at robot is dangerous.
		///	@param		x, y robot coordinates.
		///	@return		true if it kills robot.
		bool dangerous_tile( unsigned int x, unsigned int y );

		///	@brief		Checks if tile at robot is an obstacle.
		///	@param		x, y robot coordinates.
		///	@throws		invalid_argument if tile has no robot.
		///	@return		true if it's an obstacle.
		bool obstacle( unsigned int x, unsigned int y );

		///	@brief		Checks if a tile is a wall for a type of robot.
		///	@param		x, y tile coordinates.
		///	@param		r_type robot type. 
		///	@return		true if it's a wall.
		bool wall( unsigned int x, unsigned int y, robot_type r_type );

		///	@brief		Checks if the next tile for robot is a wall for it's type.
		///	@param		x, y tile coordinates.
		///	@param		d direction to check next tile.
		///	@throws		invalid_argument if tile has no robot.
		///	@return		true if the next tile is a wall for robot.
		bool wall_next_tile( unsigned int x, unsigned int y, const direction &d );

		///	@brief		Updates all open doors.
		void update_doors();

		/// GETTER 
		bool until_wall() const noexcept;
	};
}