#include "polyhedron.h"
#include "math\sorting.h"

struct vdata
{
	vec3 pos;
	uint16 index;
	uint16 volt;
	vdata(){volt=0;}
	vdata(vec3 v, uint16 i):pos(v),index(i){volt=0;}
};

template<int coord=0>
struct vec_sort_t
{
	bool operator()(const vdata& v1, const vdata& v2) const
	{
		return v1.pos[coord]<v2.pos[coord];
	}
	static bool less(const vdata& v1, const vdata& v2)
	{
		return v1.pos[coord]<v2.pos[coord];
	}
	static bool eq(const vdata& v1, const vdata& v2)
	{
		return v1.pos[coord]==v2.pos[coord];
	}
};

void simplify_vertex_array(vector<vec3>& dst, vector<uint16>& remap, const vec3* src, int vcount, float vertex_min_dist)
{
	vector<vdata> w;
	w.resize(vcount);
	for (int n=0; n<vcount; ++n)
		w[n]=vdata(src[n],uint16(n));
	remap.resize(vcount);
#ifdef _DEBUG
	for (int n=0; n<vcount; ++n)
		remap[n]=(uint16)(-1);
#endif//_DEBUG

	intro_sort(w.begin(),vcount,vec_sort_t<0>());

	int act_index=0;

	for (unsigned n=0; n<w.size(); ++n)
	{
		if (w[n].volt)
			continue;

		for (unsigned m=n; m<w.size(); ++m)
		{
			if (w[m].volt)
				continue;

			if (fabs(w[m].pos.x-w[n].pos.x)>vertex_min_dist)
				break;

			if ((w[m].pos-w[n].pos).squarelength()<=vertex_min_dist*vertex_min_dist)
			{
				w[m].volt=1;
#ifdef _DEBUG
				assertion(remap[w[m].index]==-1);
#endif
				remap[w[m].index]=act_index;
			}
		}
		dst.push_back(w[n].pos);
		++act_index;
	}

#ifdef _DEBUG
	for (unsigned n=0; n<remap.size(); ++n)
		assertion(remap[n]!=-1);
#endif//_DEBUG

}

void generate_from_triangle_soup(polyhedron_t& p, const vec3* v, int vcount, const unsigned short* f, int fcount)
{
	p.vertices.resize(vcount);
	for (int n=0; n<vcount; ++n) p.vertices[n]=v[n];
}