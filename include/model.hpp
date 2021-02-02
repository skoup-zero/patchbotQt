#pragma once

#include <terrain.hpp>
#include <tga_loader.hpp>
#include <controls.hpp>

#include <qpainter.h>

namespace patchbot
{
	/// @class model for data
	class model
	{
		unsigned int pixel_tga_width_;
		unsigned int pixel_tga_height_;
		unsigned int pixel_terrain_width_;
		unsigned int pixel_terrain_height_;
		bool game_is_on_ = false;

	public:


		load_assets assets_; /* preloaded TGA's */
		terrain terrain_; /* map */
		std::filesystem::path current_path_;

		model();
		model( terrain &&ter, std::filesystem::path current_path );

		/// @brief		renders visible part of map 
		/// @details	loads preloaded TGA images and draws it to a pixmap 
		///				according to map (terrain_)
		/// 
		/// @param		pixmap to draw on
		/// @param		screen_width, screen_height - size of view
		/// @param		scroll_value_x, scroll_value_y - scroll values
		void render_map( QPixmap &pixmap, unsigned int label_x, unsigned int label_y,
			unsigned int space_x, unsigned int space_y );

		bool const check_win();

		int pixel_tga_width() const noexcept;
		int pixel_tga_height() const noexcept;
		int pixel_terrain_width() const noexcept;
		int pixel_terrain_height() const noexcept;

		void set_game_is_on( bool status );
	};
}