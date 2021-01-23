#pragma once

#include <QtWidgets/QMainWindow>
#include <qpainter.h>
#include <ui_patchbot_gui.h>
#include <qfiledialog.h>

#include <terrain.hpp>
#include <tga_loader.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>

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

		std::vector<std::shared_ptr<robot>> robots_; /* contains all Robots on map */
		load_assets assets_; /* preloaded TGA's */
		terrain terrain_; /* map */

		model();
		model( terrain &&ter );

		/// @brief		renders visible part of map 
		/// @details	loads preloaded TGA images and draws it to a pixmap 
		///				according to map (terrain_)
		/// 
		/// @param		pixmap to draw on
		/// @param		screen_width, screen_height - size of view
		/// @param		scroll_value_x, scroll_value_y - scroll values
		void render_map( QPixmap &pixmap, unsigned int label_x, unsigned int label_y,
			unsigned int space_x, unsigned int space_y );

		int pixel_tga_width() const noexcept;
		int pixel_tga_height() const noexcept;
		int pixel_terrain_width() const noexcept;
		int pixel_terrain_height() const noexcept;

		void set_game_is_on(bool status);
	};


	/// @class patchbot_gui as GUI with MVP design pattern
	class patchbot_gui: public QMainWindow
	{
		Q_OBJECT

	private:

		Ui::patchbot_guiClass ui_; /* view */
		model model_;	/* model for data */

	public:

		QPixmap pixmap_;

		patchbot_gui( QWidget *parent = Q_NULLPTR );

		/// @brief		refresh all widgets sizes
		void refresh_window();
		void resizeEvent( QResizeEvent *event );

	public slots:

		/// @brief change map
		void on_change_colonie_button_clicked();

		/// @brief mission button dummys
		void on_mission_start_button_clicked();
		void on_mission_cancel_button_clicked();
		void on_mission_step_button_clicked();
		void on_mission_auto_button_clicked();
		void on_mission_stop_button_clicked();

		/// @brief programm button dummys
		void on_arrow_up_button_clicked();
		void on_arrow_down_button_clicked();
		void on_arrow_left_button_clicked();
		void on_arrow_right_button_clicked();
		void on_center_button_clicked();
		void on_delete_button_clicked();

		/// @brief scrollbars 
		void on_map_scrollbar_h_valueChanged( int change );
		void on_map_scrollbar_v_valueChanged( int change );
	};
}