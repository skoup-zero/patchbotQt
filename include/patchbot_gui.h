#pragma once

#include <QtWidgets/QMainWindow>
#include <ui_patchbot_gui.h>
#include <QPainter>
#include <QFileDialog>
#include <QTimer>
#include <QMessageBox>

#include <model.hpp>
#include <terrain.hpp>
#include <tga_loader.hpp>
#include <controls.hpp>

#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>

namespace patchbot
{
	/// @class patchbot_gui as GUI with MVP design pattern
	class patchbot_gui: public QMainWindow
	{
		Q_OBJECT

	private:

		Ui::patchbot_guiClass ui_; /* ui as view */
		model model_;	/* model for data */
		controls controls_; /* controls for interaction */
		QTimer timer_;
		QPixmap pixmap_;
		QString last_instruction_ = ""; /* saving instruction from last game */

	public:

		patchbot_gui( QWidget *parent = Q_NULLPTR );

		/// @brief adjust size of Map and Sequence line
		void refresh_window();
		void resizeEvent( QResizeEvent *event );

		/// @brief (de-)activate all instruction buttons
		void activate_instruction_buttons( bool activate );

	public slots:

		/// @brief change map
		void on_change_colonie_button_clicked();

		/// @brief mission button 
		void on_mission_start_button_clicked();
		void on_mission_cancel_button_clicked();
		void on_mission_step_button_clicked();
		void on_mission_auto_button_clicked();
		void on_mission_stop_button_clicked();

		/// @brief programm button
		void on_arrow_up_button_clicked();
		void on_arrow_down_button_clicked();
		void on_arrow_left_button_clicked();
		void on_arrow_right_button_clicked();
		void on_center_button_clicked();
		void on_delete_button_clicked();

		/// @brief scrollbars 
		void on_map_scrollbar_h_valueChanged( int change );
		void on_map_scrollbar_v_valueChanged( int change );
		void on_sequenz_scrollbar_h_valueChanged( int change );
	};
}