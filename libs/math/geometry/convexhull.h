#ifndef _convexhull_h_
#define _convexhull_h_

#include "polyhedron.h"

struct convex_hull_t:polyhedron_t
{
	void get_extremal_vertex(const vec3& dir, vec3& extr_vertex)
	{
		edge_data* starte=get_opposite(&edges[0]);//(get_edge(&vertices[0]));
		f32 max_dist=dot(get_vertexpos(&edges[0]),dir);
		edge_data* ende=starte;
_fasszom:
		edge_data* e=starte;

		do 
		{
			f32 actdist=dot(get_vertexpos(e),dir);

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
		edge_data* starte=get_opposite(&edges[0]);//(get_edge(&vertices[0]));
		f32 max_dist=dot(get_vertexpos(&edges[0]),dir);
		edge_data* ende=starte;
_fasszom:
		edge_data* e=starte;

		do 
		{
			f32 actdist=dot(get_vertexpos(e),dir);

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
		f32 max_dist=dot(get_vertexpos(start_vertex),dir);
		edge_data* ende=starte;
_fasszom:
		edge_data* e=starte;

		do 
		{
			f32 actdist=dot(get_vertexpos(e),dir);

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
		f32 max_dist=dot(get_vertexpos(vedge),dir);

		edge_data* starte=get_opposite(vedge);
		edge_data* ende=starte;
		edge_data* e=starte;

		do 
		{
			f32 actdist=dot(get_vertexpos(e),dir);
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
