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

void model::render_map( QPixmap &pixmap, unsigned int label_x, unsigned int label_y,
	unsigned int space_x, unsigned int space_y )
{
	/* render entire map if it fits in Label */
	unsigned int width = ( pixel_terrain_width_ < label_x )
		? pixel_terrain_width_ : label_x;
	unsigned int height = ( pixel_terrain_height_ < label_y )
		? pixel_terrain_height_ : label_y;

	QPainter painter;
	tile_type t_type;
	robot_type r_type;

	unsigned int padding_x = space_x / pixel_tga_width_, padding_y = space_y / pixel_tga_height_,
		last_x = terrain_.width() - 1, last_y = terrain_.height() - 1;

	painter.begin( &pixmap );
	for( int y = 0; y <= height / ( pixel_tga_height_ - 1 ) && y <= terrain_.height(); y++ )
	{
		for( int x = 0; x <= width / ( pixel_tga_width_ - 1 ) && x <= terrain_.width(); x++ )
		{
			try
			{	/* Render Background */
				if( x + padding_x >= terrain_.width() && y + padding_y >= terrain_.height() )
				{
					t_type = terrain_.at( last_x, last_y ).type();

				} else if( x + padding_x >= terrain_.width() )
				{
					t_type = terrain_.at( last_x, y + padding_y ).type();

				} else if( y + padding_y >= terrain_.height() )
				{
					t_type = terrain_.at( x + padding_x, last_y ).type();

				} else
				{
					t_type = terrain_.at( x + padding_x, y + padding_y ).type();
				}
				painter.drawPixmap( x * pixel_tga_width_ - space_x % 32,
					y * pixel_tga_height_ - space_y % 32, assets_.terrain_img.at( t_type ) );

				/* Render Robots and save them in a vector */
				if( x + padding_x >= terrain_.width() && y + padding_y >= terrain_.height() )
				{
					if( terrain_.at( last_x, last_y ).occupant_ )
					{
						r_type = terrain_.at( last_x, last_y ).occupant_->robot_type_;
						terrain_.at( last_x, last_y ).occupant_->x_ = x;
						terrain_.at( last_x, last_y ).occupant_->y_ = y;
						robots_.push_back( terrain_.at( last_x, last_y ).occupant_ );

						painter.drawPixmap( x * pixel_tga_width_ - space_x % 32,
							y * pixel_tga_height_ - space_y % 32, assets_.robot_img.at( r_type ) );
					}
				} else if( x + padding_x >= terrain_.width() )
				{
					if( terrain_.at( last_x, y + padding_y ).occupant_ )
					{
						r_type = terrain_.at( last_x, y + padding_y ).occupant_->robot_type_;
						terrain_.at( last_x, y + padding_y ).occupant_->x_ = x;
						terrain_.at( last_x, y + padding_y ).occupant_->y_ = y;
						robots_.push_back( terrain_.at( last_x, y + padding_y ).occupant_ );

						painter.drawPixmap( x * pixel_tga_width_ - space_x % 32,
							y * pixel_tga_height_ - space_y % 32, assets_.robot_img.at( r_type ) );
					}
				} else if( y + padding_y >= terrain_.height() )
				{
					if( terrain_.at( x + padding_x, last_y ).occupant_ )
					{
						r_type = terrain_.at( x + padding_x, last_y ).occupant_->robot_type_;
						terrain_.at( x + padding_x, last_y ).occupant_->x_ = x;
						terrain_.at( x + padding_x, last_y ).occupant_->y_ = y;
						robots_.push_back( terrain_.at( x + padding_x, last_y ).occupant_ );

						painter.drawPixmap( x * pixel_tga_width_ - space_x % 32,
							y * pixel_tga_height_ - space_y % 32, assets_.robot_img.at( r_type ) );
					}
				} else
				{
					if( terrain_.at( x + padding_x, y + padding_y ).occupant_ )
					{
						r_type = terrain_.at( x + padding_x, y + padding_y ).occupant_->robot_type_;
						terrain_.at( x + padding_x, y + padding_y ).occupant_->x_ = x;
						terrain_.at( x + padding_x, y + padding_y ).occupant_->y_ = y;
						robots_.push_back( terrain_.at( x + padding_x, y + padding_y ).occupant_ );

						painter.drawPixmap( x * pixel_tga_width_ - space_x % 32,
							y * pixel_tga_height_ - space_y % 32, assets_.robot_img.at( r_type ) );
					}
				}
			} catch( const std::exception &exc )
			{
				std::cout << "Error: " << exc.what() << std::endl;
				return;
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


/*********************************~ PRESENTER ~***********************************/
patchbot_gui::patchbot_gui( QWidget *parent )
	: QMainWindow( parent )
{
	model model_;
	try
	{
		model_ = model();
	} catch( const std::exception &exc )
	{
		std::cout << "Error: " << exc.what() << std::endl;
		return;
	}

	ui_.setupUi( this );

	ui_.map_label->setText( "Aktuelle Kolonie: everything" );

	pixmap_ = QPixmap( ui_.map_placeholder_label->size() );
	ui_.map_placeholder_label->setMaximumSize(
		model_.pixel_terrain_width(), model_.pixel_terrain_height() );

	ui_.map_scroll_area->setMaximumSize(
		model_.pixel_terrain_width(), model_.pixel_terrain_height() );

	/* same on all maps */
	ui_.sequenz_line_edit->setReadOnly( true );
	ui_.repeat_dropdown->addItems( { "1x", "2x", "3x", "4x", "5x", "6x", "7x", "8x", "9x", "10x" } );
	ui_.center_button->setEnabled( false );
}


void patchbot_gui::refresh_window()
{
	unsigned int width = ( ui_.map_scroll_area->width() < model_.pixel_terrain_width() )
		? ui_.map_scroll_area->width() : model_.pixel_terrain_width();
	unsigned int height = ( ui_.map_scroll_area->height() < model_.pixel_terrain_height() )
		? ui_.map_scroll_area->height() : model_.pixel_terrain_height();

	ui_.map_placeholder_label->setFixedWidth( width );
	ui_.map_placeholder_label->setFixedHeight( height );

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
		ui_.map_label->setText( "Aktuelle Kolonie: " + path.baseName() );
		ui_.map_scrollbar_h->setValue( 0 );
		ui_.map_scrollbar_v->setValue( 0 );

		std::filesystem::path casted_path = change_map_path.toUtf8().constData();

		try
		{
			auto temp = ( terrain::load_map_from_file( casted_path ) );
			model_ = model( std::move( temp ) );
		} catch( std::exception &exc )
		{
			std::cout << "Error: " << exc.what() << std::endl;
			return;
		}
		refresh_window();
	}
}


/* MISSION BUTTONS */
void patchbot_gui::on_mission_start_button_clicked()
{
	QMessageBox::about( this, "START_BUTTON", "start button is clicked" );
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


/* SCROLLBARS */
void patchbot_gui::on_map_scrollbar_h_valueChanged( int change )
{
	model_.render_map( pixmap_,
		ui_.map_scroll_area->width(), ui_.map_scroll_area->height(),
		ui_.map_scrollbar_h->value(), ui_.map_scrollbar_v->value() );

	ui_.map_placeholder_label->setPixmap( pixmap_ );
}

void patchbot_gui::on_map_scrollbar_v_valueChanged( int change )
{
	model_.render_map( pixmap_,
		ui_.map_scroll_area->width(), ui_.map_scroll_area->height(),
		ui_.map_scrollbar_h->value(), ui_.map_scrollbar_v->value() );

	ui_.map_placeholder_label->setPixmap( pixmap_ );
}