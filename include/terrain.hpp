#pragma once

#include<entity.hpp>

#include <filesystem>
#include <vector>
#include <memory>
#include <string>

namespace patchbot
{

	/// @class with width, height and tiles representing map
	class terrain
	{
		unsigned int width_;
		unsigned int height_;
		std::vector<tile> tiles_;

		terrain( std::vector<tile> &&tiles, std::vector<std::shared_ptr<robot>> &&robots_,
			unsigned int width, unsigned int height );

	public:

		std::vector<std::shared_ptr<robot>> robots_;

		///	@brief		reads the symbols from a txt file .
		///
		///	@details	creates a vector with instances of Tile and pointers to Robot, 
		///				which will be returned as a map represented by the class terrain.
		/// 
		///	@param		path to a file, that represents a patchbot terrain.
		///
		///	@throws		std::invalid_argument if path isn´t a regular file.
		///	@throws		patchbot_exception if header of txt is invalid
		///	@throws		patchbot_exception if width, height or symbol is incorrect
		///				or goal, patchbot doesn´t exist
		/// 
		///	@return		instance of terrain
		static terrain load_map_from_file( const std::filesystem::path &path );

		///	@brief		returns a tile at given index
		///
		///	@param		index of terrain
		/// 
		///	@throws		std::out_of_range if index is out of range
		/// 
		///	@return		a tile at given index
		const tile &at( unsigned int index ) const;

		///	@brief		returns a tile at given coordinates
		///
		///	@param		coordinates of terrain
		/// 
		///	@throws		std::out_of_range if coordinates is out of range
		/// 
		///	@return		a tile at given coordinates
		tile &at( unsigned int x, unsigned int y );

		unsigned int width() const noexcept;
		unsigned int height() const noexcept;
	};
}