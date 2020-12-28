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

	/* creates vector with iteratable unsigned chars */
	std::vector<unsigned char> buffer( std::istreambuf_iterator<char>( input ), {} );

	image_header header;

	input.seekg( 0, input.beg );
	input.read( reinterpret_cast<char *> ( &header ), 18 );
	input.close();

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

	/* deleting Header and Metadata */
	buffer.erase( buffer.begin(), buffer.begin() + 18 );
	buffer.erase( buffer.begin() + img_size, buffer.end() );

	( header.image_width, header.image_heigth, buffer );

	return image( std::move( header ), std::move( buffer ) );
}

unsigned int image::pixel_index( unsigned int x, unsigned int y, unsigned int width, unsigned int heigth )
{
	if( x >= width || y >= heigth )
		throw std::out_of_range{ "coordinates out of range" };

	return ( width * 4 * y ) + ( x * 4 );
}

/// GETTER

image_header image::header() const
{
	return header_;
}

std::vector<unsigned char> patchbot::image::pixels() const
{
	return pixels_;
}
