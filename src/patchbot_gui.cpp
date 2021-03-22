#include <patchbot_gui.h>

using namespace patchbot;

patchbot_gui::patchbot_gui( QWidget *parent )
	: QMainWindow( parent )
	, render_engine_()
	, controls_( render_engine_.terrain_ )
	, timer_( QTimer() )
{
	ui_.setupUi( this );

	/* Display */
	ui_.map_scroll_area->resize( 370, 320 );
	ui_.map_placeholder_label->resize( 370, 320 );
	pixmap_ = QPixmap( 370, 320 );
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
	/* size is entire map if it fits in label */
	const auto width = ( ui_.map_scroll_area->width() < render_engine_.pixel_terrain_width() )
		? ui_.map_scroll_area->width() : render_engine_.pixel_terrain_width();
	const auto height = ( ui_.map_scroll_area->height() < render_engine_.pixel_terrain_height() )
		? ui_.map_scroll_area->height() : render_engine_.pixel_terrain_height();

	ui_.map_placeholder_label->resize( width, height );
	pixmap_ = pixmap_.scaled( ui_.map_placeholder_label->size(), Qt::IgnoreAspectRatio );

	ui_.map_scrollbar_h->setMaximum( render_engine_.pixel_terrain_width() - width );
	ui_.map_scrollbar_h->setPageStep( ui_.map_scroll_area->width() );
	ui_.map_scrollbar_v->setMaximum( render_engine_.pixel_terrain_height() - height );
	ui_.map_scrollbar_v->setPageStep( ui_.map_scroll_area->height() );

	render_engine_.render_map( pixmap_, width, height,
		ui_.map_scrollbar_h->value(), ui_.map_scrollbar_v->value() );
	ui_.map_placeholder_label->setPixmap( pixmap_ );

	adjust_sequence_scrollbar();

	/* (OPTIONAL) Jumps Cursor to beginning so user can always see current command*/
	ui_.sequenz_line_edit->setCursorPosition( 0 );
	ui_.sequenz_scrollbar_h->setValue( 0 );
}

void patchbot_gui::resizeEvent( QResizeEvent *event )
{
	refresh_window();
}

void patchbot_gui::activate_program_buttons( const bool activate ) const
{
	ui_.arrow_up_button->setEnabled( activate );
	ui_.arrow_down_button->setEnabled( activate );
	ui_.arrow_left_button->setEnabled( activate );
	ui_.arrow_right_button->setEnabled( activate );
	ui_.center_button->setEnabled( activate );
	ui_.sequenz_line_edit->setEnabled( activate );
	ui_.delete_button->setEnabled( activate );
	ui_.repeat_dropdown->setEnabled( activate );

	/* (OPTIONAL) Uncomment this if User is NOT allowed to scroll during gameplay */
	//ui_.sequenz_scrollbar_h->setEnabled( activate );
}

void patchbot_gui::adjust_sequence_scrollbar() const
{
	ui_.sequenz_line_edit->end( false );
	if( ui_.sequenz_line_edit->text().size() > 18 )
	{
		ui_.sequenz_line_edit->end( false );
		ui_.sequenz_scrollbar_h->setRange( 0, ui_.sequenz_line_edit->cursorPosition() / 2 );
		ui_.sequenz_scrollbar_h->setValue( ui_.sequenz_scrollbar_h->maximum() );
		ui_.sequenz_scrollbar_h->setPageStep( 50 );
	}
	else
		ui_.sequenz_scrollbar_h->setMaximum( 0 );
}

/////////////////////////////////////// PUBLIC SLOTS ///////////////////////////////////////

