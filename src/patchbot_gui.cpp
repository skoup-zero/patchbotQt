#include <patchbot_gui.h>
#include <qmessagebox.h>

using namespace patchbot;


patchbot_gui::patchbot_gui( QWidget *parent )
	: QMainWindow( parent )
	, model_()
	, controls_( &model_.terrain_ )
	, timer_( QTimer() )
{
	ui_.setupUi( this );

	/* Display */
	ui_.map_scroll_area->resize( 324, 321 );
	ui_.map_placeholder_label->resize( 324, 321 );
	pixmap_ = QPixmap( 324, 321 );
	ui_.map_label->setText( "Aktuelle Kolonie: everything" );

	/* program buttons */
	ui_.sequenz_line_edit->setReadOnly( true );
	ui_.repeat_dropdown->addItems( { "1x", "2x", "3x", "4x", "5x", "6x", "7x", "8x",
		"9x", "bis Hindernis" } );
	ui_.sequenz_scrollbar_h->setMaximum( 0 );

	/* control buttons */
	ui_.mission_step_button->setEnabled( false );
	ui_.mission_cancel_button->setEnabled( false );
	ui_.mission_auto_button->setEnabled( false );
	ui_.mission_stop_button->setEnabled( false );

	connect( &timer_, &QTimer::timeout, this, &patchbot_gui::on_mission_step_button_clicked );
}

void patchbot_gui::refresh_window()
{
	/* adjust Map */
	unsigned int width = ( ui_.map_scroll_area->width() < model_.pixel_terrain_width() )
		? ui_.map_scroll_area->width() : model_.pixel_terrain_width();
	unsigned int height = ( ui_.map_scroll_area->height() < model_.pixel_terrain_height() )
		? ui_.map_scroll_area->height() : model_.pixel_terrain_height();

	ui_.map_placeholder_label->resize( width, height );
	pixmap_ = pixmap_.scaled( ui_.map_placeholder_label->size(), Qt::IgnoreAspectRatio );

	ui_.map_scrollbar_h->setMaximum( model_.pixel_terrain_width() - width );
	ui_.map_scrollbar_v->setMaximum( model_.pixel_terrain_height() - height );

	model_.render_map( pixmap_, width, height,
		ui_.map_scrollbar_h->value(), ui_.map_scrollbar_v->value() );
	ui_.map_placeholder_label->setPixmap( pixmap_ );

	/* adjust Program line edit */
	ui_.sequenz_line_edit->end( false );
	ui_.sequenz_scrollbar_h->setRange( 0, ui_.sequenz_line_edit->cursorPosition() / 2 );
	ui_.sequenz_scrollbar_h->setValue( ui_.sequenz_scrollbar_h->minimum() );
	ui_.sequenz_line_edit->setCursorPosition( 0 );
}

void patchbot_gui::resizeEvent( QResizeEvent *event )
{
	refresh_window();
}

void patchbot_gui::activate_instruction_buttons( bool activate )
{
	ui_.arrow_up_button->setEnabled( activate );
	ui_.arrow_down_button->setEnabled( activate );
	ui_.arrow_left_button->setEnabled( activate );
	ui_.arrow_right_button->setEnabled( activate );
	ui_.center_button->setEnabled( activate );
	ui_.sequenz_line_edit->setEnabled( activate );
	ui_.delete_button->setEnabled( activate );
	ui_.repeat_dropdown->setEnabled( activate );
	ui_.sequenz_scrollbar_h->setEnabled( activate );
}


/////////////////////////////////////// PUBLIC SLOTS ///////////////////////////////////////

/* CHANGE MAP  */
void patchbot_gui::on_change_colonie_button_clicked()
{
	QString change_map_path = QFileDialog::getOpenFileName(
		this,
		"Please Select a Map",
		R"(x64/Debug/koloniekarten)",
		"txt files (*.txt)" );

	if( !change_map_path.isNull() )
	{
		QFileInfo path( change_map_path );
		std::filesystem::path casted_path = change_map_path.toUtf8().constData();

		try
		{
			auto &temp = ( terrain::load_map_from_file( casted_path ) );
			model_ = model( std::move( temp ), casted_path );

		} catch( std::exception &exc )
		{
			std::cout << "Error: " << exc.what() << std::endl;
			QMessageBox::information( this, "selected File is Broken!", "Please select another File" );
			return;
		}
		ui_.map_label->setText( "Aktuelle Kolonie: " + path.baseName() );
		ui_.map_scrollbar_h->setValue( 0 );
		ui_.map_scrollbar_v->setValue( 0 );
		pixmap_ = QPixmap( ui_.map_placeholder_label->size() );

		on_mission_cancel_button_clicked();
		refresh_window();
	}
}


