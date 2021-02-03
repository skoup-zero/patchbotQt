#pragma once

#include <controls.hpp>
#include <QPixmap>

namespace patchbot
{
	/* to match the struct size with tga header size */
#pragma pack(push, 1)

	/// @struct header of image	
	struct image_header
	{
		std::uint8_t	id_length;
		std::uint8_t	color_map_type;
		std::uint8_t	image_type;
		std::uint16_t	color_map_start;
		std::uint16_t	color_map_length;
		std::uint8_t	color_map_depth;
		std::uint16_t	x_origin;
		std::uint16_t	y_origin;
		std::uint16_t	image_width;
		std::uint16_t	image_heigth;
		std::uint8_t	bits_per_pixel;
		std::uint8_t	image_descriptor;
	};
#pragma pack(pop)


	/// @class image with header and pixels representing tga file
	class image
	{
		const image_header header_;
		const std::vector<unsigned char> pixels_;

		image( image_header &&header, std::vector<unsigned char> &&pixels );

	public:

		/// @brief		Reads data from a tga file.
		/// @details	Reads header first and saves it in struct, image information
		///				are saved in a vector BGRA which represents a pixel.
		/// @param		path to file.
		/// @throws		invalid_argument if file isn´t found.
		/// @throws		runtime_error if file couldn´t be opened.
		/// @throws		patchbot_exception if header is not valid.
		/// @return		instance of tga_loader.
		static image load_tga_from_file( const std::filesystem::path &path );

		/// @brief		Tests if system is little endian.
		///	@details	Assigning 1 to an interger variable and checking the value
		///				of the first bit stored in RAM. 
		///				If 1 then the first bit is the least significant bit.
		///				If 0 then the first bit is the most significant bit.
		/// @return		1 (true) if system is little endian.
		static const bool is_system_little_endian();

		/// @brief		Swaps two bytes.
		/// @param		2 byte word.
		static void swap_bytes( std::uint16_t &word );

		/// @brief		Converts image into pixmap.
		///	@return		Converted pixmap.
		QPixmap qpixmap_converter() const;

		/// @brief		Function to acces a specific pixel at given index.
		/// @param		x, y as coordiantes.
		/// @param		width, heigth as image_size.
		/// @return		index of pixel.
		/// @throws		out_of_range for invalid coordinates.
		static unsigned int pixel_index( unsigned int x, unsigned int y,
			unsigned int width, unsigned int heigth );

		image_header header() const noexcept;
		std::vector<unsigned char> pixels() const noexcept;
	};

	/// @class to preload tga files
	class load_assets
	{
	public:
		load_assets();
		std::unordered_map<tile_type, QPixmap> terrain_img;
		std::unordered_map<robot_type, QPixmap> robot_img;
		std::unordered_map<direction, QPixmap> arrow_img;
	};
}