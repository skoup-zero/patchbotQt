#include <entity.hpp>
#include <terrain.hpp>
#include <exceptions.hpp>

#include <fstream>
#include <string>
#include <stdexcept>
#include <map>
#include <cctype>

using namespace patchbot;

terrain::terrain( std::vector<tile> &&tiles, unsigned int width,
	unsigned int height )
	:tiles_{ tiles }
	, width_{ width }
	, height_{ height }
{}

terrain terrain::load_map_from_file( const std::filesystem::path &path )
{
	unsigned int width, height;
	std::string current_line;

	if( !std::filesystem::is_regular_file( path ) ) /* exc: irregular file */
		throw std::invalid_argument( "no valid path" );

	std::fstream init_map( path );

	try
	{
		std::getline( init_map, current_line );

		if( !( std::all_of( current_line.begin(), current_line.end(), isdigit ) ) /*exc: all strings are integers */
			|| isdigit( current_line[0] == 0 ) ) /* exc: empty char */
		{
			throw patchbot_exception{ patchbot_enum_exception::map_format_exception };
		}

		width = stoi( current_line );
		std::getline( init_map, current_line );

		if( !( std::all_of( current_line.begin(), current_line.end(), ::isdigit ) ) /*exc: all strings are integers */
			|| isdigit( current_line[0] == 0 ) ) /* exc: empty char */
		{
			throw patchbot_exception{ patchbot_enum_exception::map_format_exception };
		}

		height = stoi( current_line );

	} catch( const std::exception &exc )
	{
		throw;
	}

	unsigned int pb_start = 0, pb_goals = 0, counter = 0;

	std::vector<tile> tiles;
	tiles.reserve( width * height );

	/* reads each char of each line and creates Objects asigned to that symbol */
	while( std::getline( init_map, current_line ) )
	{
		if( current_line.length() != width ) /* exc: lenght of line is correct */
			throw patchbot_exception{ patchbot_enum_exception::map_format_exception };

		for( char j : current_line )
		{
			const auto r_it = robot_map.find( j );
			const auto t_it = tile_map.find( j );

			if( !( r_it == robot_map.end() ) ) /* creating robot and a tile it stands on */
			{
				if( r_it->first == 'p' )
				{
					auto temp = tile( tile_type::patchbot_start );
					temp.occupant_ = std::make_shared<robot>( r_it->second );
					tiles.push_back( temp );
					pb_start++;

				} else
				{
					auto temp = tile( tile_type::enemy_start );
					temp.occupant_ = std::make_shared<robot>( r_it->second );
					tiles.push_back( temp );
				}
			} else if( !( t_it == tile_map.end() ) ) /* creating tile */
			{
				if( t_it->first == 'P' )
					pb_goals++;

				auto temp = tile( t_it->second );
				tiles.push_back( temp );

			} else
			{ /* exc: invalid char */
				throw patchbot_exception{ patchbot_enum_exception::map_format_exception };
			}
		}
		counter++;
	}
	if( counter != height ) /* exc: height of map is incorrect */
		throw patchbot_exception{ patchbot_enum_exception::map_format_exception };

	if( pb_goals < 1 ) /* exc: no server found */
		throw patchbot_exception{ patchbot_enum_exception::map_format_exception };

	if( pb_start != 1 ) /* exc: less or more then one patchbot */
		throw patchbot_exception{ patchbot_enum_exception::map_format_exception };

	return terrain( std::move( tiles ), width, height );
}


/// GETTER
unsigned int terrain::width() const
{
	return width_;
}

unsigned int terrain::height() const
{
	return height_;
}

const tile &terrain::at( unsigned int index ) const
{
	if( index > height_ * width_ )
	{
		throw std::out_of_range{ "index is out of range" };
	}
	return tiles_.at( index );
}

const tile &terrain::at( unsigned int x, unsigned int y ) const
{
	if( x >= width_ || y >= height_ )
	{
		throw std::out_of_range{ "coordinates out of range" };
	}
	return tiles_.at( width_ * y + x );
}
