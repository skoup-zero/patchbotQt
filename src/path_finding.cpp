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
	const auto width = terrain.width();
	const auto height = terrain.height();

	std::priority_queue<
		std::tuple<unsigned int, unsigned int, unsigned int>,
		std::vector<std::tuple<unsigned int, unsigned int, unsigned int>>,
		compare_nodes> pq;

	std::vector<std::tuple<unsigned int, direction>> path_tree(
		width * height, std::make_tuple( UINT_MAX, direction::undefined ) );

	pq.push( std::make_tuple( 0, terrain.patchbot_->x_, terrain.patchbot_->y_ ) );

	while( !pq.empty() )
	{
		auto const cost = std::get<0>( pq.top() );
		const auto x = std::get<1>( pq.top() );
		const auto y = std::get<2>( pq.top() );
		pq.pop();
		
		/* UP */
		if( y > 0 )
		{
			const auto child_cost = terrain.at( x, y - 1 ).node_cost();
			const auto index = width * ( y - 1 ) + x;

			if( child_cost > 0 && std::get<0>( path_tree[index] ) > cost + child_cost)
			{
				std::get<0>( path_tree[index] ) = cost + child_cost;
				std::get<1>( path_tree[index] ) = direction::down;
				pq.push( std::make_tuple( cost + child_cost, x, y - 1 ) );
			}
		}

		/* RIGHT */
		if( x < width - 1 )
		{
			const auto child_cost = terrain.at( x + 1, y ).node_cost();
			const auto index = width * y + x + 1;

			if( child_cost > 0 && std::get<0>( path_tree[index] ) > cost + child_cost )
			{
				std::get<0>( path_tree[index] ) = cost + child_cost;
				std::get<1>( path_tree[index] ) = direction::left;
				pq.push( std::make_tuple( cost + child_cost, x + 1, y ) );
			}
		}

		/* DOWN */
		if( y < height - 1 )
		{
			const auto child_cost = terrain.at( x, y + 1 ).node_cost();
			const auto index = width * ( y + 1 ) + x;

			if( child_cost > 0 && std::get<0>( path_tree[index] ) > cost + child_cost )
			{
				std::get<0>( path_tree[index] ) = cost + child_cost;
				std::get<1>( path_tree[index] ) = direction::up;
				pq.push( std::make_tuple( cost + child_cost, x, y + 1 ) );
			}
		}

		/* LEFT */
		if( x > 0 )
		{
			const auto child_cost = terrain.at( x - 1, y ).node_cost();
			const auto index = width * y + x - 1;

			if( child_cost > 0 && std::get<0>( path_tree[index] ) > cost + child_cost )
			{
				std::get<0>( path_tree[index] ) = cost + child_cost;
				std::get<1>( path_tree[index] ) = direction::right;
				pq.push( std::make_tuple( cost + child_cost, x - 1, y ) );
			}
		}
	}

	return path_tree;
}