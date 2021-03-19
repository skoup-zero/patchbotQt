#include <terrain.hpp>

using namespace patchbot;

terrain::terrain( std::vector<tile> &&tiles, std::vector<std::shared_ptr<robot>> &&robots,
	std::shared_ptr<robot> patchbot, unsigned int width, unsigned int height )
	: tiles_{ tiles }
	, robots_{ robots }
	, patchbot_{ patchbot }
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
	}
	catch( const std::exception &exc )
	{
		throw;
	}

	unsigned int pb_start = 0, pb_goals = 0, counter_height = 0, counter_width = 0;

	std::vector<tile> tiles;
	std::vector<std::shared_ptr<robot>> robots;
	std::shared_ptr<robot> patchbot;

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
					tile temp = tile( tile_type::patchbot_start );
					temp.occupant_ = std::make_shared<robot>( r_it->second );
					temp.occupant_->x_ = counter_width;
					temp.occupant_->y_ = counter_height;
					tiles.push_back( temp );
					patchbot = temp.occupant_;
					pb_start++;
				}
				else
				{
					tile temp = tile( tile_type::enemy_start );
					temp.occupant_ = std::make_shared<robot>( r_it->second );
					temp.occupant_->x_ = counter_width;
					temp.occupant_->y_ = counter_height;
					tiles.push_back( temp );
					robots.push_back( temp.occupant_ );
				}
			}
			else if( !( t_it == tile_map.end() ) ) /* creating tile */
			{
				if( t_it->first == 'P' )
					pb_goals++;

				if( t_it->first == 'd' || t_it->first == 'D' )
				{
					tile temp = tile( t_it->second, true );
					tiles.push_back( temp );
				}
				else
				{
					tile temp = tile( t_it->second );
					tiles.push_back( temp );
				}
			}
			else
			{ /* exc: invalid char */
				throw patchbot_exception{ patchbot_enum_exception::map_format_exception };
			}
			counter_width++;
		}
		counter_width = 0;
		counter_height++;
	}
	if( counter_height != height ) /* exc: height of map is incorrect */
		throw patchbot_exception{ patchbot_enum_exception::map_format_exception };

	if( pb_goals < 1 ) /* exc: no server found */
		throw patchbot_exception{ patchbot_enum_exception::map_format_exception };

	if( pb_start != 1 ) /* exc: less or more then one patchbot */
		throw patchbot_exception{ patchbot_enum_exception::map_format_exception };

	return terrain( std::move( tiles ), std::move( robots ), std::move( patchbot ), width, height );
}

/// SETTER
void terrain::set_dijkstra_path( std::vector<std::pair<unsigned int, direction>> &dijkstra_path_tree )
{
	dijkstra_path_tree_ = dijkstra_path_tree;
}

/// GETTER

const tile &terrain::at( unsigned int index ) const
{
	if( index > height_ * width_ )
	{
		throw std::out_of_range{ "index is out of range" };
	}
	return tiles_.at( index );
}

tile &terrain::at( unsigned int x, unsigned int y )
{
	if( x >= width_ || y >= height_ )
	{
		throw std::out_of_range{ "coordinates out of range" };
	}
	return tiles_.at( width_ * y + x );
}

unsigned int terrain::width() const noexcept
{
	return width_;
}

unsigned int terrain::height() const noexcept
{
	return height_;
}