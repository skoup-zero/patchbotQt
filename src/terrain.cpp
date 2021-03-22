#include <terrain.hpp>
#include <path_finding.hpp>

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


void terrain::move_robot( const unsigned int x, const unsigned int y, const direction d )
{
	if( !at( x, y ).occupant_ )
		throw std::invalid_argument( "ERROR: no robot at tile" );

	auto &robot = at( x, y ).occupant_;

	switch( d )
	{
		case direction::up:
			if( y > 0 )
			{
				at( x, y - 1 ).occupant_.swap( robot );
				at( x, y - 1 ).occupant_->y_--; break;
			}

		case direction::right:
			if( x < width_ - 1 )
			{
				at( x + 1, y ).occupant_.swap( robot );
				at( x + 1, y ).occupant_->x_++; break;
			}

		case direction::down:
			if( y < height_ - 1 )
			{
				at( x, y + 1 ).occupant_.swap( robot );
				at( x, y + 1 ).occupant_->y_++; break;
			}

		case direction::left:
			if( x > 0 )
			{
				at( x - 1, y ).occupant_.swap( robot );
				at( x - 1, y ).occupant_->x_--; break;
			}
	}
}

void terrain::push_robot( const unsigned int x, const unsigned int y, direction d )
{
	std::shared_ptr<robot> target_r;

	switch( d )
	{
		case direction::up:
			target_r = at( x, y - 1 ).occupant_; break;

		case direction::right:
			target_r = at( x + 1, y ).occupant_; break;

		case direction::down:
			target_r = at( x, y + 1 ).occupant_; break;

		case direction::left:
			target_r = at( x - 1, y ).occupant_; break;

		default: return;
	}

	if( !target_r )
		return;

	/* don't push if roboter is blocked */
	if( wall_next_tile( target_r->x_, target_r->y_, d ) || robot_next_tile( target_r->x_, target_r->y_, d ) )
		return;

	move_robot( target_r->x_, target_r->y_, d );

	/* kill roboter if it lands on dangerous tile */
	if( dangerous_tile( target_r->x_, target_r->y_ ) )
		kill_robot( target_r->x_, target_r->y_ );
}

void terrain::kill_robot( const unsigned int x, const unsigned int y )
{
	tile &t = at( x, y );
	t.grave_set_timer();
	t.occupant_ = nullptr;
	graves_.push_back( &t );
}

void terrain::corrupt_patchbot( const unsigned x, const unsigned y, const direction d )
{
	/* can't corrupt patchbot on his save space */
	if( at( patchbot_->x_, patchbot_->y_ ).type() == tile_type::secret_path )
		return;
	
	switch( d )
	{
		case direction::up:
			if( x == patchbot_->x_ && y - 1 == patchbot_->y_ )
				patchbot_corrupted_ = true;
			break;

		case direction::right:
			if( x + 1 == patchbot_->x_ && y == patchbot_->y_ )
				patchbot_corrupted_ = true;
			break;

		case direction::down:
			if( x == patchbot_->x_ && y + 1 == patchbot_->y_ )
				patchbot_corrupted_ = true;
			break;

		case direction::left:
			if( x - 1 == patchbot_->x_ && y == patchbot_->y_ )
				patchbot_corrupted_ = true;
			break;
	}
}


bool terrain::dangerous_tile( const unsigned int  x, const unsigned int y )
{
	if( !at( x, y ).occupant_ )
		throw std::invalid_argument( "ERROR: no robot at tile" );

	const tile &tile = at( x, y );

	if( tile.type() == tile_type::precipice )
		return true;

	return tile.type() == tile_type::water && tile.occupant_->type() != robot_type::swimmer;
}

bool terrain::obstacle( const unsigned int x, const unsigned int y, const direction d )
{
	if( !at( x, y ).occupant_ )
		throw std::invalid_argument( "ERROR: no robot at tile" );

	tile &tile = at( x, y );
	auto &robot = tile.occupant_;

	/* obstructed robots can move again */
	if( robot->obstructed() )
	{
		robot->update_obstructed();
		return false;
	}

	const bool has_wheels =
		robot->type() == robot_type::patchbot ||
		robot->type() == robot_type::pusher ||
		robot->type() == robot_type::digger ||
		robot->type() == robot_type::swimmer;

	/* obsacle for robots with wheels */
	if( tile.type() == tile_type::alien_weed && has_wheels )
	{
		robot->update_obstructed();
		return true;
	}

	/* obstacle for robots with legs */
	if( tile.type() == tile_type::gravel && !has_wheels )
	{
		robot->update_obstructed();
		return true;
	}

	/* wait if next tile is closed door */
	return door_next_tile( x, y, d );
}

