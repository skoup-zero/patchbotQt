#include <path_finding.hpp>

using namespace patchbot;

node::node( const unsigned int distance, const unsigned int x, const unsigned int y )
	:distance_{ distance }
	, x_{ x }
	, y_{ y }
{}

bool compare_nodes::operator()( const node &d1, const node &d2 ) const
{
	return d1.distance_ > d2.distance_;
}

std::vector<std::pair<unsigned int, direction>> dijkstra::calculate_paths( terrain &terrain )
{
	const unsigned int width = terrain.width();
	const unsigned int height = terrain.height();

	/* priority queue as Min-Heap */
	std::priority_queue<node, std::vector<node>, compare_nodes> pq;

	/* initialize path tree with max distance and wait direction */
	std::vector<std::pair<unsigned int, direction>> path_tree
	( width * height, std::make_pair( UINT_MAX, direction::wait ) );

	/* coordinates of patchbot as root */
	pq.push( node( 0, terrain.patchbot_->x_, terrain.patchbot_->y_ ) );

	/* lambda function expand child nodes */
	auto handle_direction = [&]( const unsigned int x, const unsigned int y,
		const unsigned int distance, const direction d, const bool occupied )
	{
		const bool replace = rand() % 2;
		const unsigned int index = width * y + x;

		if( path_tree[index].first > distance )
		{
			path_tree[index].second = d;
			path_tree[index].first = distance;

			if( !occupied )
				pq.push( node( distance, x, y ) );
		}
		else if( replace && path_tree[index].first == distance )
			path_tree[index].second = d;
	};

	while( !pq.empty() )
	{
		const unsigned int distance = pq.top().distance_;
		const unsigned int x = pq.top().x_;
		const unsigned int y = pq.top().y_;
		pq.pop();

		/* UP */
		if( y > 0 && terrain.at( x, y - 1 ).node_cost() >= 0 )
		{
			const bool occupied = terrain.at( x, y - 1 ).node_cost() == 0;
			const unsigned int current = terrain.at( x, y - 1 ).node_cost() + distance;
			handle_direction( x, y - 1, current, direction::down, occupied );
		}

		/* RIGHT */
		if( x < width - 1 && terrain.at( x + 1, y ).node_cost() >= 0 )
		{
			const bool occupied = terrain.at( x + 1, y ).node_cost() == 0;
			const unsigned int current = terrain.at( x + 1, y ).node_cost() + distance;
			handle_direction( x + 1, y, current, direction::left, occupied );
		}

		/* DOWN */
		if( y < height - 1 && terrain.at( x, y + 1 ).node_cost() >= 0 )
		{
			const bool occupied = terrain.at( x, y + 1 ).node_cost() == 0;
			const unsigned int current = terrain.at( x, y + 1 ).node_cost() + distance;
			handle_direction( x, y + 1, current, direction::up, occupied );
		}

		/* LEFT */
		if( x > 0 && terrain.at( x - 1, y ).node_cost() >= 0 )
		{
			const bool occupied = terrain.at( x - 1, y ).node_cost() == 0;
			const unsigned int current = terrain.at( x - 1, y ).node_cost() + distance;
			handle_direction( x - 1, y, current, direction::right, occupied );
		}
	}
	return path_tree;
}