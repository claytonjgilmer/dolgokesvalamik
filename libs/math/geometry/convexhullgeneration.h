#ifndef _convexhullgeneration_h_
#define _convexhullgeneration_h_

#include "convexhull.h"
#include "math/plane.h"
#include "math\mtx4x3.h"
#include "containers\intr_list.h"

struct convex_hull_desc
{
	vector<vec3> vertex_array;
	double face_thickness;
	double vertex_min_dist;
	bool triangle_output;

	convex_hull_desc()
	{
		face_thickness=0.000001;
		vertex_min_dist=.01;
		triangle_output=false;
	}
};
convex_hull_t generate_convex_hull(const convex_hull_desc& hull_desc);



struct gen_face_t;
struct gen_half_edge_t
{
	int edge_index;
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
		edge_index=-1;

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
	int face_index;

	gen_face_t()
	{
		face_index=-1;
		valid=true;
		freed=false;
	}

	void delete_edges()
	{
		while (edges.first())
		{
			gen_half_edge_t* edge=edges.first();
			edges.erase(edge);
			delete edge;
		}
	}

	~gen_face_t()
	{
		delete_edges();
		freed=true;
	}

	void* operator new(size_t size);
	void operator delete(void* face);
};
struct convex_hull_generator
{
	void do_all(const convex_hull_desc& hull_desc);
	~convex_hull_generator();
	void init(const convex_hull_desc& hull_desc);
	bool generate();
	void get_result();
	void draw(const mtx4x3& mtx);



	void clear();
	void simplify_vertex_array(const vector<vec3>& src_array);
	void set_big_face();
	void calculate_horizon(vector<gen_half_edge_t*>& edge_array, const dvec3& ref_vertex);
	bool is_horizon_edge(gen_half_edge_t* edge, const dvec3& ref_vertex);
	gen_half_edge_t* find_next_horizon_edge(gen_half_edge_t* edge, const dvec3& ref_vertex);
	int check_vertex(gen_face_t* face, const dvec3& v);
	void insert_vertex(const vector<gen_half_edge_t*>& edge_array);
	void merge_faces();


	double get_signed_volume(gen_face_t* face, const vec3& point);




	vector<dvec3> work_array;
	vector<gen_face_t*> faces;
	double plane_thickness;
	double vertex_min_dist;
	bool triangle_output;

	convex_hull_t ch;

	bool state;

	int vertex_index;
	vector<gen_half_edge_t*> new_horizon;
	int first_face;
	vector<gen_face_t*> out_face_array;
};















#endif//_convexhullgeneration_h_
