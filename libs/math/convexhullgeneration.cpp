#include <algorithm>
#include "convexhullgeneration.h"
#include "sorting.h"
#include "plane.h"
#include "containers\intr_list.h"
#include "containers\listallocator.h"

const int PLANE_OUTSIDE=1;
const int PLANE_INSIDE=-1;
const int PLANE_ON=0;

struct gen_face_t;
struct gen_half_edge_t;

struct gen_half_edge_t
{
    int head_vertex;
    gen_half_edge_t* opposite;
    gen_face_t* face;

    gen_half_edge_t* prev;
    gen_half_edge_t* next;

    gen_half_edge_t(gen_face_t* i_face,int i_head_vertex):
    head_vertex(i_head_vertex),
    face(i_face)
    {
    	prev=next=NULL;
    }

	void* operator new(size_t size);
	void operator delete(void* edge);
};

list_allocator<gen_half_edge_t,2048> edge_allocator;

void* gen_half_edge_t::operator new(size_t size)
{
	return edge_allocator.allocate_place();
}

void gen_half_edge_t::operator delete(void* edge)
{
	return edge_allocator.deallocate_place((gen_half_edge_t*)edge);
}


struct gen_face_t:plane_t
{
    struct intr_circular_list<gen_half_edge_t> edges; //vertexek, koruljarasi sorrendben
    bool valid;

	gen_face_t()
	{
		valid=true;
	}

    ~gen_face_t()
    {
    	while (edges.first())
    	{
    		gen_half_edge_t* edge=edges.first();
    		edges.erase(edge);
    		delete edge;
    	}
    }

	void* operator new(size_t size);
	void operator delete(void* face);
};

list_allocator<gen_face_t,2048> face_allocator;

void* gen_face_t::operator new(size_t size)
{
	return face_allocator.allocate_place();
}

void gen_face_t::operator delete(void* face)
{
	face_allocator.deallocate_place((gen_face_t*)face);
}









template<int coord=0>
struct vec_sort_t
{
     bool operator()(const vec3& v1, const vec3& v2) const
    {
        return v1[coord]<v2[coord];
    }
    static bool less(const vec3& v1, const vec3& v2)
    {
        return v1[coord]<v2[coord];
    }
    static bool eq(const vec3& v1, const vec3& v2)
    {
        return v1[coord]==v2[coord];
    }
};

struct convex_hull_generator
{
	~convex_hull_generator();
	void generate(const convex_hull_desc& hull_desc);



	void simplify_vertex_array(const vector<vec3>& src_array);
	void set_big_face();
	void calculate_horizon(vector<gen_half_edge_t*>& edge_array, const vec3& ref_vertex);
	bool is_horizon_edge(gen_half_edge_t* edge, const vec3& ref_vertex);
	gen_half_edge_t* find_next_horizon_edge(gen_half_edge_t* edge, const vec3& ref_vertex);
	int check_vertex(gen_face_t* face, const vec3& v);
	void insert_vertex(int vertex_index, const vector<gen_half_edge_t*>& edge_array, vector<gen_half_edge_t*>& new_horizon);
	void merge_faces(vector<gen_half_edge_t*>& edge_array, int vertex_index);
	vector<vec3> work_array;
	vector<gen_face_t*> faces;
	float plane_thickness;

	convex_hull ch;
};

#define _MAX_VERTEX_DIST_ 0.01
#define _MAX_VERTEX_DIST_SQUARE_ (_MAX_VERTEX_DIST_*_MAX_VERTEX_DIST_)

void convex_hull_generator::simplify_vertex_array(const vector<vec3>& src_array)
{
    vector<vec3> tmp_array=src_array;
    intro_sort(tmp_array.begin(), tmp_array.size(),vec_sort_t<0>());
//	std::sort(tmp_array.begin(), tmp_array.end(),vec_sort_t<0>());

    vec3 sum=tmp_array[0];
    float num=1;

//	work_array.reserve(src_array.size());

    for (unsigned n=1; n<tmp_array.size(); ++n)
    {
		if ((tmp_array[n]-sum/num).squarelength()>_MAX_VERTEX_DIST_SQUARE_)
        {
            work_array.push_back(sum/num);
            sum.set(0,0,0);
            num=0;
        }

		sum+=tmp_array[n];
		++num;
    }
    work_array.push_back(sum/num);
    num=1;
}

