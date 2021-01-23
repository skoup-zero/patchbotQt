#include <patchbot_gui.h>
#include <qmessagebox.h>

using namespace patchbot;

/*********************************~ MODEL ~***********************************/
model::model()
	: assets_{ load_assets() }
	, terrain_{ terrain::load_map_from_file( R"(assets\txt\koloniekarten\everything.txt)" ) }
{
	pixel_tga_width_ = assets_.terrain_img.at( tile_type::patchbot_start ).width();
	pixel_tga_height_ = assets_.terrain_img.at( tile_type::patchbot_start ).height();
	pixel_terrain_width_ = terrain_.width() * pixel_tga_width_;
	pixel_terrain_height_ = terrain_.height() * pixel_tga_height_;
}

model::model( terrain &&ter )
	: assets_{ load_assets() }
	, terrain_{ ter }
{
	pixel_tga_width_ = assets_.terrain_img.at( tile_type::patchbot_start ).width();
	pixel_tga_height_ = assets_.terrain_img.at( tile_type::patchbot_start ).height();
	pixel_terrain_width_ = terrain_.width() * pixel_tga_width_;
	pixel_terrain_height_ = terrain_.height() * pixel_tga_height_;
}

void model::render_map( QPixmap &pixmap, unsigned int screen_width, unsigned int screen_height,
	unsigned int scroll_value_x, unsigned int scroll_value_y )
{
	/* render entire map if it fits in Label */
	unsigned int width = ( pixel_terrain_width_ < screen_width )
		? pixel_terrain_width_ : screen_width;
	unsigned int height = ( pixel_terrain_height_ < screen_height )
		? pixel_terrain_height_ : screen_height;

	int left_border = std::floor( scroll_value_x / pixel_tga_width_ );
	int top_border = std::floor( scroll_value_y / pixel_tga_height_ );

	int right_border = std::ceil( ( width + scroll_value_x ) / pixel_tga_width_ ) + 1;
	int bottom_border = std::ceil( ( height + scroll_value_y ) / pixel_tga_height_ ) + 1;

	QPainter painter;
	painter.begin( &pixmap );

	for( int y = top_border; y <= bottom_border && y < terrain_.height(); y++ )
	{
		for( int x = left_border; x <= right_border && x < terrain_.width(); x++ )
		{
			/* draw Background */
			const auto &tile = terrain_.at( x, y );
			const auto &img = assets_.terrain_img.at( tile.type() );

			painter.drawPixmap( ( x - left_border ) * pixel_tga_width_ - scroll_value_x % 32,
				( y - top_border ) * pixel_tga_height_ - scroll_value_y % 32, img );

			/* draw Robots */
			if( tile.occupant_ )
			{
				const auto &robot = tile.occupant_;
				const auto &img = assets_.robot_img.at( robot->robot_type_ );

				if( robot->robot_type_ == robot_type::patchbot )
				{
					if( game_is_on_ )
					{
						painter.drawPixmap( ( x - left_border ) * pixel_tga_width_ - scroll_value_x % 32,
							( y - top_border ) * pixel_tga_height_ - scroll_value_y % 32, img );
						robots_.push_back( robot );
					}
				} else
				{
					painter.drawPixmap( ( x - left_border ) * pixel_tga_width_ - scroll_value_x % 32,
						( y - top_border ) * pixel_tga_height_ - scroll_value_y % 32, img );
					robots_.push_back( robot );
				}
			}
		}
	}
	painter.end();
}



// GETTER
int model::pixel_tga_width() const noexcept
{
	return pixel_tga_width_;
}

int model::pixel_tga_height() const noexcept
{
	return pixel_tga_height_;
}

int model::pixel_terrain_width() const noexcept
{
	return pixel_terrain_width_;

}

int model::pixel_terrain_height() const noexcept
{
	return pixel_terrain_height_;
}

void model::set_game_is_on( bool status )
{
	game_is_on_ = status;
}


/*********************************~ PRESENTER ~***********************************/
patchbot_gui::patchbot_gui( QWidget *parent )
	: QMainWindow( parent )
	, model_()
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

	/* control buttons */
	ui_.mission_step_button->setEnabled( false );
	ui_.mission_cancel_button->setEnabled( false );
	ui_.mission_auto_button->setEnabled( false );
}


void patchbot_gui::refresh_window()
{
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
}