/* MISSION BUTTONS */
void patchbot_gui::on_mission_start_button_clicked()
{
	ui_.change_colonie_button->setEnabled( false );

	ui_.mission_start_button->setEnabled( false );
	ui_.mission_stop_button->setEnabled( false );
	ui_.mission_step_button->setEnabled( true );
	ui_.mission_cancel_button->setEnabled( true );
	ui_.mission_auto_button->setEnabled( true );

	activate_instruction_buttons( false );

	model_.set_game_is_on( true );
	controls_ = controls( ui_.sequenz_line_edit->text(), &model_.terrain_ );

	refresh_window();
}

void patchbot_gui::on_mission_cancel_button_clicked()
{
	ui_.change_colonie_button->setEnabled( true );

	ui_.mission_step_button->setEnabled( false );
	ui_.mission_auto_button->setEnabled( false );
	ui_.mission_stop_button->setEnabled( false );
	ui_.mission_cancel_button->setEnabled( false );
	ui_.mission_start_button->setEnabled( true );

	ui_.sequenz_line_edit->setText( "" );
	activate_instruction_buttons( true );
	model_.set_game_is_on( false );

	/* reload map when canceled */
	timer_.stop();
	auto temp = ( terrain::load_map_from_file( model_.current_path_ ) );
	model_ = model( std::move( temp ), model_.current_path_ );

	refresh_window();
}

void patchbot_gui::on_mission_step_button_clicked()
{
	auto full_command = ui_.sequenz_line_edit->text();

	if( model_.check_win() )
	{
		QMessageBox::about( this, "!!! WIN !!!", "you found the server" );
		on_mission_cancel_button_clicked();

	} else if( !model_.terrain_.robots_[0]->alive() ) /* move loose to end*/
	{
		QMessageBox::about( this, "!!! LOSE !!!", "patchbot died" );
		on_mission_cancel_button_clicked();

	} else if( full_command.size() <= 0 )
	{
		QMessageBox::about( this, "!!! LOSE !!!", "you didn't found the server" );
		on_mission_cancel_button_clicked();

	} else
	{
		controls_.update_world();

		if( !controls_.until_wall() ) /* skip if until wall is active */
		{
			if( full_command.at( 1 ).digitValue() <= 1 || full_command.at( 1 ) == 'X' )
				full_command.remove( 0, 2 );

			else
			{
				auto frequency = full_command.at( 1 ).digitValue() - 1;
				full_command.replace( 1, 1, QString::fromStdString( std::to_string( frequency ) ) );
			}
		}
	}

	ui_.sequenz_line_edit->setText( full_command );
	refresh_window();
}

void patchbot_gui::on_mission_auto_button_clicked()
{
	ui_.mission_step_button->setEnabled( false );
	ui_.mission_auto_button->setEnabled( false );
	ui_.mission_stop_button->setEnabled( true );

	timer_.start( 1000 );
}

void patchbot_gui::on_mission_stop_button_clicked()
{
	ui_.mission_step_button->setEnabled( true );
	ui_.mission_auto_button->setEnabled( true );

	timer_.stop();
}


/* PROGRAMM BUTTONS */

void patchbot_gui::on_arrow_up_button_clicked()
{
	auto frequency = ui_.repeat_dropdown->currentIndex() + 1;
	ui_.sequenz_line_edit->end( false );

	if( frequency == 10 )
	{
		ui_.sequenz_line_edit->insert( "OX" );
	} else
	{
		ui_.sequenz_line_edit->insert( "O" );
		ui_.sequenz_line_edit->insert( QString::number( frequency ) );
	}

	if( ui_.sequenz_line_edit->text().size() > 18 )
	{
		ui_.sequenz_line_edit->end( false );
		ui_.sequenz_scrollbar_h->setRange( 0, ui_.sequenz_line_edit->cursorPosition() / 2 );
		ui_.sequenz_scrollbar_h->setValue( ui_.sequenz_scrollbar_h->maximum() );
	}
}

