#include "convexhull.h"
#include "render/rendersystem.h"
#include "physics/collision/shapes/convexmeshdata.h"
#include "math/aabox.h"

void draw_hull(convex_hull_t* ch, const mtx4x3& mtx)
{
/*
	for (int n=0; n<(int)ch->vertices.size()-1; ++n)
	{
		int firstadj=ch->vertices[n].adj_vertex_map_index;
		int adjnum=ch->vertices[n+1].adj_vertex_map_index-firstadj;

		vec3 start=mtx.transform(ch->vertices[n].pos);

		for (int m=0; m<adjnum; ++m)
		{
			int vindex=ch->vertex_adjacency[firstadj+m];

			if (vindex<n)
			{
				vec3 end=mtx.transform(ch->vertices[vindex].pos);
				rendersystem::ptr->draw_line(start,color_r8g8b8a8(255,255,255,255),end,color_r8g8b8a8(255,255,255,255));
			}
		}
	}
*/
}

void hull_to_convex_mesh(convex_mesh_data_t& obj, convex_hull_t& hull)
{
	obj.vert.resize(hull.vertices.size());
	obj.vertex_vertex_adjindex.resize(hull.vertices.size()+1);
	obj.vertex_vertex_adjmap.resize(hull.edges.size());

	aabb_t aabb;aabb.min.set(FLT_MAX,FLT_MAX,FLT_MAX); aabb.max=-aabb.min;

	for (unsigned n=0; n<obj.vert.size(); ++n)
	{
		obj.vert[n]=hull.vertices[n];
		aabb.extend(obj.vert[n]);
	}
	obj.center=.5f*(aabb.min+aabb.max);
	obj.half_extent=.5f*(aabb.max-aabb.min);
	int adjnum=0;
	vector<edge_data*> vert_edges;
	vert_edges.resize(obj.vert.size());
	for (uint32 n=0; n<hull.edges.size(); ++n)
		vert_edges[hull.edges[n].head_vertex_index]=&hull.edges[n];

	for (uint32 n=0; n<obj.vert.size(); ++n)
	{
		obj.vertex_vertex_adjindex[n]=adjnum;
		edge_data* sedge=vert_edges[n];
		edge_data* e=sedge;

		do 
		{
			obj.vertex_vertex_adjmap[adjnum]=hull.get_opposite(e)->head_vertex_index;
			++adjnum;
			e=hull.get_rot_edge(e);
		} while (e!=sedge);
	}
	obj.vertex_vertex_adjindex[obj.vert.size()]=adjnum;
	obj.facenormal.resize(hull.faces.size());

	for (unsigned n=0; n<obj.facenormal.size(); ++n)
		obj.facenormal[n]=hull.faces[n].normal;
}
