#include "utils/misc.h"
#include "quadedge.h"

struct vertex_t
{
	void add_edge(edge_t*);
};

struct face_t
{
	void add_edge(edge_t*);
};
unsigned nextedgeid=4;
struct quad_edge_t
{
	quad_edge_t()
	{
		edges[0].index=0;
		edges[1].index=1;
		edges[2].index=2;
		edges[3].index=3;

		edges[0].next=edges+0;
		edges[1].next=edges+3;
		edges[2].next=edges+2;
		edges[3].next=edges+1;

		edges[0].id=nextedgeid+0;
		edges[1].id=nextedgeid+1;
		edges[2].id=nextedgeid+2;
		edges[3].id=nextedgeid+3;

		nextedgeid+=4;
	}

	edge_t edges[4];
};




edge_t* edge_t::make()
{
	return (new quad_edge_t())->edges;
}

void edge_t::kill()
{
	splice(get_head_prev());
	get_sym()->splice(get_sym()->get_head_prev());

	delete (quad_edge_t*)(this-index);
}

void edge_t::splice(edge_t* other)
{
	edge_t* alpha=get_head_next()->get_rot();
	edge_t* beta=other->get_head_next()->get_rot();

	edge_t* t1=other->get_head_next();
	edge_t* t2=get_head_next();
	edge_t* t3=beta->get_head_next();
	edge_t* t4=alpha->get_head_next();

	next=t1;
	other->next=t2;
	alpha->next=t3;
	beta->next=t4;
}

void edge_t::set_head(vertex_t* head)
{
	vertex=head;
	head->add_edge(this);
}

void edge_t::set_tail(vertex_t* tail)
{
	get_sym()->vertex=tail;
	tail->add_edge(get_sym());
}

void edge_t::set_left_face(face_t* face)
{
	get_rot()->face=face;
	face->add_edge(this);
}

void edge_t::set_right_face(face_t* face)
{
	get_inv_rot()->face=face;
	face->add_edge(get_sym());
}

edge_t::edge_t()
{
	vertex=NULL;
	face=NULL;
}

edge_t::~edge_t()
{
}