void convex_hull_generator::set_big_face()
{
    int face[3];
    vec3 bmin,bmax;
    bmin.set(FLT_MAX,FLT_MAX,FLT_MAX);
    bmax.set(-FLT_MAX,-FLT_MAX,-FLT_MAX);

    int minindex[3], maxindex[3];

	for (unsigned n=0; n<work_array.size(); ++n)
	{
		for (int m=0; m<3; ++m)
		{
			if (work_array[n][m]<bmin[m])
			{
				bmin[m]=work_array[n][m];
				minindex[m]=n;
			}
			if (work_array[n][m]>bmax[m])
			{
				bmax[m]=work_array[n][m];
				maxindex[m]=n;
			}
		}
	}

    vec3 extent=bmax-bmin;
	float* minptr=max_elem(&extent[0],&extent[3]);
	int	axis=minptr-&extent[0];

	face[0]=minindex[axis];
	face[1]=maxindex[axis];

	vec3 segmentdir=work_array[face[1]]-work_array[face[0]];
	segmentdir.normalize();
	float maxdist=-FLT_MAX;
	for (unsigned int n=0; n<work_array.size(); ++n)
    {
        vec3 v=work_array[n]-work_array[face[0]];
        float actdist=(v-dot(v,segmentdir)*segmentdir).squarelength();

        if (actdist>maxdist)
        {
            maxdist=actdist;
            face[2]=n;
        }
    }

//    introsort<int> i; i(face,face+2);
    introsort<int> i; i(face,3);

	swap(work_array[0],work_array[face[0]]);
	swap(work_array[1],work_array[face[1]]);
	swap(work_array[2],work_array[face[2]]);

	plane_t plane;
	plane.set(work_array[0],cross(work_array[2]-work_array[0],work_array[1]-work_array[0]));

	//most megkeressuk a legtavolabbi vertexet az elso face-tol, es az lesz a 4. vertex;

	float max_dist=-FLT_MAX;
	int max_index;
	for (unsigned n=3; n<work_array.size(); ++n)
	{
		float act_dist=fabsf(plane.get_distance(work_array[n]));
		if (act_dist>max_dist)
		{
			max_dist=act_dist;
			max_index=n;
		}
	}

	swap(work_array[3],work_array[max_index]);
}

bool convex_hull_generator::is_horizon_edge(gen_half_edge_t* edge, const vec3& ref_vertex) //sajat face out kell hogy legyen
{
	return (check_vertex(edge->opposite->face,ref_vertex)!=PLANE_OUTSIDE);
}

gen_half_edge_t* convex_hull_generator::find_next_horizon_edge(gen_half_edge_t* edge, const vec3& ref_vertex)
{
	while (!is_horizon_edge(edge->next,ref_vertex))
		edge=edge->next->opposite;

	return edge->next;
	if (is_horizon_edge(edge->next,ref_vertex))
		return edge->next;
	else
		return find_next_horizon_edge(edge->next->opposite,ref_vertex);
}

void convex_hull_generator::calculate_horizon(vector<gen_half_edge_t*>& edge_array, const vec3& ref_vertex)
{
	gen_half_edge_t* next_edge=find_next_horizon_edge(edge_array.back(), ref_vertex);

	while (next_edge!=edge_array[0])
	{
		edge_array.push_back(next_edge);
		next_edge=find_next_horizon_edge(edge_array.back(),ref_vertex);
	}

/*
	if (next_edge!=edge_array[0])
	{
		edge_array.push_back(next_edge);
		calculate_horizon(edge_array,ref_vertex);
	}
*/
}

void convex_hull_generator::insert_vertex(int vertex_index, const vector<gen_half_edge_t*>& edge_array, vector<gen_half_edge_t*>& new_horizon)
{
	int first_face=faces.size();
	int edge_count=edge_array.size();
	for (int n=0; n<edge_count; ++n)
	{
		gen_face_t* face=new gen_face_t();
		faces.push_back(face);
		int v1=edge_array[n]->opposite->head_vertex;
		int v2=edge_array[n]->head_vertex;
		int v3=vertex_index;

		gen_half_edge_t* edge1=new gen_half_edge_t(face,v1);
		gen_half_edge_t* edge2=new gen_half_edge_t(face,v2);
		edge2->opposite=edge_array[n]->opposite;
		edge2->opposite->opposite=edge2;
		new_horizon.push_back(edge2);
		gen_half_edge_t* edge3=new gen_half_edge_t(face,v3);

		face->edges.push_back(edge1);
		face->edges.push_back(edge2);
		face->edges.push_back(edge3);
		face->set(work_array[v1],cross(work_array[v3]-work_array[v1],work_array[v2]-work_array[v1]));
	}

	for (int n=0; n<edge_count; ++n)
	{
		int prevface=((n+edge_count-1) %edge_count)+first_face;
		int actface=n+first_face;
		int nextface=((n+1) %edge_count)+first_face;

		faces[actface]->edges.first()->opposite=faces[prevface]->edges.first()->prev;
		faces[actface]->edges.first()->prev->opposite=faces[nextface]->edges.first();
	}
}

