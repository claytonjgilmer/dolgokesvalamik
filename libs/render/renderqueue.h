#ifndef _renderqueue_h_
#define _renderqueue_h_

#include "rendermesh.h"
#include "math/mtx4x3.h"
#include "containers/vector.h"
#include "containers/string.h"

namespace render
{
	struct queueelem
	{
		queueelem(){}
		queueelem(mesh* i_mesh, const math::mtx4x3& i_mtx):m_mesh(i_mesh),m_mtx(i_mtx){}
		mesh*	m_mesh;
		math::mtx4x3	m_mtx;
	};

	struct queue
	{
		ctr::vector<queueelem> m_buf;
		ctr::string m_name;
	};
}
#endif//_renderqueue_h_