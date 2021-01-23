#include <controls.hpp>


//void model::move_robot( const char direction, const char frequency, const unsigned int id )
//{
//	switch( id )
//	{
//		case 0: move_robot_patchbot( direction, frequency ); break;
//		default: move_robot_patchbot( direction, frequency );
//	}
//}
//
//void model::move_robot_patchbot( const char direction, const char frequency )
//{
//	unsigned int x = robots_[0]->x_;
//	unsigned int y = robots_[0]->y_;
//	unsigned int frq = static_cast<int>( frequency );
//	auto current_loc = terrain_.at( x, y );
//
//	switch( direction )
//	{
//		case 'O':
//			terrain_.at( x, y + frq ).occupant_ = current_loc.occupant_;
//			terrain_.at( x, y ).occupant_ = nullptr;
//			break;
//		case 'U': break;
//		case 'L': break;
//		case 'R': break;
//		default:
//	}
//}