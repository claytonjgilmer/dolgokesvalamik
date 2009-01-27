#ifndef _quadedge_h_
#define _quadedge_h_


struct face_t;
struct edge_t;
struct vertex_t;


struct edge_t
{
	static edge_t* make();

	void kill();
	void splice(edge_t* other);

	vertex_t* get_head();
	vertex_t* get_tail();
	void set_head(vertex_t* head);
	void set_tail(vertex_t* tail);

	face_t* get_left_face();
	face_t* get_right_face();

	void set_left_face(face_t* face);
	void set_right_face(face_t* face);

	edge_t* get_rot();
	edge_t* get_inv_rot();
	edge_t* get_sym();

	edge_t* get_head_next();
	edge_t* get_head_prev();

	edge_t* get_tail_next();
	edge_t* get_tail_prev();

	edge_t* get_left_face_next();
	edge_t* get_left_face_prev();

	edge_t* get_right_face_next();
	edge_t* get_right_face_prev();

	edge_t();
	~edge_t();



	unsigned index;
	edge_t* next;
	unsigned id;

	vertex_t* vertex;
	face_t* face;
};



MLINLINE edge_t* edge_t::get_rot()
{
	return index<3 ? this+1 : this-3;
}

MLINLINE edge_t* edge_t::get_inv_rot()
{
	return index>0 ? this-1 : this+3;
}

MLINLINE edge_t* edge_t::get_sym()
{
	return index<2? this+2 : this-2;
}

MLINLINE edge_t* edge_t::get_head_next()
{
	return next;
}

MLINLINE edge_t* edge_t::get_head_prev()
{
	return get_rot()->get_head_next()->get_rot();
}

MLINLINE edge_t* edge_t::get_tail_next()
{
	return get_sym()->get_head_next()->get_sym();
}

MLINLINE edge_t* edge_t::get_tail_prev()
{
	return get_inv_rot()->get_head_next()->get_inv_rot();
}

MLINLINE edge_t* edge_t::get_left_face_next()
{
	return get_inv_rot()->get_head_next()->get_rot();
}

MLINLINE edge_t* edge_t::get_left_face_prev()
{
	return get_head_next()->get_sym();
}

MLINLINE edge_t* edge_t::get_right_face_next()
{
	return get_rot()->get_head_next()->get_inv_rot();
}

MLINLINE edge_t* edge_t::get_right_face_prev()
{
	return get_sym()->get_head_next();
}

MLINLINE vertex_t* edge_t::get_head()
{
	return vertex;
}

MLINLINE vertex_t* edge_t::get_tail()
{
	return get_sym()->get_head();
}

MLINLINE face_t* edge_t::get_left_face()
{
	return get_rot()->face;
}

MLINLINE face_t* edge_t::get_right_face()
{
	return get_inv_rot()->face;
}

#endif//_quadedge_h_