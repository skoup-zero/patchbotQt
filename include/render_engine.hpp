#pragma once

#include <terrain.hpp>
#include <tga_loader.hpp>
#include <path_finding.hpp>

#include <QPainter>

namespace patchbot
{
	/// @class render_engine for data
	class render_engine
	{
		unsigned int pixel_tga_width_;
		unsigned int pixel_tga_height_;
		unsigned int pixel_terrain_width_;
		unsigned int pixel_terrain_height_;
		bool game_is_on_ = false;
		bool arrows_is_on = false;

	public:
		/* preloaded TGA's */
		load_assets assets_;
		terrain terrain_;
		std::filesystem::path current_path_;

		/* shortest path to patchbot */
		std::vector<std::pair<unsigned int, direction>> dijkstra_path_tree_;

		render_engine();
		render_engine( terrain &&ter, std::filesystem::path current_path );

		///	@brief		Renders visible part of map.
		///	@details	Loads preloaded TGA images and draws it to a pixmap
		///				according to map (terrain_).
		///	@param		pixmap to draw on.
		///	@param		screen_width, screen_height - size of view.
		///	@param		scroll_value_x, scroll_value_y - scroll values.
		void render_map( QPixmap &pixmap,
			unsigned int screen_width, unsigned int screen_height,
			unsigned int scroll_value_x, unsigned int scroll_value_y );

		/// SETTER
		void set_game_is_on( bool status );
		void load_dijkstra_path();

		/// GETTER 
		int pixel_tga_width() const noexcept;
		int pixel_tga_height() const noexcept;
		int pixel_terrain_width() const noexcept;
		int pixel_terrain_height() const noexcept;
	};
}