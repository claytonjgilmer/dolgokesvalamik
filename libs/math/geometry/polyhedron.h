#ifndef _polyhedron_h_
#define _polyhedron_h_

#include "math/vec3.h"
#include "math/mtx4x3.h"
#include "containers\vector.h"

typedef vec3 vertex_data;

//struct vertex_data:vec3
//{
//	vertex_data()
//	{
//		pos.clear();
//	}
//};

struct face_data
{
	vec3 normal;
};

struct edge_data
{
	uint16 head_vertex_index;
	uint16 next_edge;
	uint16 prev_edge;
	uint16 opp_edge;
	uint16 face_index;
};

struct polyhedron_t
{
	vector<vertex_data> vertices;
	vector<face_data> faces;
	vector<edge_data> edges;

	vec3 center;
	vec3 half_extent;

	void clear()
	{
		vertices.clear();
		faces.clear();
		edges.clear();
	}

	edge_data* get_next(edge_data* e) //kovetkezo, ugyanahhoz a face-hez tartozo el
	{
		return &edges[e->next_edge];
	}
	edge_data* get_prev(edge_data* e) //elozo, ugyanahhoz a face-hez tartozo el
	{
		return &edges[e->prev_edge];
	}
	edge_data* get_opposite(edge_data* e)
	{
		return &edges[e->opp_edge];
	}
	face_data* get_face(edge_data* e)
	{
		return &faces[e->face_index];
	}

	face_data* get_opposite_face(edge_data* e)
	{
		return get_face(get_opposite(e));
	}

	const vec3& get_face_normal(edge_data* e)
	{
		return get_face(e)->normal;
	}

	const vec3& get_opposite_face_normal(edge_data* e)
	{
		return get_opposite_face(e)->normal;
	}

	edge_data* get_rot_edge(edge_data* e) //kovetkezo, ugyanabba a vertexbe befuto el
	{
		return get_prev(get_opposite(e));
	}

	edge_data* get_tailrot_edge(edge_data* e) //kovetkezo, ugyanabbol a vertexbol kiindulo el
	{
		return get_opposite(get_prev(e));
	}

	const vec3& get_vertexpos(edge_data* e)
	{
		return vertices[e->head_vertex_index];
	}

	const vec3& get_tailvertexpos(edge_data* e)
	{
		return get_vertexpos(get_opposite(e));
	}

	vec3 get_edge_direction(edge_data* e)
	{
		return get_vertexpos(e)-get_tailvertexpos(e);
	}
};

void generate_from_triangle_soup(polyhedron_t& p, const vec3* v, const unsigned short* f);
void simplify_vertex_array(vector<vec3>& dst, vector<uint16>& remap, const vec3* src, int vcount, float vertex_min_dist);


#endif//_polyhedron_h_