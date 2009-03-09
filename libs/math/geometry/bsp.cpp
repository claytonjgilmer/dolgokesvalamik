#include "bsp.h"
#include "polyhedron.h"
#include "containers/vector.h"
#include "math/geometry/intersection.h"

#define plane_thickness 0.0001f
void bsp_tree::insert_face(bsp_tree_node*act, bsp_tree_node* parent, bool back, vec3* face, int vnum, plane_t& p)
{
	if (!act)//ha a levelben vagyunk, hozzaadjuk az uj node-ot
	{
		bsp_tree_node* new_node=new (node_buf.allocate_place()) bsp_tree_node;
		new_node->plane=p;


		if (!root)
		{
			root=new_node;
		}
		else
		{
			new_node->parent=parent;

			if (back)
				parent->back_child=new_node;
			else
				parent->front_child=new_node;
		}
	}
	else
	{
		vec3 front[50],back[50];
		int vcount_front, vcount_back;
		int on=split_polygon(act->plane,plane_thickness,face,vnum,front,vcount_front,back,vcount_back);
		if (on)
			return;

		if (vcount_back>=3)
			insert_face(act->back_child,act,true,back,vcount_back,p);
		if (vcount_front>=3)
			insert_face(act->front_child,act,false,front,vcount_front,p);
		
	}
}

bsp_tree::bsp_tree(const vec3* vert, int vcount, const int* index, int icount)
{
	assertion(!(icount %3));
	vector<vec3> tmp;
	vector<int> remap;
	simplify_vertex_array(tmp,remap,vert,vcount,0.001f);

	vector<int> itmp;
	itmp.resize(icount);

	for (int n=0; n<icount; ++n)
		itmp[n]=remap[index[n]];


	root=NULL;

	
	for (int n=0; n<icount; n+=3)
	{
		int i1=itmp[n];
		int i2=itmp[n+1];
		int i3=itmp[n+2];
		vec3 f[3]={tmp[i1],tmp[i2],tmp[i3]};

		vec3 norm=cross(tmp[i1]-tmp[i2],tmp[i2]-tmp[i3]);
		norm.normalize();
		plane_t p(tmp[i1],norm);

		insert_face(root,NULL,true,f,3,p);
	}
}