/* CHANGE MAP  */
void patchbot_gui::on_change_colonie_button_clicked()
{
	const QString change_map_path = QFileDialog::getOpenFileName(
		this,
		"Please Select a Map",
		R"(assets\txt\koloniekarten)",
		"txt files (*.txt)" );

	if( !change_map_path.isNull() )
	{
		const QFileInfo path( change_map_path );
		std::filesystem::path casted_path = change_map_path.toUtf8().constData();
		try
		{
			auto &temp = ( terrain::load_map_from_file( casted_path ) );
			render_engine_ = render_engine( std::move( temp ), casted_path );
		}
		catch( std::exception &exc )
		{
			/* catch if a broken file is selected and return */
			std::cout << "Error: " << exc.what() << std::endl;
			QMessageBox::information( this, "selected File is Broken!",
				"Please select another File" );
			return;
		}
		ui_.map_label->setText( "Aktuelle Kolonie: " + path.baseName() );
		ui_.map_scrollbar_h->setValue( 0 );
		ui_.map_scrollbar_v->setValue( 0 );
		ui_.repeat_dropdown->setCurrentIndex( 0 );
		pixmap_ = QPixmap( ui_.map_placeholder_label->size() );

		on_mission_cancel_button_clicked();
		refresh_window();
	}
}


//////////////////////////////////////////////////
///////////////* MISSION BUTTONS *////////////////
//////////////////////////////////////////////////

void patchbot_gui::on_mission_start_button_clicked()
{
	activate_program_buttons( false );
	ui_.change_colonie_button->setEnabled( false );

	ui_.mission_stop_button->setEnabled( false );
	ui_.mission_start_button->setEnabled( false );
	ui_.mission_step_button->setEnabled( true );
	ui_.mission_auto_button->setEnabled( true );
	ui_.mission_cancel_button->setEnabled( true );

	render_engine_.set_game_is_on( true );
	controls_.init_enemies();

	refresh_window();
}

void patchbot_gui::on_mission_cancel_button_clicked()
{
	activate_program_buttons( true );
	ui_.change_colonie_button->setEnabled( true );

	ui_.mission_step_button->setEnabled( false );
	ui_.mission_auto_button->setEnabled( false );
	ui_.mission_stop_button->setEnabled( false );
	ui_.mission_cancel_button->setEnabled( false );
	ui_.mission_start_button->setEnabled( true );

	ui_.sequenz_line_edit->setText( "" );
	render_engine_.set_game_is_on( false );
	timer_.stop();

	/* reload map when canceled */
	auto temp = ( terrain::load_map_from_file( render_engine_.current_path_ ) );
	render_engine_ = render_engine( std::move( temp ), render_engine_.current_path_ );
	controls_ = controls( render_engine_.terrain_ );

	refresh_window();
}

void patchbot_gui::on_mission_step_button_clicked()
{
	auto full_command = ui_.sequenz_line_edit->text();

	if( controls_.check_win() )
	{
		QMessageBox::about( this, "!!! WIN !!!", "you found the server!" );
		on_mission_cancel_button_clicked();
		return;
	}

	if( controls_.patchbot_dead() )
	{
		QMessageBox::about( this, "!!! LOSE !!!", "Patchbot died!" );
		on_mission_cancel_button_clicked();
		return;
	}

	if( controls_.instructions_empty() )
	{
		QMessageBox::about( this, "!!! LOSE !!!", "you didn't find the server!" );
		on_mission_cancel_button_clicked();
		return;
	}
	
	controls_.update_world();

	/* Skip instruction edit if until wall is active or patchbot is obstructed */
	if( !controls_.until_wall() && !controls_.patchbot_obstructed() )
	{
		if( full_command.at( 1 ).digitValue() <= 1 || full_command.at( 1 ) == 'X' )
			full_command.remove( 0, 2 );

		else
		{
			const auto frequency = full_command.at( 1 ).digitValue() - 1;
			full_command.replace( 1, 1, QString::fromStdString( std::to_string( frequency ) ) );
		}
	}

	/* uncomment next line to show arrows */
	//render_engine_.arrows_on();
	
	ui_.sequenz_line_edit->setText( full_command );
	refresh_window();

	if( controls_.patchbot_corrupted() )
	{
		QMessageBox::about( this, "!!! LOSE !!!", "Patchbot has been corrupted!" );
		on_mission_cancel_button_clicked();
	}
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
	ui_.mission_stop_button->setEnabled( false );

	timer_.stop();
}


//////////////////////////////////////////////////
///////////////* PROGRAM BUTTONS *////////////////
//////////////////////////////////////////////////

