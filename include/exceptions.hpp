#pragma once
#include <exception>

/// @brief	enums for exceptions
enum class patchbot_enum_exception
{
	map_format_exception,
	image_format_exception
};

/// @brief	throws exceptions whenever an error occurs
class patchbot_exception: public std::exception
{
	const patchbot_enum_exception exception_;

public:

	explicit patchbot_exception( const patchbot_enum_exception patchbot_exception );
	virtual const char *what() const noexcept;
};