#pragma once

#include <entity.hpp>
#include <exceptions.hpp>

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

namespace patchbot
{

	/// @class with width, height and tiles representing map
	class terrain
	{
		unsigned int width_;
		unsigned int height_;
		std::vector<tile> tiles_;
		std::vector<tile *> open_doors_;
		std::vector<tile *> graves_;
		bool patchbot_corrupted_ = false;

		terrain( std::vector<tile> &&tiles, std::vector<std::shared_ptr<robot>> &&robots_,
			std::shared_ptr<robot> patchbot, unsigned int width, unsigned int height );

	public:
		std::shared_ptr<robot> patchbot_;
		std::vector<std::shared_ptr<robot>> robots_;

		/* shortest path to Patchbot */
		std::vector<std::pair<unsigned int, direction>> dijkstra_path_tree_;

		///	@brief		reads the symbols from a txt file .
		///	@details	creates a vector with instances of Tile and pointers to Robot, 
		///				which will be returned as a map represented by the class terrain.
		///	@param		path to a file, that represents a patchbot terrain.
		///	@throws		std::invalid_argument if path isn´t a regular file.
		///	@throws		patchbot_exception if header of txt is invalid
		///	@throws		patchbot_exception if width, height or symbol is incorrect
		///				or goal, patchbot doesn´t exist
		///	@return		instance of terrain
		static terrain load_map_from_file( const std::filesystem::path &path );

		///	@brief		returns a tile at given index
		///	@param		index of terrain
		///	@throws		std::out_of_range if index is out of range
		///	@return		a tile at given index
		const tile &at( unsigned int index ) const;

		///	@brief		returns a tile at given coordinates
		///	@param		x, y coordinates of terrain
		///	@throws		std::out_of_range if coordinates is out of range 
		///	@return		a tile at given coordinates
		tile &at( unsigned int x, unsigned int y );


		/* ROBOT INTERACTION */

		///	@brief		Moves a robot to another tile.
		///	@details	Swaps the occupant pointer of two tiles.
		///	@param		x, y robot coordinates.
		///	@param		d direction to action.
		///	@throws		invalid_argument if tile has no robot or direction is invalid.
		void move_robot( unsigned int x, unsigned int y, direction d );

		///	@brief		Pushes a roboter (if possible) to another tile and
		///				kills if it lands on a dangerous tile.
		///	@param		x, y coordinates of pushing roboter.
		///	@param		d direction to push.
		void push_robot( unsigned int x, unsigned int y, direction d );

		///	@brief		Kills robot at a position and replaces it with a grave.
		///	@details	Robot death is marked as a nullptr on terrain (map) so
		///				other robots can't interact with it anymore.
		///	@param		x, y as robot coordinates.
		void kill_robot( unsigned int x, unsigned int y );

		///	@brief		Follower type robots corrupt patchbot.
		///	@param		x, y coordinates of attacking robot.
		///	@param		d direction attacking robot is heading.
		void corrupt_patchbot( unsigned int x, unsigned int y, direction d );

		///	@brief		Checks if tile at robot is dangerous.
		///	@param		x, y robot coordinates.
		///	@return		true if it kills robot.
		bool dangerous_tile( unsigned int x, unsigned int y );

		///	@brief		Checks if tile at robot is an obstacle.
		///	@param		x, y robot coordinates.
		///	@param		d direction to action.
		///	@throws		invalid_argument if tile has no robot.
		///	@return		true if it's an obstacle.
		bool obstacle( unsigned int x, unsigned int y, direction d );

		///	@brief		Checks if a tile is a wall for a type of robot.
		///	@param		x, y tile coordinates.
		///	@param		r_type robot type. 
		///	@return		true if it's a wall.
		bool wall( unsigned int x, unsigned int y, robot_type r_type );

		///	@brief		Checks if the next tile for robot is a wall for it's type.
		///	@param		x, y tile coordinates.
		///	@param		d direction to action.
		///	@throws		invalid_argument if tile has no robot.
		///	@return		true if the next tile is a wall for robot.
		bool wall_next_tile( unsigned int x, unsigned int y, direction d );

		///	@brief		Checks if next tile is a closed door.
		///	@param		x, y robot coordinates.
		///	@param		d direction to action.
		///	@return		true if next tile is a door.
		bool door_next_tile( unsigned int x, unsigned int y, direction d );

		///	@brief		Checks if Robot is on next tile.
		///	@param		x, y robot coordinates. 
		///	@param		d direction to look.
		///	@return		true if robot is on next tile.
		bool robot_next_tile( unsigned int x, unsigned int y, direction d );

		///	@brief		Updates all open doors.
		void update_doors();

		///	@brief		Updates all graves.
		void update_graves();

		///	@brief		Get direction from dijkstra graph.
		///	@param		x, y as coordinates.
		///	@return		direction at dijkstra position.
		direction dijkstra_at( unsigned int x, unsigned int y ) const;

		/// SETTER
		void load_dijkstra_path();

		/// GETTER 
		unsigned int width() const noexcept;
		unsigned int height() const noexcept;
		bool patchbot_corrupted() const noexcept;
	};
}