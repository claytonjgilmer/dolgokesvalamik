#ifndef _renderqueue_h_
#define _renderqueue_h_

#include "mesh.h"
#include "math/mtx4x3.h"
#include "containers/vector.h"
#include "containers/string.h"
#include "shaderparam.h"

	struct queueelem
	{
		queueelem(){}
		queueelem(mesh_t* i_mesh, shader_param_array_t* i_param_array, const mtx4x3& mtx): m_mesh(i_mesh),m_mtx(mtx),param_array(i_param_array){}
		mesh_t*	m_mesh;
		mtx4x3	m_mtx;
		shader_param_array_t* param_array;
	};

	struct renderqueue
	{
		vector<queueelem> m_buf;
		string m_name;
	};
#endif//_renderqueue_h_
