#pragma once

#include <terrain.hpp>
#include <tga_loader.hpp>

#include <QPainter>

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

		/* preloaded TGA's */
		load_assets assets_;
		terrain terrain_;
		std::filesystem::path current_path_;

		model();
		model( terrain &&ter, std::filesystem::path current_path );

		///	@brief		Renders visible part of map.
		///	@details	Loads preloaded TGA images and draws it to a pixmap
		///				according to map (terrain_).
		///	@param		pixmap to draw on.
		///	@param		screen_width, screen_height - size of view.
		///	@param		scroll_value_x, scroll_value_y - scroll values.
		void render_map( QPixmap &pixmap, unsigned int label_x, unsigned int label_y,
			unsigned int space_x, unsigned int space_y );

		///	@brief		Check if player won the game.
		///	@return		true if patchbot is on server.
		bool check_win();

		/// SETTER
		void set_game_is_on( bool status );

		/// GETTER 
		int pixel_tga_width() const noexcept;
		int pixel_tga_height() const noexcept;
		int pixel_terrain_width() const noexcept;
		int pixel_terrain_height() const noexcept;
	};
}