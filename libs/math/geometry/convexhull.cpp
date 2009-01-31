#include "convexhull.h"
#include "render/rendersystem.h"

void draw_hull(convex_hull* ch)
{
	for (int n=0; n<(int)ch->vertices.size()-1; ++n)
	{
		int firstadj=ch->vertices[n].adj_index;
		int adjnum=ch->vertices[n+1].adj_index-firstadj;

		vec3 start=ch->vertices[n].pos;

		for (int m=0; m<adjnum; ++m)
		{
			int vindex=ch->vertex_adjacency[firstadj+m];

			if (vindex<n)
			{
				vec3 end=ch->vertices[vindex].pos;
				rendersystem::ptr->draw_line(start,color_r8g8b8a8(255,255,255,255),end,color_r8g8b8a8(255,255,255,255));
			}
		}
	}
}
