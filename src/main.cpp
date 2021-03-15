#include <patchbot_gui.h>
#include <QtWidgets/QApplication>

#include <terrain.hpp>
#include <tga_loader.hpp>

/// @brief	takes a terrain and writes it into a txt file
///
/// @param	map is an instance of terrain
void write_map_to_file( const patchbot::terrain &map )
{
	unsigned int height = map.height();
	unsigned int width = map.width();
	unsigned int counter = 0;

	std::ofstream output_file;
	output_file.open( "Test.txt" );
	output_file << width << std::endl << height << std::endl;

	for( int i = 0; i < height * width; i++ )
	{
		if( counter >= width )
		{
			output_file << std::endl;
			counter = 0;
		}
		counter++;

		if( map.at( i ).occupant_ )
		{
			auto temp = map.at( i ).occupant_;

			for( auto iter = patchbot::robot_map.begin();
				iter != patchbot::robot_map.end(); ++iter )
			{
				if( iter->second == temp->r_type_ )
					output_file << iter->first;
			}
		}
		else
		{
			for( auto iter = patchbot::tile_map.begin();
				iter != patchbot::tile_map.end(); ++iter )
			{
				if( iter->second == map.at( i ).type() )
					output_file << iter->first;
			}
		}
	}
	output_file.close();
}

/// @brief	takes a tga and writes it into  tga file
///
/// @param	tga is an instance of image
void write_tga_to_file( const patchbot::image &tga )
{
	std::ofstream output( "Test.tga", std::ios::binary | std::ios::out );

	const auto &header = tga.header();
	const auto &pixels = tga.pixels();

	output.write( reinterpret_cast<const char *>( &header ), 18 );

	for( int i = 0; i < pixels.size(); i++ )
		output.write( reinterpret_cast<const char *> ( &pixels[i] ), sizeof( pixels[i] ) );

	output.close();
}

int main( int argc, char *argv[] )
{
	QApplication a( argc, argv );
	try
	{
		patchbot::patchbot_gui w;
		w.show();

		return a.exec();
	}
	catch( const std::exception &exc )
	{
		std::cout << "Error: " << exc.what() << std::endl;
		return EXIT_FAILURE;
	}
}

/*

Testat 5
[Funktionalität] Warten vor der Tür ->done
[Schlichtheit] interne Befehls-Repräsentation ->done
[Schlichtheit] inline if else, um bool Variable zu schreiben -> done
[const] bei Methoden -> done
[Referenzen] Verweise nach Möglichkeit als Referenz speichern -> done


VERBESSERUNG:
- roboter warten vor der tür.

- interne befehlsrepräsentation: extra konstruktor gelöscht und befehle werden
	nun im controller seperat vom Qstring gebaut

- anfängerfehler behoben ( if/else für bool variable )

- methoden sind nun const

- verweis auf terrain jetzt als referenz statt pointer


 FRAGEN:

	testat 7:
		dijkstra algorithmus nur neu aufrufen, wenn patchbot sich bewegt hat
			-> aber digger kann felswände zerstören und somit die kosten der pfade ändern
				ohne das patchbot sich bewegt.
*/