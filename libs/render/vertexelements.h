#ifndef _vertexelements_h_
#define _vertexelements_h_

	typedef enum 
	{
		vertexelement_position,
		vertexelement_normal,
		vertexelement_uv,
		vertexelement_color,
		vertexelement_tangent,
		vertexelement_binormal,

		vertexelement_num,
		vertexelement_unused=vertexelement_num,
	} vertexelemtype;

	struct vertexelem
	{
		vertexelemtype m_elemtype;
		unsigned m_elemsize;

		vertexelem(vertexelemtype i_type,unsigned i_size): m_elemtype(i_type),m_elemsize(i_size){}
	};
#endif//_vertexelements_h_