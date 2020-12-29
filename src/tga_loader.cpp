#include <tga_loader.hpp>
#include <exceptions.hpp>

#include <filesystem>
#include <stdexcept>
#include <vector>

using namespace patchbot;

image::image( image_header &&header, std::vector<unsigned char> &&pixels )
	:header_{ header }
	, pixels_{ pixels }
{}

image image::load_tga_from_file( const std::filesystem::path &path )
{
	if( !std::filesystem::is_regular_file( path ) ) /* exc: irregular file */
		throw std::invalid_argument( "no valid path" );

	std::ifstream input( path, std::ios::binary | std::ios::in );

	if( !input ) /* exc: file not open */
		throw std::runtime_error( "couldn´t open file" );

	input.seekg( 0, input.end );
	int input_size = input.tellg();
	input.seekg( 0, input.beg );

	if( input_size < 18 ) /* exc: corrupted tga file */
		throw patchbot_exception( patchbot_enum_exception::image_format_exception );

	if( !input.good() )	/* exc: errors in input stream */
		throw std::runtime_error( "corrupted tga file" );

	image_header header;

	input.read( reinterpret_cast<char *> ( &header ), 18 );

	/* correcting bytes for big endian systems */
	if( !is_system_little_endian() )
	{
		swap_bytes( header.color_map_start );
		swap_bytes( header.color_map_length );
		swap_bytes( header.x_origin );
		swap_bytes( header.y_origin );
		swap_bytes( header.image_width );
		swap_bytes( header.image_heigth );
	}

	if( static_cast<int>( header.id_length ) ) /* exc: header id included */
		throw patchbot_exception( patchbot_enum_exception::image_format_exception );

	if( static_cast<int>( header.color_map_type ) ) /* exc: color map type included */
		throw patchbot_exception( patchbot_enum_exception::image_format_exception );

	if( static_cast<int>( header.image_type ) != 2 ) /* exc: image type is not RGB or RGBA */
		throw patchbot_exception( patchbot_enum_exception::image_format_exception );

	if( static_cast<int>( header.color_map_start ) ) /* exc: color map start included */
		throw patchbot_exception( patchbot_enum_exception::image_format_exception );

	if( static_cast<int>( header.color_map_length ) ) /* exc: color map length included */
		throw patchbot_exception( patchbot_enum_exception::image_format_exception );

	if( static_cast<int>( header.x_origin ) ) /* exc: x included */
		throw patchbot_exception( patchbot_enum_exception::image_format_exception );

	if( static_cast<int>( header.y_origin ) ) /* exc: y included */
		throw patchbot_exception( patchbot_enum_exception::image_format_exception );

	if( static_cast<int>( header.bits_per_pixel ) != 32 ) /* exc: image type is not RGBA */
		throw patchbot_exception( patchbot_enum_exception::image_format_exception );

	unsigned int img_size = static_cast<int>( header.image_heigth )
		* static_cast<int>( header.image_width ) * 4;

	if( input_size + 18 <= img_size ) /* exc: file size is not big enough */
		throw patchbot_exception( patchbot_enum_exception::image_format_exception );

	std::vector<unsigned char> buffer;
	buffer.resize( img_size );

	for( int i = 0; i < img_size; i++ )
	{
		input.read( reinterpret_cast<char *>( &buffer[i] ), 1 );
	}

	return image( std::move( header ), std::move( buffer ) );
}


const bool image::is_system_little_endian()
{
	const unsigned int endian_test = 1;
	const auto first_byte = reinterpret_cast<const unsigned char *> ( &endian_test );
	return first_byte[0];
}


void image::swap_bytes( std::uint16_t &word )
{
	word = ( word >> 8 ) | ( word << 8 );
}


/// GETTER

unsigned int image::pixel_index( unsigned int x, unsigned int y, unsigned int width, unsigned int heigth )
{
	if( x >= width || y >= heigth )
		throw std::out_of_range{ "coordinates out of range" };

	return ( width * 4 * y ) + ( x * 4 );
}


image_header image::header() const noexcept
{
	return header_;
}


std::vector<unsigned char> patchbot::image::pixels() const noexcept
{
	return pixels_;
}
