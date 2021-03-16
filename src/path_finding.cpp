#include <path_finding.hpp>

using namespace patchbot;

bool compare_nodes::operator()(
	std::tuple<unsigned int, unsigned int, unsigned int> const &t1,
	std::tuple<unsigned int, unsigned int, unsigned int> const &t2 ) const
{
	return std::get<0>( t1 ) > std::get<0>( t2 );
}

std::vector<std::tuple<unsigned int, direction>> dijkstra::calculate_paths( terrain &terrain )
{
	const unsigned int width = terrain.width();
	const unsigned int height = terrain.height();

	/* priority queue as Min-Heap */
	std::priority_queue<
		std::tuple<unsigned int, unsigned int, unsigned int>,
		std::vector<std::tuple<unsigned int, unsigned int, unsigned int>>,
		compare_nodes> pq;

	/* initialize path tree with max distance and undefined direction */
	std::vector<std::tuple<unsigned int, direction>> path_tree
	( width * height, std::make_tuple( UINT_MAX, direction::undefined ) );

	/* nodes are tuple < distance, x, y > */
	pq.push( std::make_tuple( 0, terrain.patchbot_->x_, terrain.patchbot_->y_ ) );

	/* lamba function expand child nodes */
	auto handle_direction = [&]( const unsigned int x, const unsigned int y,
		const unsigned int distance, const direction d )
	{
		const bool replace = rand() % 2;
		const unsigned int index = width * y + x;

		if( std::get<0>( path_tree[index] ) > distance )
		{
			std::get<0>( path_tree[index] ) = distance;
			std::get<1>( path_tree[index] ) = d;
			pq.push( std::make_tuple( distance, x, y ) );
		}
		else if( replace && std::get<0>( path_tree[index] ) == distance )
			std::get<1>( path_tree[index] ) = d;
	};

	while( !pq.empty() )
	{
		const unsigned int distance = std::get<0>( pq.top() );
		const unsigned int x = std::get<1>( pq.top() );
		const unsigned int y = std::get<2>( pq.top() );
		pq.pop();

		/* UP */
		if( y > 0 && terrain.at( x, y - 1 ).node_cost() > 0 )
		{
			const unsigned int current = terrain.at( x, y - 1 ).node_cost() + distance;
			handle_direction( x, y - 1, current, direction::down );
		}

		/* RIGHT */
		if( x < width - 1 && terrain.at( x + 1, y ).node_cost() > 0 )
		{
			const unsigned int current = terrain.at( x + 1, y ).node_cost() + distance;
			handle_direction( x + 1, y, current, direction::left );
		}

		/* DOWN */
		if( y < height - 1 && terrain.at( x, y + 1 ).node_cost() > 0 )
		{
			const unsigned int current = terrain.at( x, y + 1 ).node_cost() + distance;
			handle_direction( x, y + 1, current, direction::up );
		}

		/* LEFT */
		if( x > 0 && terrain.at( x - 1, y ).node_cost() > 0 )
		{
			const unsigned int current = terrain.at( x - 1, y ).node_cost() + distance;
			handle_direction( x - 1, y, current, direction::right );
		}
	}
	return path_tree;
}