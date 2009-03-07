#include "bsp.h"
#include "polyhedron.h"
#include "containers/vector.h"

bsp_tree::bsp_tree(const vec3* vert, int vcount, const uint16* index, int icount)
{
	assertion(!(icount %3));
	vector<vec3> tmp;
	vector<uint16> remap;
	simplify_vertex_array(tmp,remap,vert,vcount,0.001f);

	vector<uint16> itmp;
	itmp.resize(icount);

	for (int n=0; n<icount; ++n)
		itmp[n]=remap[index[n]];


	root=NULL;

	
	for (int n=0; n<icount; n+=3)
	{
		int i1=itmp[n];
		int i2=itmp[n+1];
		int i3=itmp[n+2];

		vec3 n=cross(tmp[i1]-tmp[i2],tmp[i2]-tmp[i3]);
		n.normalize();
		plane_t p(tmp[i1],n);

		bsp_tree_node* new_node=new (node_buf.allocate_place()) bsp_tree_node;
		new_node->plane=p;

		if (!root)
		{
			root=new_node;
		}
		else
		{
			bsp_tree_node* ptr=root;
			bsp_tree_node* prevptr=NULL;
			bool leftchild;

			while (ptr)
			{
				
			}
		}
	}
}