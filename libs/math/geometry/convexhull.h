#ifndef _convexhull_h_
#define _convexhull_h_

#include "math/vec3.h"
#include "math/mtx4x3.h"
#include "containers\vector.h"

struct vertex_data
{
    vec3 pos;
//    uint16 adj_vertex_map_index;
//	uint16 adj_edge_index;
};

/*
struct vertex_vertex_adjacency
{
	uint16 first_adj_vertex_index;
};
*/
struct face_data
{
    vec3 normal;
//	uint16 adj_edge_index;
};

struct edge_data
{
	uint16 head_vertex_index;
	uint16 next_edge;
	uint16 prev_edge;
	uint16 opp_edge;
	uint16 face_index;
};

struct convex_hull_t
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

//	edge_data* get_edge(vertex_data* v)
//	{
//		return &edges[v->adj_edge_index];
//	}

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
		return vertices[e->head_vertex_index].pos;
	}

	const vec3& get_tailvertexpos(edge_data* e)
	{
		return get_vertexpos(get_opposite(e));
	}

	vec3 get_edge_direction(edge_data* e)
	{
		return get_vertexpos(e)-get_tailvertexpos(e);
	}

	void get_extremal_vertex(const vec3& dir, vec3& extr_vertex)
	{
//		static edge_data* starte=&edges[0];

		edge_data* starte=get_opposite(&edges[0]);//(get_edge(&vertices[0]));
		float max_dist=dot(get_vertexpos(&edges[0]),dir);
//		float max_dist=dot(get_tailvertexpos(starte),dir);
		edge_data* ende=starte;
_fasszom:
		edge_data* e=starte;

		do 
		{
			float actdist=dot(get_vertexpos(e),dir);

			if (actdist>max_dist)
			{
				max_dist=actdist;
				ende=get_opposite(e);
				starte=get_tailrot_edge(ende);
				goto _fasszom;
			}
			e=get_tailrot_edge(e);
		} while (e!=ende);

		extr_vertex=get_tailvertexpos(starte);
	}

	edge_data* get_extremal_vertex(const vec3& dir)
	{
		//		static edge_data* starte=&edges[0];
//		float max_dist=dot(vertices[0].pos,dir);

		edge_data* starte=get_opposite(&edges[0]);//(get_edge(&vertices[0]));
		float max_dist=dot(get_vertexpos(&edges[0]),dir);
		edge_data* ende=starte;
_fasszom:
		edge_data* e=starte;

		do 
		{
			float actdist=dot(get_vertexpos(e),dir);

			if (actdist>max_dist)
			{
				max_dist=actdist;
				ende=get_opposite(e);
				starte=get_tailrot_edge(ende);
				goto _fasszom;
			}
			e=get_tailrot_edge(e);
		} while (e!=ende);

		return get_opposite(starte);
	}

	edge_data* get_extremal_vertex(const vec3& dir,edge_data* start_vertex)
	{
		edge_data* starte=get_opposite(start_vertex);
		float max_dist=dot(get_vertexpos(start_vertex),dir);
		edge_data* ende=starte;
_fasszom:
		edge_data* e=starte;

		do 
		{
			float actdist=dot(get_vertexpos(e),dir);

			if (actdist>max_dist+.0001)
			{
				max_dist=actdist;
				ende=get_opposite(e);
				starte=get_tailrot_edge(ende);
				goto _fasszom;
			}
			e=get_tailrot_edge(e);
		} while (e!=ende);

		return get_opposite(starte);
	}

	bool is_extremal_vertex(const vec3& dir, edge_data* vedge)
	{
		//		static edge_data* starte=&edges[0];
		float max_dist=dot(get_vertexpos(vedge),dir);

		edge_data* starte=get_opposite(vedge);
		edge_data* ende=starte;
		edge_data* e=starte;

		do 
		{
			float actdist=dot(get_vertexpos(e),dir);
			if (actdist>max_dist+0.0001)
				return false;
			e=get_tailrot_edge(e);
		} while (e!=ende);

		return true;
	}
};

struct convex_mesh_data_t;

void hull_to_convex_mesh(convex_mesh_data_t& obj, convex_hull_t& hull);

void draw_hull(convex_hull_t* ch, const mtx4x3& mtx);
#endif//_convexhull_h_
