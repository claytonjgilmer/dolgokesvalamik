#ifndef _convexhullgeneration_h_
#define _convexhullgeneration_h_

#include "convexhull.h"
#include "math/plane.h"
#include "containers\intr_list.h"

struct convex_hull_desc
{
	vector<vec3> vertex_array;
	double face_thickness;
	bool triangle_output;

	convex_hull_desc()
	{
		face_thickness=0.000001;
		triangle_output=false;
	}
};
convex_hull generate_convex_hull(const convex_hull_desc& hull_desc);



struct gen_face_t;
struct gen_half_edge_t
{
	int head_vertex;
	gen_half_edge_t* opposite;
	gen_face_t* face;

	gen_half_edge_t* prev;
	gen_half_edge_t* next;

	bool freed;

	gen_half_edge_t(gen_face_t* i_face,int i_head_vertex):
	head_vertex(i_head_vertex),
		face(i_face)
	{
		prev=next=NULL;
		freed=false;
		opposite=NULL;

	}

	~gen_half_edge_t()
	{
		assertion(!freed);
		freed=true;
	}



	void* operator new(size_t size);
	void operator delete(void* edge);
};
struct gen_face_t:dplane_t
{
	struct intr_circular_list<gen_half_edge_t> edges; //vertexek, koruljarasi sorrendben
	bool valid;
	bool freed;

	gen_face_t()
	{
		valid=true;
		freed=false;
	}

	~gen_face_t()
	{
		while (edges.first())
		{
			gen_half_edge_t* edge=edges.first();
			edges.erase(edge);
			delete edge;
		}

		freed=true;
	}

	void* operator new(size_t size);
	void operator delete(void* face);
};
struct convex_hull_generator
{
	~convex_hull_generator();
	void init(const convex_hull_desc& hull_desc);
	bool generate();
	void get_result();
	void draw();



	void simplify_vertex_array(const vector<vec3>& src_array);
	void set_big_face();
	void calculate_horizon(vector<gen_half_edge_t*>& edge_array, const dvec3& ref_vertex);
	bool is_horizon_edge(gen_half_edge_t* edge, const dvec3& ref_vertex);
	gen_half_edge_t* find_next_horizon_edge(gen_half_edge_t* edge, const dvec3& ref_vertex);
	int check_vertex(gen_face_t* face, const dvec3& v);
	void insert_vertex(const vector<gen_half_edge_t*>& edge_array);
	void merge_faces();
	vector<dvec3> work_array;
	vector<gen_face_t*> faces;
	double dplane_thickness;
	bool triangle_output;

	convex_hull ch;

	bool state;

	int vertex_index;
	vector<gen_half_edge_t*> new_horizon;
	int first_face;
};















#endif//_convexhullgeneration_h_
