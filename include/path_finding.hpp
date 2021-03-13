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

	class dijkstra
	{
	public:

		///	@brief		Calculates paths to Patchbot using Dijkstra Algorithm.
		///	@param		terrain as graph.
		///	@return		Shortest Paths.
		static std::vector<std::tuple< unsigned int, direction>>
		calculate_paths( terrain &terrain );
	};
}