void patchbot_gui::on_arrow_up_button_clicked()
{
	const auto frequency = ui_.repeat_dropdown->currentIndex() + 1;
	ui_.sequenz_line_edit->end( false );

	if( frequency == 10 )
	{
		ui_.sequenz_line_edit->insert( "OX" );
		controls_.add_instruction( direction::up, 0 );
	}
	else
	{
		ui_.sequenz_line_edit->insert( "O" );
		ui_.sequenz_line_edit->insert( QString::number( frequency ) );
		controls_.add_instruction( direction::up, frequency );
	}

	adjust_sequence_scrollbar();
}

void patchbot_gui::on_arrow_down_button_clicked()
{
	const auto frequency = ui_.repeat_dropdown->currentIndex() + 1;
	ui_.sequenz_line_edit->end( false );

	if( frequency == 10 )
	{
		ui_.sequenz_line_edit->insert( "UX" );
		controls_.add_instruction( direction::down, 0 );
	}
	else
	{
		ui_.sequenz_line_edit->insert( "U" );
		ui_.sequenz_line_edit->insert( QString::number( frequency ) );
		controls_.add_instruction( direction::down, frequency );
	}

	adjust_sequence_scrollbar();
}

void patchbot_gui::on_arrow_left_button_clicked()
{
	const auto frequency = ui_.repeat_dropdown->currentIndex() + 1;
	ui_.sequenz_line_edit->end( false );

	if( frequency == 10 )
	{
		ui_.sequenz_line_edit->insert( "LX" );
		controls_.add_instruction( direction::left, 0 );
	}
	else
	{
		ui_.sequenz_line_edit->insert( "L" );
		ui_.sequenz_line_edit->insert( QString::number( frequency ) );
		controls_.add_instruction( direction::left, frequency );
	}

	adjust_sequence_scrollbar();
}

void patchbot_gui::on_arrow_right_button_clicked()
{
	const auto frequency = ui_.repeat_dropdown->currentIndex() + 1;
	ui_.sequenz_line_edit->end( false );

	if( frequency == 10 )
	{
		ui_.sequenz_line_edit->insert( "RX" );
		controls_.add_instruction( direction::right, 0 );
	}
	else
	{
		ui_.sequenz_line_edit->insert( "R" );
		ui_.sequenz_line_edit->insert( QString::number( frequency ) );
		controls_.add_instruction( direction::right, frequency );
	}

	adjust_sequence_scrollbar();
}

void patchbot_gui::on_center_button_clicked()
{
	const auto frequency = ui_.repeat_dropdown->currentIndex() + 1;

	if( frequency == 10 )
	{
		QMessageBox::information( this, "Are you sure?", "Patchbot would never action" );
		return;
	}

	ui_.sequenz_line_edit->end( false );
	ui_.sequenz_line_edit->insert( "W" );
	ui_.sequenz_line_edit->insert( QString::number( frequency ) );

	adjust_sequence_scrollbar();

	controls_.add_instruction( direction::wait, frequency );
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
	adjust_sequence_scrollbar();

	controls_.remove_instruction();
}


/////////////////////////////////////////////
///////////////* SCROLLBARS *////////////////
/////////////////////////////////////////////
void patchbot_gui::on_map_scrollbar_h_valueChanged( int change )
{
	render_engine_.render_map( pixmap_,
		ui_.map_placeholder_label->width(), ui_.map_placeholder_label->height(),
		change, ui_.map_scrollbar_v->value() );

	ui_.map_placeholder_label->setPixmap( pixmap_ );
}

void patchbot_gui::on_map_scrollbar_v_valueChanged( int change )
{
	render_engine_.render_map( pixmap_,
		ui_.map_placeholder_label->width(), ui_.map_placeholder_label->height(),
		ui_.map_scrollbar_h->value(), change );

	ui_.map_placeholder_label->setPixmap( pixmap_ );
}

void patchbot_gui::on_sequenz_scrollbar_h_valueChanged( int change ) const
{
	ui_.sequenz_line_edit->setCursorPosition( change * 2 );
}