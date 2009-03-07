#ifndef _bsp_h_
#define _bsp_h_

#include "math/vec3.h"
#include "containers/listallocator.h"

struct bsp_tree_node
{
	plane_t plane;
	bsp_tree_node* parent;
	bsp_tree_node* left_child;
	bsp_tree_node* right_child;

	bsp_tree_node()
	{
		parent=left_child=right_child=NULL;
	}
};

struct bsp_tree
{
	bsp_tree(const vec3* vert, int vcount, const uint16* index, int icount);
	list_allocator<bsp_tree_node> node_buf;
	bsp_tree_node* root;
};
#endif//_bsp_h_