bool terrain::wall( const unsigned int x, const unsigned int y, const robot_type r_type )
{
	/* map boundaries as walls */
	if( x >= width_ || y >= height_ )
		return true;

	const tile &tile = at( x, y );

	/* environment as walls */

	/* concret wall and server walls for every robot */
	if( tile.type() == tile_type::concrete_wall || tile.type() == tile_type::server )
		return true;
	/* breakable walls */
	if( tile.type() == tile_type::rock_wall && r_type != robot_type::digger )
		return true;
	/* secret door (wall) for KI */
	if( tile.type() == tile_type::secret_path && r_type != robot_type::patchbot )
		return true;
	/* automatic door (wall) for Patchbot */
	if( tile.type() == tile_type::automatic_door &&
		( r_type == robot_type::patchbot || r_type == robot_type::bugger ) )
		return true;
	/* manual door and dangerous tiles (wall) for bugger */
	if( r_type == robot_type::bugger && ( tile.type() == tile_type::manual_door ||
		tile.type() == tile_type::water || tile.type() == tile_type::precipice ) )
		return true;


	/* follower, hunter, sniffer interpret other enemies as walls */
	if( tile.occupant_ && tile.occupant_->type() != robot_type::patchbot &&
		( r_type == robot_type::follower || r_type == robot_type::hunter || r_type == robot_type::sniffer ) )
		return true;

	return false;
}

bool terrain::wall_next_tile( const unsigned int x, const unsigned int y, const direction d )
{
	if( !at( x, y ).occupant_ )
		throw std::invalid_argument( "ERROR: no robot at tile" );

	switch( d )
	{
		case direction::up:
			return wall( x, y - 1, at( x, y ).occupant_->type() );

		case direction::down:
			return wall( x, y + 1, at( x, y ).occupant_->type() );

		case direction::left:
			return wall( x - 1, y, at( x, y ).occupant_->type() );

		case direction::right:
			return wall( x + 1, y, at( x, y ).occupant_->type() );

		default: return false;
	}
}

bool terrain::door_next_tile( const unsigned int x, const unsigned int y, const direction d )
{
	if( !at( x, y ).occupant_ )
		throw std::invalid_argument( "ERROR: no robot at tile" );

	tile *tile;

	switch( d )
	{
		case direction::up:
			if( y > 0 )
				tile = &at( x, y - 1 );
			else
				return false;
			break;

		case direction::right:
			if( x < width_ - 1 )
				tile = &at( x + 1, y );
			else
				return false;
			break;

		case direction::down:
			if( y < height_ - 1 )
				tile = &at( x, y + 1 );
			else
				return false;
			break;

		case direction::left:
			if( x > 0 )
				tile = &at( x - 1, y );
			else
				return false;
			break;

		default: return false;
	}

	if( !tile->door_ )
		return false;

	if( tile->door_is_open() )
		return false;

	auto &robot = at( x, y ).occupant_;

	/* Patchbot can't use automatic doors */
	if( tile->door_is_automatic() && robot->type() == robot_type::patchbot )
		return false;

	robot->update_obstructed();
	tile->door_set_timer();		/* open door */
	open_doors_.push_back( tile );	/* save opened door */
	return true;
}

bool terrain::robot_next_tile( const unsigned x, const  unsigned y, const direction d )
{
	if( x >= width_ || y >= height_ )
		return false;

	//TODO review this
	switch( d )
	{
		case direction::up:
			return ( y > 0 ) ? !( at( x, y - 1 ).occupant_ == nullptr ) : false;

		case direction::right:
			return ( x < width_ - 1 ) ? !( at( x + 1, y ).occupant_ == nullptr ) : false;

		case direction::down:
			return ( y < height_ - 1 ) ? !( at( x, y + 1 ).occupant_ == nullptr ) : false;

		case direction::left:
			return ( x > 0 ) ? !( at( x - 1, y ).occupant_ == nullptr ) : false;

		default: return false;
	}
}

void terrain::update_doors()
{
	for( auto i = 0; i < open_doors_.size(); i++ )
	{
		open_doors_[i]->door_decrement_timer();

		if( !open_doors_[i]->door_is_open() )
			open_doors_.erase( open_doors_.begin() + i );
	}
}

void terrain::update_graves()
{
	for( auto i = 0; i < graves_.size(); i++ )
	{
		graves_[i]->grave_decrement_timer();

		if( !graves_[i]->is_grave() )
			graves_.erase( graves_.begin() + i );
	}
}

direction terrain::dijkstra_at( const unsigned int x, const unsigned int y ) const
{
	if( dijkstra_path_tree_.empty() )
		return direction::undefined;

	if( x >= width_ || y >= height_ )
		return direction::undefined;

	return dijkstra_path_tree_[width_ * y + x].second;
}

/// SETTER
void terrain::load_dijkstra_path()
{
	dijkstra_path_tree_ = dijkstra::calculate_paths( *this );
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

bool terrain::patchbot_corrupted() const noexcept
{
	return patchbot_corrupted_;
}