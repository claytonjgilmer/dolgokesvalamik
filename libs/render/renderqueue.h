#ifndef _renderqueue_h_
#define _renderqueue_h_

#include "rendermesh.h"
#include "math/mtx4x3.h"

namespace render
{
	struct queueelem
	{
		mesh*	m_mesh;
		math::mtx4x3	m_mtx;
	};

	class queue
	{
	public:
		void render();
		void clear();

	private:
		ctr::vector<queueelem> m_buf;
	};
}
#endif//_renderqueue_h_