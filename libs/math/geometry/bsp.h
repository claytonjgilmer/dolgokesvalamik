#ifndef _bsp_h_
#define _bsp_h_

#include "math/vec3.h"
#include "math/plane.h"
#include "containers/listallocator.h"

struct bsp_tree_node
{
	plane_t plane;
	bsp_tree_node* parent;
	bsp_tree_node* back_child;
	bsp_tree_node* front_child;

	bsp_tree_node()
	{
		parent=back_child=front_child=NULL;
	}
};

struct bsp_tree
{
	bsp_tree(const vec3* vert, int vcount, const int* index, int icount);
	list_allocator<sizeof(bsp_tree_node),128> node_buf;
	bsp_tree_node* root;


	void insert_face(bsp_tree_node*act, bsp_tree_node* prev, bool back, vec3* face, int vnum, plane_t& p);
};
#endif//_bsp_h_