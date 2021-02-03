#include <model.hpp>

using namespace patchbot;

model::model()
	: assets_{ load_assets() }
	, terrain_{ terrain::load_map_from_file( R"(assets\txt\koloniekarten\everything.txt)" ) }
	, current_path_{ R"(assets\txt\koloniekarten\everything.txt)" }
{
	pixel_tga_width_ = assets_.terrain_img.at( tile_type::patchbot_start ).width();
	pixel_tga_height_ = assets_.terrain_img.at( tile_type::patchbot_start ).height();
	pixel_terrain_width_ = terrain_.width() * pixel_tga_width_;
	pixel_terrain_height_ = terrain_.height() * pixel_tga_height_;
}

model::model( terrain &&ter, std::filesystem::path current_path )
	: assets_{ load_assets() }
	, terrain_{ ter }
	, current_path_{ current_path }
{
	pixel_tga_width_ = assets_.terrain_img.at( tile_type::patchbot_start ).width();
	pixel_tga_height_ = assets_.terrain_img.at( tile_type::patchbot_start ).height();
	pixel_terrain_width_ = terrain_.width() * pixel_tga_width_;
	pixel_terrain_height_ = terrain_.height() * pixel_tga_height_;
}

void model::render_map( QPixmap &pixmap, unsigned int screen_width, unsigned int screen_height,
	unsigned int scroll_value_x, unsigned int scroll_value_y )
{
	/* render entire map if it fits in Label */
	unsigned int width = ( pixel_terrain_width_ < screen_width )
		? pixel_terrain_width_ : screen_width;
	unsigned int height = ( pixel_terrain_height_ < screen_height )
		? pixel_terrain_height_ : screen_height;

	int left_border = std::floor( scroll_value_x / pixel_tga_width_ );
	int top_border = std::floor( scroll_value_y / pixel_tga_height_ );

	int right_border = std::ceil( ( width + scroll_value_x ) / pixel_tga_width_ ) + 1;
	int bottom_border = std::ceil( ( height + scroll_value_y ) / pixel_tga_height_ ) + 1;

	QPainter painter;
	painter.begin( &pixmap );

	for( int y = top_border; y <= bottom_border && y < terrain_.height(); y++ )
		for( int x = left_border; x <= right_border && x < terrain_.width(); x++ )
		{
			/* draw Background */
			const auto &tile = terrain_.at( x, y );
			const auto &img = assets_.terrain_img.at( tile.type() );

			if( tile.door_ && tile.door_is_open() )
			{
				/* draw open door */
				const auto &open_door_img =
					( tile.door_is_automatic() ) ? assets_.terrain_img.at( tile_type::automatic_door_open )
					: assets_.terrain_img.at( tile_type::manual_door_open );

				painter.drawPixmap( ( x - left_border ) * pixel_tga_width_ - scroll_value_x % 32,
					( y - top_border ) * pixel_tga_height_ - scroll_value_y % 32, open_door_img );
			} else
				painter.drawPixmap( ( x - left_border ) * pixel_tga_width_ - scroll_value_x % 32,
					( y - top_border ) * pixel_tga_height_ - scroll_value_y % 32, img );

			/* draw Robots */
			if( tile.occupant_ && tile.occupant_->alive() )
			{
				const auto &robot = tile.occupant_;
				const auto &img = assets_.robot_img.at( robot->r_type_ );

				if( robot->r_type_ == robot_type::patchbot )
				{
					if( game_is_on_ )
					{
						painter.drawPixmap( ( x - left_border ) * pixel_tga_width_ - scroll_value_x % 32,
							( y - top_border ) * pixel_tga_height_ - scroll_value_y % 32, img );
					}
				} else
				{
					painter.drawPixmap( ( x - left_border ) * pixel_tga_width_ - scroll_value_x % 32,
						( y - top_border ) * pixel_tga_height_ - scroll_value_y % 32, img );
				}
			} else if( tile.occupant_ && !tile.occupant_->alive() )
			{
				painter.drawPixmap( ( x - left_border ) * pixel_tga_width_ - scroll_value_x % 32,
					( y - top_border ) * pixel_tga_height_ - scroll_value_y % 32,
					assets_.robot_img.at( robot_type::dead ) );
			}
		}
	painter.end();
}

bool const model::check_win()
{
	const auto &tile = terrain_.at( terrain_.robots_[0]->x_, terrain_.robots_[0]->y_ );
	if( tile.type() == tile_type::server )
		return true;
	else
		return false;
}


// GETTER
int model::pixel_tga_width() const noexcept
{
	return pixel_tga_width_;
}

int model::pixel_tga_height() const noexcept
{
	return pixel_tga_height_;
}

int model::pixel_terrain_width() const noexcept
{
	return pixel_terrain_width_;

}

int model::pixel_terrain_height() const noexcept
{
	return pixel_terrain_height_;
}

void model::set_game_is_on( bool status )
{
	game_is_on_ = status;
}