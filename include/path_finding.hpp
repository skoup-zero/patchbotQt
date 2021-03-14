#pragma once

#include <controls.hpp>

#include <queue>

namespace patchbot
{
	///	@struct to compare costs of nodes in priority queue
	///		to implement it as a Min-Heap.
	struct compare_nodes
	{
		bool operator()(
			std::tuple<unsigned int, unsigned int, unsigned int> const &t1,
			std::tuple<unsigned int, unsigned int, unsigned int> const &t2 ) const;
	};

	///	@class to find a path to patchbot.
	class dijkstra
	{
	public:
		///	@brief		Calculates paths to Patchbot using Dijkstra Algorithm.
		///	@details	Each node expands at most 4 nodes.
		///				Lowest cost of each valid node is saved in a vector with
		///				their cost and direction. 
		///	@param		terrain as graph.
		///	@return		vector as tree that contains shortest paths to Patchbot.
		static std::vector<std::tuple< unsigned int, direction>>
			calculate_paths( terrain &terrain );
	};
}