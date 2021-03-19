#pragma once

#include <terrain.hpp>
#include <queue>

namespace patchbot
{
	///	@struct node represents a node.
	struct node
	{
		unsigned int distance_, x_, y_;
		node( unsigned int distance, unsigned int x, unsigned int y );
	};

	///	@struct to compare distances of nodes in priority queue
	///		to implement it as a Min-Heap.
	struct compare_nodes
	{
		bool operator()( const node &d1, const node &d2 ) const;
	};

	///	@class to find a path to patchbot.
	class dijkstra
	{
	public:
		///	@brief		Calculates paths to Patchbot using Dijkstra Algorithm.
		///	@details	Each node expands at most 4 nodes.
		///				Shortest distance of each valid node is saved in a vector with
		///				their distances and direction.
		///	@param		terrain as graph.
		///	@return		vector as tree that contains shortest paths to Patchbot.
		static std::vector<std::pair< unsigned int, direction>>
			calculate_paths( terrain &terrain );
	};
}