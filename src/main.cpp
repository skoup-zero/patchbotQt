#include <patchbot_gui.h>
#include <QtWidgets/QApplication>

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

	auto header = tga.header();
	auto pixels = tga.pixels();

	output.write( reinterpret_cast<char *>( &header ), 18 );

	for( int i = 0; i < pixels.size(); i++ )
	{
		output.write( reinterpret_cast<char *> ( &pixels[i] ), sizeof( pixels[i] ) );
	}

	output.close();
}

int main( int argc, char *argv[] )
{

	QApplication a( argc, argv );
	try
	{
		auto test = patchbot::image::load_tga_from_file(R"(C:\Zero_Work\patchbot\assets\tga\grafiken\roboter\patchbot.tga)" );
		write_tga_to_file( test );

	} catch( const std::exception &exc )
	{
		std::cout << "Error: " << exc.what() << std::endl;
		return EXIT_FAILURE;
	}
	patchbot_gui w;
	w.show();
	return a.exec();
}

//int main( int argc, char **argv )
//{
//	if( argc != 2 )
//	{
//		std::cout << "please put a valid path for the map to load"
//			<< R"(e.g.: F:\Work\Patchbot\extra\txt\koloniekarten\map.txt)" << std::endl;
//
//		return EXIT_FAILURE;
//	}
//	try
//	{
//		//patchbot::terrain map = patchbot::terrain::load_map_from_file( argv[1] );
//		//write_map_to_file( map );
//
//		std::cout << argv[1] << std::endl;
//		auto test = patchbot::image::load_tga_from_file( argv[1] );
//		write_tga_to_file( test );
//
//	} catch( const std::exception &exc )
//	{
//		std::cout << "Error: " << exc.what() << std::endl;
//		return EXIT_FAILURE;
//	}
//
//	return EXIT_SUCCESS;
//}