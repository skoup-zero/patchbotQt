#include <path_finding.hpp>

using namespace patchbot;

bool compare_nodes::operator()( std::tuple<unsigned int, unsigned int, unsigned int> const &t1,
	std::tuple<unsigned int, unsigned int, unsigned int> const &t2 ) const
{
	return std::get<0>( t1 ) > std::get<0>( t2 );
}

void dijkstra::calculate_path( terrain &terrain )
{
	const auto width = terrain.width();
	const auto height = terrain.height();

	std::priority_queue<std::tuple<unsigned int, unsigned int, unsigned int>,
		std::vector<std::tuple<unsigned int, unsigned int, unsigned int>>,
		compare_nodes> pq;

	terrain.at( terrain.patchbot_->x_ , terrain.patchbot_->y_ ).predecessor_ = direction::undefined;
	pq.push( std::make_tuple( 0, terrain.patchbot_->x_, terrain.patchbot_->y_ ) );

	while( !pq.empty() )
	{
		auto cost = std::get<0>( pq.top() );
		const auto x = std::get<1>( pq.top() );
		const auto y = std::get<2>( pq.top() );
		pq.pop();

		auto &node = terrain.at( x, y );
		
		if( cost > node.path_cost_ )
		{
			std::cout << "got skipped" << std::endl;
			continue;
		}

		/* UP */
		if( y > 0 )
		{
			auto &child = terrain.at( x, y - 1 );
			const auto child_cost = child.node_cost();

			if( child_cost > 0 && child.predecessor_ != direction::up )
			{
				if( child.path_cost_ > cost + child_cost )
				{
					child.path_cost_ = cost + child_cost;
					child.predecessor_ = direction::down;
					pq.push( std::make_tuple( cost + child_cost, x, y - 1 ) );
				}
			}
		}

		/* RIGHT */
		if( x < width - 1 )
		{
			auto &child = terrain.at( x + 1, y );
			const auto child_cost = child.node_cost();

			if( child_cost > 0 && child.predecessor_ != direction::right )
			{
				if( child.path_cost_ > cost + child_cost )
				{
					child.path_cost_ = cost + child_cost;
					child.predecessor_ = direction::left;
					pq.push( std::make_tuple( cost + child_cost, x + 1, y ) );
				}
			}
		}

		/* DOWN */
		if( y < height - 1 )
		{
			auto &child = terrain.at( x, y + 1 );
			const auto child_cost = child.node_cost();

			if( child_cost > 0 && child.predecessor_ != direction::down )
			{
				if( child.path_cost_ > cost + child_cost )
				{
					child.path_cost_ = cost + child_cost;
					child.predecessor_ = direction::up;
					pq.push( std::make_tuple( cost + child_cost, x, y + 1 ) );
				}
			}
		}

		/* LEFT */
		if( x > 0 )
		{
			auto &child = terrain.at( x - 1, y );
			const auto child_cost = child.node_cost();

			if( child_cost > 0 && child.predecessor_ != direction::left )
			{
				if( child.path_cost_ > cost + child_cost )
				{
					child.path_cost_ = cost + child_cost;
					child.predecessor_ = direction::right;
					pq.push( std::make_tuple( cost + child_cost, x - 1, y ) );
				}
			}
		}
	}
}


//bool compare_nodes::operator()( std::tuple<unsigned int, unsigned int, unsigned int> const &t1,
	//	std::tuple<unsigned int, unsigned int, unsigned int> const &t2 ) const
	//{
	//	return std::get<0>( t1 ) < std::get<0>( t2 );
	//}

//void dijkstra::calculate_path( terrain &terrain )
//{
//	const auto width = terrain.width();
//	const auto height = terrain.height();
//
//	std::priority_queue<std::tuple<unsigned int, unsigned int, unsigned int>,
//		std::vector<std::tuple<unsigned int, unsigned int, unsigned int>>,
//		compare_nodes> pq;
//
//	std::vector<int> costs( width * height, UINT_MAX );
//
//	std::tuple<unsigned int, unsigned int, unsigned int> node;
//
//	terrain.at( terrain.patchbot_->x_, terrain.patchbot_->y_ ).predecessor_ = direction::undefined;
//	pq.push( std::make_tuple( 0, terrain.patchbot_->x_, terrain.patchbot_->y_ ) );
//
//	while( !pq.empty() )
//	{
//		auto cost = std::get<0>( pq.top() );
//		const auto x = std::get<1>( pq.top() );
//		const auto y = std::get<2>( pq.top() );
//		pq.pop();
//
//		if( cost > costs[width * y + x] )
//			continue;
//
//		/* UP */
//		if( y > 0 )
//		{
//			auto &child = terrain.at( x, y - 1 );
//			const auto child_cost = child.node_cost();
//
//			if( child_cost > 0 && child.predecessor_ != direction::up )
//			{
//				if( costs[width * ( y - 1 ) + x] > cost + child_cost )
//				{
//					costs[width * ( y - 1 ) + x] = cost + child_cost;
//					child.predecessor_ = direction::down;
//					pq.push( std::make_tuple( cost + child_cost, x, y - 1 ) );
//				}
//			}
//		}
//
//		/* RIGHT */
//		if( x < width - 1 )
//		{
//			auto &child = terrain.at( x + 1, y );
//			const auto child_cost = child.node_cost();
//
//			if( child_cost > 0 && child.predecessor_ != direction::right )
//			{
//				if( costs[width * y + x + 1] > cost + child_cost )
//				{
//					costs[width * y + x + 1] = cost + child_cost;
//					child.predecessor_ = direction::left;
//					pq.push( std::make_tuple( cost + child_cost, x + 1, y ) );
//				}
//			}
//		}
//
//		/* DOWN */
//		if( y < height - 1 )
//		{
//			auto &child = terrain.at( x, y + 1 );
//			const auto child_cost = child.node_cost();
//
//			if( child_cost > 0 && child.predecessor_ != direction::down )
//			{
//				if( costs[width * ( y + 1 ) + x] > cost + child_cost )
//				{
//					costs[width * ( y + 1 ) + x] = cost + child_cost;
//					child.predecessor_ = direction::up;
//					pq.push( std::make_tuple( cost + child_cost, x, y + 1 ) );
//				}
//			}
//		}
//
//		/* LEFT */
//		if( x > 0 )
//		{
//			auto &child = terrain.at( x - 1, y );
//			const auto child_cost = child.node_cost();
//
//			if( child_cost > 0 && child.predecessor_ != direction::left )
//			{
//				if( costs[width * y + x - 1] > cost + child_cost )
//				{
//					costs[width * y + x - 1] = cost + child_cost;
//					child.predecessor_ = direction::right;
//					pq.push( std::make_tuple( cost + child_cost, x - 1, y ) );
//				}
//			}
//
//		}
//	}
//}