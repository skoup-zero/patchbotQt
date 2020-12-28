#include <exceptions.hpp>

patchbot_exception::patchbot_exception( const patchbot_enum_exception patchbot_exception )
	:exception_( patchbot_exception )
{}

const char *patchbot_exception::what() const noexcept
{
	switch( exception_ )
	{
		case patchbot_enum_exception::map_format_exception: return "map_format_exception"; 
		case patchbot_enum_exception::image_format_exception: return "image_format_exception";
		defualt:
			return "unknown exception";
	}
}