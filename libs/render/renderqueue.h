#ifndef _renderqueue_h_
#define _renderqueue_h_

#include "mesh.h"
#include "math/mtx4x3.h"
#include "containers/vector.h"
#include "containers/string.h"

	struct queueelem
	{
		queueelem(){}
		queueelem(mesh_t* i_mesh, const mtx4x3& i_mtx):m_mesh(i_mesh),m_mtx(i_mtx){}
		mesh_t*	m_mesh;
		mtx4x3	m_mtx;
	};

	struct renderqueue
	{
		vector<queueelem> m_buf;
		string m_name;
	};
#endif//_renderqueue_h_