convex_hull generate_convex_hull(const convex_hull_desc& hull_desc)
{
	convex_hull_generator chg;
	chg.generate(hull_desc);

	return chg.ch;
}

int convex_hull_generator::check_vertex(gen_face_t* face, const vec3& v)
{
	float dist=face->get_distance(v);
	if (dist>plane_thickness)
		return PLANE_OUTSIDE;
	if (dist<-plane_thickness)
		return PLANE_INSIDE;
	else
		return PLANE_ON;
}

void convex_hull_generator::merge_faces(vector<gen_half_edge_t*>& edge_array, int vertex_index)
{
	vec3 v=work_array[vertex_index];
	for (unsigned n=0; n<edge_array.size(); ++n)
	{
		gen_half_edge_t* e=edge_array[n];
		if (check_vertex(e->opposite->face,v)==PLANE_ON) //fos az el, meg kell szuntetni
		{
			gen_half_edge_t* preve=e->prev;
			gen_half_edge_t* nexte=e->next;
			e->prev->next=e->opposite->next;
			e->next->prev=e->opposite->prev;
			e->opposite->next->prev=e->prev;
			e->opposite->prev->next=e->next;
			//most mar e-re es e->opposite-ra senki sem hivatkozik
			e->opposite->face->edges.clear();

			//a szomszed face eleinek face-e en leszek am
			e->opposite->face->valid=false;
			delete e->opposite;
			delete e;

			e=preve->next;

			do 
			{
				e->face=preve->face;
				e=e->next;
			} while (e!=nexte);

			//ha preve vagy nexte parhuzamos a mellette levo ellel, akkor nem kell annyira
			vec3 v1,v2;

			v1=work_array[preve->head_vertex]-work_array[preve->prev->head_vertex]; v1.normalize();
			v2=work_array[preve->next->head_vertex]-work_array[preve->head_vertex]; v2.normalize();

			if (cross(v1,v2).squarelength()<0.0001f)
//			if (dot(v1,v2)>0.9999f)//mennyi lenne a jo szam? preve nem kell
			{
				preve->face->edges.erase(preve);
				delete preve;
			}

			v1=work_array[nexte->head_vertex]-work_array[nexte->prev->head_vertex]; v1.normalize();
			v2=work_array[nexte->prev->head_vertex]-work_array[nexte->prev->prev->head_vertex]; v2.normalize();

			if (cross(v1,v2).squarelength()<0.0001f)
//			if (dot(v1,v2)>0.9999f)//mennyi lenne a jo szam? nexte nem kell
			{
				nexte->face->edges.erase(nexte->prev);
				delete nexte->prev;
			}
		}
	}
}

