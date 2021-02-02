#ifndef LEVEL_RENDERER_HPP
#define LEVEL_RENDERER_HPP

#include <QWidget>

namespace patchbot
{

	class level;

	class level_renderer: public QWidget
	{
	public:
		void set_level( level &l ) {};

	private:

		void paintEvent( QPaintEvent *p )
		{

		}
	};

}

#endif	// LEVEL_RENDERER_HPP