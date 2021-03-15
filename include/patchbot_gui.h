#pragma once

#include <QtWidgets/QMainWindow>
#include <ui_patchbot_gui.h>
#include <QPainter>
#include <QFileDialog>
#include <QTimer>
#include <QMessageBox>

#include <model.hpp>

namespace patchbot
{
	/// @class patchbot_gui as GUI with MVP design pattern.
	class patchbot_gui: public QMainWindow
	{
		Q_OBJECT

	private:

		Ui::patchbot_guiClass ui_;
		model model_;
		controls controls_;
		QTimer timer_;
		QPixmap pixmap_;

	public:

		patchbot_gui( QWidget *parent = Q_NULLPTR );

		/// @brief	Adjust size of Map components.
		void refresh_window();

		/// @brief	Calls refresh_windows() everytime window resizes.
		void resizeEvent( QResizeEvent *event );

		/// @brief	(De-)Activates all program buttons.
		void activate_program_buttons( bool activate ) const;

		///	@brief	Adjusts sequence scrollbar for long instructions.
		void adjust_sequence_scrollbar() const;

	public slots:

		/// @brief	Change map and reset game.
		void on_change_colonie_button_clicked();


		/// @brief	Starts the game.
		void on_mission_start_button_clicked();

		/// @brief	Cancels current game and restores map values.
		void on_mission_cancel_button_clicked();

		/// @brief	Makes one time step.
		void on_mission_step_button_clicked();

		/// @brief	Makes one time step every second.
		void on_mission_auto_button_clicked();

		/// @brief	Stops automatic mode and returns to manuel mode.
		void on_mission_stop_button_clicked();


		/// @brief	Program buttons to add or delete commands for patchbot.
		void on_arrow_up_button_clicked();
		void on_arrow_down_button_clicked();
		void on_arrow_left_button_clicked();
		void on_arrow_right_button_clicked();
		void on_center_button_clicked();
		void on_delete_button_clicked();


		/// @brief	Scrollbars for map.
		void on_map_scrollbar_h_valueChanged( int change );
		void on_map_scrollbar_v_valueChanged( int change );

		/// @brief	Scrollbars for sequence line.
		void on_sequenz_scrollbar_h_valueChanged( int change ) const;
	};
}