void patchbot_gui::resizeEvent( QResizeEvent *event )
{
	refresh_window();
}

/* PUBLIC SLOTS:*/

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
			auto temp = ( terrain::load_map_from_file( casted_path ) );
			model_ = model( std::move( temp ) );

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

		refresh_window();
	}
}


/* MISSION BUTTONS */
void patchbot_gui::on_mission_start_button_clicked()
{
	ui_.arrow_up_button->setEnabled( false );
	ui_.arrow_down_button->setEnabled( false );
	ui_.arrow_left_button->setEnabled( false );
	ui_.arrow_right_button->setEnabled( false );
	ui_.center_button->setEnabled( false );
	ui_.sequenz_line_edit->setEnabled( false );
	ui_.sequenz_scrollbar_h->setEnabled( false );
	ui_.change_colonie_button->setEnabled( false );
	ui_.mission_start_button->setEnabled( false );
	ui_.delete_button->setEnabled( false );
	ui_.repeat_dropdown->setEnabled( false );

	ui_.mission_step_button->setEnabled( true );
	ui_.mission_cancel_button->setEnabled( true );
	ui_.mission_auto_button->setEnabled( true );

	model_.set_game_is_on( true );
	refresh_window();
}

void patchbot_gui::on_mission_cancel_button_clicked() //TODO
{
	QMessageBox::about( this, "CANCEL_BUTTON", "cancel button is clicked" );
}

void patchbot_gui::on_mission_step_button_clicked() //TODO
{
	auto full_command_q = ui_.sequenz_line_edit->text().remove( 0, 2 );
	std::string one_command = full_command_q.toUtf8().constData();
	ui_.sequenz_line_edit->setText( full_command_q );
}

void patchbot_gui::on_mission_auto_button_clicked() //TODO
{
	QMessageBox::about( this, "AUTO_BUTTON", "auto button is clicked" );
}

void patchbot_gui::on_mission_stop_button_clicked() //TODO
{
	QMessageBox::about( this, "STOP_BUTTON", "stop button is clicked" );
}


/* PROGRAMM BUTTONS */

void patchbot_gui::on_arrow_up_button_clicked()
{
	auto frequency = ui_.repeat_dropdown->currentIndex() + 1;

	if( frequency == 10 )
	{
		ui_.sequenz_line_edit->insert( "OX" );
	} else
	{
		ui_.sequenz_line_edit->insert( "O" );
		ui_.sequenz_line_edit->insert( QString::number( frequency ) );
	}
}

void patchbot_gui::on_arrow_down_button_clicked()
{
	auto frequency = ui_.repeat_dropdown->currentIndex() + 1;

	if( frequency == 10 )
	{
		ui_.sequenz_line_edit->insert( "UX" );
	} else
	{
		ui_.sequenz_line_edit->insert( "U" );
		ui_.sequenz_line_edit->insert( QString::number( frequency ) );
	}
}

void patchbot_gui::on_arrow_left_button_clicked()
{
	auto frequency = ui_.repeat_dropdown->currentIndex() + 1;

	if( frequency == 10 )
	{
		ui_.sequenz_line_edit->insert( "LX" );
	} else
	{
		ui_.sequenz_line_edit->insert( "L" );
		ui_.sequenz_line_edit->insert( QString::number( frequency ) );
	}
}

void patchbot_gui::on_arrow_right_button_clicked()
{
	auto frequency = ui_.repeat_dropdown->currentIndex() + 1;

	if( frequency == 10 )
	{
		ui_.sequenz_line_edit->insert( "RX" );
	} else
	{
		ui_.sequenz_line_edit->insert( "R" );
		ui_.sequenz_line_edit->insert( QString::number( frequency ) );
	}
}

void patchbot_gui::on_center_button_clicked()
{
	auto frequency = ui_.repeat_dropdown->currentIndex() + 1;

	if( frequency == 10 )
	{
		ui_.sequenz_line_edit->insert( "WX" );
	} else
	{
		ui_.sequenz_line_edit->insert( "W" );
		ui_.sequenz_line_edit->insert( QString::number( frequency ) );
	}
}

void patchbot_gui::on_delete_button_clicked()
{
	auto text = ui_.sequenz_line_edit->displayText();
	if( text.size() == 0 )
	{
		QMessageBox::information( this, "Delete error",
			"no command to delete" );
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