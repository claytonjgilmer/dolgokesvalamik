#ifndef _convexhull_h_
#define _convexhull_h_

#include "math/vec3.h"
#include "containers\vector.h"

struct vertex_data
{
    vec3 pos;
    uint16 adj_index;
};

struct face_data
{
    vec3 normal;
    uint16 vertex_index;
};

struct edge_data
{
    vec3 dir;
    uint16 vertex_index;
};

struct convex_hull
{
    vector<vertex_data> vertices;
    vector<uint16> vertex_adjacency;

    vector<face_data> faces;
    vector<uint16> face_indices;

    vector<edge_data> edges;
    vector<uint16> edge_indices;
};
#endif//_convexhull_h_