void patchbot_gui::on_arrow_down_button_clicked()
{
	auto frequency = ui_.repeat_dropdown->currentIndex() + 1;
	ui_.sequenz_line_edit->end( false );

	if( frequency == 10 )
	{
		ui_.sequenz_line_edit->insert( "UX" );
	} else
	{
		ui_.sequenz_line_edit->insert( "U" );
		ui_.sequenz_line_edit->insert( QString::number( frequency ) );
	}

	if( ui_.sequenz_line_edit->text().size() > 18 )
	{
		ui_.sequenz_line_edit->end( false );
		ui_.sequenz_scrollbar_h->setRange( 0, ui_.sequenz_line_edit->cursorPosition() / 2 );
		ui_.sequenz_scrollbar_h->setValue( ui_.sequenz_scrollbar_h->maximum() );
	}
}

void patchbot_gui::on_arrow_left_button_clicked()
{
	auto frequency = ui_.repeat_dropdown->currentIndex() + 1;
	ui_.sequenz_line_edit->end( false );

	if( frequency == 10 )
	{
		ui_.sequenz_line_edit->insert( "LX" );
	} else
	{
		ui_.sequenz_line_edit->insert( "L" );
		ui_.sequenz_line_edit->insert( QString::number( frequency ) );
	}

	if( ui_.sequenz_line_edit->text().size() > 18 )
	{
		ui_.sequenz_line_edit->end( false );
		ui_.sequenz_scrollbar_h->setRange( 0, ui_.sequenz_line_edit->cursorPosition() / 2 );
		ui_.sequenz_scrollbar_h->setValue( ui_.sequenz_scrollbar_h->maximum() );
	}
}

void patchbot_gui::on_arrow_right_button_clicked()
{
	auto frequency = ui_.repeat_dropdown->currentIndex() + 1;
	ui_.sequenz_line_edit->end( false );

	if( frequency == 10 )
	{
		ui_.sequenz_line_edit->insert( "RX" );
	} else
	{
		ui_.sequenz_line_edit->insert( "R" );
		ui_.sequenz_line_edit->insert( QString::number( frequency ) );
	}

	if( ui_.sequenz_line_edit->text().size() > 18 )
	{
		ui_.sequenz_line_edit->end( false );
		ui_.sequenz_scrollbar_h->setRange( 0, ui_.sequenz_line_edit->cursorPosition() / 2 );
		ui_.sequenz_scrollbar_h->setValue( ui_.sequenz_scrollbar_h->maximum() );
	}
}

void patchbot_gui::on_center_button_clicked()
{
	auto frequency = ui_.repeat_dropdown->currentIndex() + 1;
	ui_.sequenz_line_edit->end( false );

	if( frequency == 10 )
	{
		QMessageBox::information( this, "Are you sure?", "patchbot will never move" );
	} else
	{
		ui_.sequenz_line_edit->insert( "W" );
		ui_.sequenz_line_edit->insert( QString::number( frequency ) );
	}

	if( ui_.sequenz_line_edit->text().size() > 18 )
	{
		ui_.sequenz_line_edit->end( false );
		ui_.sequenz_scrollbar_h->setRange( 0, ui_.sequenz_line_edit->cursorPosition() / 2 );
		ui_.sequenz_scrollbar_h->setValue( ui_.sequenz_scrollbar_h->maximum() );
	}
}

void patchbot_gui::on_delete_button_clicked()
{
	auto text = ui_.sequenz_line_edit->displayText();
	ui_.sequenz_line_edit->end( false );

	if( text.size() == 0 )
	{
		QMessageBox::information( this, "Delete error", "no command to delete" );
		return;
	}
	text.chop( 2 );
	ui_.sequenz_line_edit->setText( text );
}


/* SCROLLBARS */
void patchbot_gui::on_map_scrollbar_h_valueChanged( int change )
{
	model_.render_map( pixmap_,
		ui_.map_placeholder_label->width(), ui_.map_placeholder_label->height(),
		change, ui_.map_scrollbar_v->value() );

	ui_.map_placeholder_label->setPixmap( pixmap_ );
}

void patchbot_gui::on_map_scrollbar_v_valueChanged( int change )
{
	model_.render_map( pixmap_,
		ui_.map_placeholder_label->width(), ui_.map_placeholder_label->height(),
		ui_.map_scrollbar_h->value(), change );
	ui_.map_placeholder_label->setPixmap( pixmap_ );
}

void patchbot_gui::on_sequenz_scrollbar_h_valueChanged( int change )
{
	ui_.sequenz_line_edit->setCursorPosition( change * 2 );
}