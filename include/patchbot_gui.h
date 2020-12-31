#pragma once

#include <QtWidgets/QMainWindow>
#include <ui_patchbot_gui.h>

/// @class model for data
class model
{};


/// @class patchbot_gui as GUI with MVP design pattern
class patchbot_gui: public QMainWindow
{
	Q_OBJECT

private:
	Ui::patchbot_guiClass ui_;
	model model_;	/* model for data */

public:
	patchbot_gui( QWidget *parent = Q_NULLPTR );

public slots:
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
	void on_delete_button_clicked();
};