void convex_hull_generator::generate(const convex_hull_desc& hull_desc)
{
	plane_thickness=hull_desc.face_thickness;
    simplify_vertex_array(hull_desc.vertex_array);
	set_big_face();

    gen_face_t* face1=new gen_face_t();
    faces.push_back(face1);
    gen_half_edge_t* edge1=new gen_half_edge_t(face1,0);
    gen_half_edge_t* edge2=new gen_half_edge_t(face1,1);
    gen_half_edge_t* edge3=new gen_half_edge_t(face1,2);
    face1->edges.push_back(edge1);
    face1->edges.push_back(edge2);
    face1->edges.push_back(edge3);

    gen_face_t* face2=new gen_face_t();
    faces.push_back(face2);
    gen_half_edge_t* edge4=new gen_half_edge_t(face2,2);
    gen_half_edge_t* edge5=new gen_half_edge_t(face2,1);
    gen_half_edge_t* edge6=new gen_half_edge_t(face2,0);
    face2->edges.push_back(edge4);
    face2->edges.push_back(edge5);
    face2->edges.push_back(edge6);

    edge1->opposite=edge4;
    edge4->opposite=edge1;
    edge2->opposite=edge6;
    edge6->opposite=edge2;
    edge3->opposite=edge5;
    edge5->opposite=edge3;

	vec3 normal; normal.cross(work_array[2]-work_array[0],work_array[1]-work_array[0]); normal.normalize();

	face1->set(work_array[2],normal);
	face2->set(work_array[2],-normal);

	vector<gen_face_t*> out_face_array;
	vector<gen_half_edge_t*> horizon_edge_array;
	vector<gen_half_edge_t*> new_horizon;
	for (unsigned int vertex_index=3; vertex_index<work_array.size(); ++vertex_index)
	{
		out_face_array.clear();
		gen_half_edge_t* horizon_edge=NULL;
		bool vanjoface=false;

		vec3 act_vertex=work_array[vertex_index];

		for (unsigned int n=0; n<faces.size(); ++n)
		{
			gen_face_t* act_face=faces[n];
			if (!act_face->valid)
				continue;

			if (check_vertex(act_face,act_vertex)==PLANE_OUTSIDE)//ez jo face, ha van horizontedge-e
			{
				vanjoface=true;
				act_face->valid=false; //ha kivul van, akkor ez a face mar annyira nem is kell
				out_face_array.push_back(act_face);

				if (!horizon_edge)
				{
					gen_half_edge_t* edge=act_face->edges.first();

					do 
					{
						gen_face_t* otherface=edge->opposite->face;
						if (check_vertex(otherface,act_vertex)!=PLANE_OUTSIDE)//ha a masik face nem laccik a vertexbol
							horizon_edge=edge;

						edge=edge->next;
					} while (!horizon_edge && edge!=act_face->edges.first());
				}

/*
				for (gen_half_edge_t* edge=act_face->edges.first(); !horizon_edge && edge!=act_face->edges.last(); edge=edge->next)//vegigjarjuk az eleket, hogy talaljunk horizont edge-et
				{
					gen_face_t* otherface=edge->opposite->face;

					if (check_vertex(otherface,act_vertex)!=PLANE_OUTSIDE)//ha a masik face nem laccik a vertexbol
						horizon_edge=edge;
				}
*/
			}
		}

		if (horizon_edge)
		{
			horizon_edge_array.clear();
			new_horizon.clear();
			horizon_edge_array.push_back(horizon_edge);
			calculate_horizon(horizon_edge_array,act_vertex);
			insert_vertex(vertex_index,horizon_edge_array,new_horizon);
			merge_faces(new_horizon,vertex_index);
		}
		else
		{
			assertion(!vanjoface,"ez bizony nem annyira josagos");
			//valid_vertex[vertex_index]=false;
		}
	}
	vector<int > valid_vertex;
	valid_vertex.assign(work_array.size(),0);

	for (unsigned n=0; n<faces.size(); ++n)
	{
		gen_face_t* face=faces[n];

		if (!face->valid)
		{
			faces[n]=faces.back();
			faces.pop_back();
			delete face;
			--n;
			continue;
		}

		gen_half_edge_t* edge=face->edges.first();

		do 
		{
			assertion(edge->opposite->face->valid);
			++valid_vertex[edge->head_vertex];
			edge=edge->next;
		} while (edge!=face->edges.first());
	}

	vector<int> vertex_remap;
	vertex_remap.resize(valid_vertex.size());

	int act_index=0;

	for (unsigned n=0; n<work_array.size(); ++n)
	{
 		assertion(valid_vertex[n]!=1 && valid_vertex[n]!=2);//ha egy vertexet hasznalatban van, akkor legyen mar rajta legalabb 3 face-en

		if (valid_vertex[n])
			vertex_remap[n]=act_index++;
		else
			vertex_remap[n]=-1;
	}

	ch.vertices.reserve(act_index+1);

	int act_adj=0;

	for (unsigned n=0; n<vertex_remap.size(); ++n)
	{
		if (valid_vertex[n])
		{
			ch.vertices.push_back(vertex_data());
			ch.vertices.back().pos=work_array[n];
			ch.vertices.back().adj_index=act_adj;
			act_adj+=valid_vertex[n];
		}
	}

	//sentinel vertex
	ch.vertices.push_back(vertex_data());
	ch.vertices.back().adj_index=act_adj;

	vector<int> act_adj_index;
	act_adj_index.assign(act_index,0);
	ch.vertex_adjacency.resize(act_adj);

	ch.faces.resize(faces.size());

	for (int n=0; n<faces.size(); ++n)
	{
		gen_face_t* face=faces[n];
		gen_half_edge_t* edge=face->edges.first();

		do 
		{
			if (edge->head_vertex==4 || edge->opposite->head_vertex==4)
				strlen("");
			int act_vertex=vertex_remap[edge->head_vertex];
			int adj_vertex=vertex_remap[edge->opposite->head_vertex];

			if (adj_vertex==-1)
				strlen("");

			ch.vertex_adjacency[ch.vertices[act_vertex].adj_index+act_adj_index[act_vertex]]=adj_vertex;
			++act_adj_index[act_vertex];

			edge=edge->next;
		} while (edge!=face->edges.first());
	}

	for (int n=0; n<act_index; ++n)
	{
		assertion(act_adj_index[n]==ch.vertices[n+1].adj_index-ch.vertices[n].adj_index);
	}
}

convex_hull_generator::~convex_hull_generator()
{
	for (unsigned int n=0; n<faces.size(); ++n)
		delete faces[n];
}

//todo: a 4. vertexnek messze kell lenni az elso ket haromszog sikjatol, mert ha veletlenul rajta van, akkor bukta
