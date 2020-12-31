#include <patchbot_gui.h>
#include <qmessagebox.h>

patchbot_gui::patchbot_gui( QWidget *parent )
    : QMainWindow( parent )
{
    ui_.setupUi(this);
    /* model values will be added here */

    ui_.sequenz_line_edit->setReadOnly( true );
    ui_.repeat_dropdown->addItems( { "1x", "2x", "3x", "4x", "5x", "6x", "7x", "8x", "9x", "10x" } );
    ui_.center_button->setEnabled( false );
}


void patchbot_gui::on_change_colonie_button_clicked()
{
    QMessageBox::about( this, "CHANGE_BUTTON", "change-colonie button is clicked" );
}


/* MISSION BUTTONS */
void patchbot_gui::on_mission_start_button_clicked()
{
    QMessageBox::about(this, "START_BUTTON", "start button is clicked");
}

void patchbot_gui::on_mission_cancel_button_clicked()
{
    QMessageBox::about( this, "CANCEL_BUTTON", "cancel button is clicked" );
}

void patchbot_gui::on_mission_step_button_clicked()
{
    QMessageBox::about( this, "STEP_BUTTON", "step button is clicked" );
}

void patchbot_gui::on_mission_auto_button_clicked()
{
    QMessageBox::about( this, "AUTO_BUTTON", "auto button is clicked" );
}

void patchbot_gui::on_mission_stop_button_clicked()
{
    QMessageBox::about( this, "STOP_BUTTON", "stop button is clicked" );
}


/* PROGRAMM BUTTONS */

void patchbot_gui::on_arrow_up_button_clicked()
{
    QMessageBox::about( this, "UP_BUTTON", "up-arrow button is clicked" );
}

void patchbot_gui::on_arrow_down_button_clicked()
{
    QMessageBox::about( this, "DOWN_BUTTON", "down-arrow button is clicked" );
}

void patchbot_gui::on_arrow_left_button_clicked()
{
    QMessageBox::about( this, "LEFT_BUTTON", "left-arrow button is clicked" );
}

void patchbot_gui::on_arrow_right_button_clicked()
{
    QMessageBox::about( this, "RIGHT_BUTTON", "right-arrow button is clicked" );
}

void patchbot_gui::on_delete_button_clicked()
{
    QMessageBox::about( this, "DELETE_BUTTON", "delete button is clicked" );
}