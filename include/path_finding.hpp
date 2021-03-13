#pragma once

#include <controls.hpp>

#include <queue>

namespace patchbot
{
	struct compare_nodes
	{
		bool operator()( std::tuple<unsigned int, unsigned int, unsigned int> const &t1,
			std::tuple<unsigned int, unsigned int, unsigned int> const &t2 ) const;
	};

	class dijkstra
	{
	public:
		static void calculate_path(terrain &terrain);
	};
}

//struct compare_nodes
//{
//	bool operator()( std::tuple<unsigned int, unsigned int, unsigned int> const &t1,
//		std::tuple<unsigned int, unsigned int, unsigned int> const &t2 ) const;
//};