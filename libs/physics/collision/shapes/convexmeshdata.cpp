#include "convexmeshdata.h"
#include "render/rendersystem.h"

void draw_hull(convex_mesh_data_t* ch, const mtx4x3& mtx)
{
	for (int n=0; n<(int)ch->vertex_vertex_adjindex.size()-1; ++n)
	{
		int firstadj=ch->vertex_vertex_adjindex[n];
		int adjnum=ch->vertex_vertex_adjindex[n+1]-firstadj;

		vec3 start=mtx.transform(ch->vert[n]);

		for (int m=0; m<adjnum; ++m)
		{
			int vindex=ch->vertex_vertex_adjmap[firstadj+m];

			if (vindex<n)
			{
				vec3 end=mtx.transform(ch->vert[vindex]);
				rendersystem::ptr->draw_line(start,color_r8g8b8a8(255,255,255,255),end,color_r8g8b8a8(255,255,255,255));
			}
		}
	}
}
