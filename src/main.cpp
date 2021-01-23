#include <patchbot_gui.h>
#include <QtWidgets/QApplication>
#include <QMessageBox>

#include <entity.hpp>
#include <terrain.hpp>
#include <tga_loader.hpp>

#include <stdexcept>
#include <iostream>
#include <vector>
#include <fstream>

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
				if( iter->second == temp->robot_type_ )
					output_file << iter->first;
			}
		} else
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
	{
		output.write( reinterpret_cast<const char *> ( &pixels[i] ), sizeof( pixels[i] ) );
	}

	output.close();
}

int main( int argc, char *argv[] )
{
	QApplication a( argc, argv );

	try
	{
		//auto map = patchbot::terrain::load_map_from_file(R"(C:\Zero_Work\patchbot\assets\txt\koloniekarten\everything.txt)" );
		//write_map_to_file( map );

		//auto test = patchbot::image::load_tga_from_file( R"(C:\Zero_Work\patchbot\assets\tga\grafiken\roboter\patchbot.tga)" );
		//write_tga_to_file( test );

		patchbot::patchbot_gui w;
		w.show();

		return a.exec();

	} catch( const std::exception &exc )
	{
		std::cout << "Error: " << exc.what() << std::endl;
		return EXIT_FAILURE;
	}
}

/*

Testat 3 + 4
[Ausnahmebehandlung] Ausnahmen, die das Programmende erfordern, in der main fangen -> done
[Schlichtheit] redundanter Code beim Laden der Grafiken,
				effizienterer Code für Konvertierung nach Qimage -> done

[Datenstrukturen] zu rendernden Ausschnitt bestimmen, Anstelle jedes Mal auf Ende der Karte zu prüfen

TODOS:
-relativer Pfad
-stürtzt ab wenn tga nicht findet 
- 

VERBESSERUNG TESTAT 4:
- rendering jetz halb so kurz und ca 2000 mal unkomplizierter.
- qpixmap converter vertauscht nach dem lesen der daten die R mit B und spiegelt es vertikal anstatt
	jeden pixel einzeln zu lesen.
- Ausnahmen, die das Programmende erfordern werden in der Main abgefangen und alle anderen per QDialog 
	dem User mitgeteilt.
*/