#include <algorithm>
#include "convexhullgeneration.h"
#include "math\sorting.h"
#include "math/aabox.h"
#include "containers\listallocator.h"
#include "render\rendersystem.h"

const int PLANE_OUTSIDE=1;
const int PLANE_INSIDE=-1;
const int PLANE_ON=0;

list_allocator<sizeof(gen_half_edge_t),2048> edge_allocator;

void* gen_half_edge_t::operator new(size_t size)
{
	return edge_allocator.allocate_place();
}

void gen_half_edge_t::operator delete(void* edge)
{
	return edge_allocator.deallocate_place((gen_half_edge_t*)edge);
}


list_allocator<sizeof(gen_face_t),2048> face_allocator;

#define CHECK_CONSISTENCY

#ifdef CHECK_CONSISTENCY
#define CONSISTENCY_ASSERT(_COND_) assertion(_COND_)
#else
#define CONSISTENCY_ASSERT
#endif




void* gen_face_t::operator new(size_t size)
{
	return face_allocator.allocate_place();
}

void gen_face_t::operator delete(void* face)
{
	face_allocator.deallocate_place((gen_face_t*)face);
}











void convex_hull_generator::draw(const mtx4x3& mtx)
{
	int face_num=0;
	for (unsigned n=0; n<faces.size(); ++n)
	{
		if (!faces[n]->valid)
			continue;

		++face_num;

		gen_half_edge_t* e=faces[n]->edges.first();
		CONSISTENCY_ASSERT(e!=NULL);

		do 
		{
			if (e->prev->head_vertex>e->head_vertex)
			{
				vec3 start;
				start.x=(f32)work_array[e->prev->head_vertex].x;
				start.y=(f32)work_array[e->prev->head_vertex].y;
				start.z=(f32)work_array[e->prev->head_vertex].z;
				vec3 end;
				end.x=(f32)work_array[e->head_vertex].x;
				end.y=(f32)work_array[e->head_vertex].y;
				end.z=(f32)work_array[e->head_vertex].z;

				start=mtx.transform(start);
				end=mtx.transform(end);
				rendersystem::ptr->draw_line(start,color_r8g8b8a8(255,255,255,255),
											 end,color_r8g8b8a8(255,255,255,255));
			}
			e=e->next;
		} while (e!=faces[n]->edges.first());
	}

	vec3 center;
	center.x=(f32)work_array[vertex_index].x;
	center.y=(f32)work_array[vertex_index].y;
	center.z=(f32)work_array[vertex_index].z;
	f32 radius=0.1f;
	rendersystem::ptr->draw_circle(center,radius);

	char str[256];
	sprintf(str,"facenum:%d",face_num);
	rendersystem::ptr->draw_text(10,200,color_f(1,1,1,1),str);
	sprintf(str,"vertexi:%d",vertex_index);
	rendersystem::ptr->draw_text(10,220,color_f(1,1,1,1),str);
}


template<int coord=0>
struct vec_sort_t
{
	bool operator()(const dvec3& v1, const dvec3& v2) const
	{
		return v1[coord]<v2[coord];
	}
	static bool less(const dvec3& v1, const dvec3& v2)
	{
		return v1[coord]<v2[coord];
	}
	static bool eq(const dvec3& v1, const dvec3& v2)
	{
		return v1[coord]==v2[coord];
	}
};

struct sort_array:dvec3
{
	int volt;
	sort_array():volt(0){}
	sort_array(const dvec3& v):volt(0){x=(v.x),y=(v.y),z=(v.z);}
};

void convex_hull_generator::simplify_vertex_array(const vector<vec3>& src_array)
{
	if (vertex_min_dist<0)
	{
		dvec3 bmin,bmax;
		bmin.set(FLT_MAX,FLT_MAX,FLT_MAX);
		bmax.set(-FLT_MAX,-FLT_MAX,-FLT_MAX);

		for (unsigned n=0; n<src_array.size(); ++n)
		{
			for (int m=0; m<3; ++m)
			{
				if (src_array[n][m]<bmin[m])
				{
					bmin[m]=src_array[n][m];
				}
				if (src_array[n][m]>bmax[m])
				{
					bmax[m]=src_array[n][m];
				}
			}
		}

		dvec3 extent=bmax-bmin;
		double* maxptr=max_elem(&extent[0],&extent[3]);

		vertex_min_dist=*maxptr*(-vertex_min_dist);
	}
    vector<sort_array> tmp_array; tmp_array.resize(src_array.size());
	
	for (unsigned n=0; n<src_array.size(); ++n)
		tmp_array[n]=(dvec3)src_array[n];
    intro_sort(tmp_array.begin(), tmp_array.size(),vec_sort_t<0>());
//	std::sort(tmp_array.begin(), tmp_array.end(),vec_sort_t<0>());

    for (unsigned n=0; n<tmp_array.size(); ++n)
    {
		if (tmp_array[n].volt)
			continue;

		dvec3 sum; sum=tmp_array[n];
		double num=1;

		for (unsigned m=n+1; m<tmp_array.size(); ++m)
		{
			if (tmp_array[m].volt)
				continue;

			dvec3 avg=sum/num;
			if (fabs(tmp_array[m].x-avg.x)>vertex_min_dist)
				break;

			if ((tmp_array[m]-avg).squarelength()<=vertex_min_dist*vertex_min_dist)
			{
				tmp_array[m].volt=1;
				sum+=tmp_array[m];
				++num;
			}
		}
		work_array.push_back(sum/num);
	}
}

void convex_hull_generator::set_big_face()
{
    int face[3];
    dvec3 bmin,bmax;
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

    dvec3 extent=bmax-bmin;
	double* minptr=max_elem(&extent[0],&extent[3]);
	int	axis=minptr-&extent[0];

	face[0]=minindex[axis];
	face[1]=maxindex[axis];

	dvec3 segmentdir=work_array[face[1]]-work_array[face[0]];
	segmentdir.normalize();
	double maxdist=-FLT_MAX;
	for (unsigned int n=0; n<work_array.size(); ++n)
    {
        dvec3 v=work_array[n]-work_array[face[0]];
        double actdist=(v-dot(v,segmentdir)*segmentdir).squarelength();

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

	dplane_t plane;
	plane.set(work_array[0],cross(work_array[2]-work_array[0],work_array[1]-work_array[0]));

	//most megkeressuk a legtavolabbi vertexet az elso face-tol, es az lesz a 4. vertex;

	double max_dist=-FLT_MAX;
	int max_index;
	for (unsigned n=3; n<work_array.size(); ++n)
	{
		double act_dist=fabs(plane.get_distance(work_array[n]));
		if (act_dist>max_dist)
		{
			max_dist=act_dist;
			max_index=n;
		}
	}

	swap(work_array[3],work_array[max_index]);
}

bool convex_hull_generator::is_horizon_edge(gen_half_edge_t* edge, const dvec3& ref_vertex) //sajat face out kell hogy legyen
{
	return (!edge->face->valid && edge->opposite->face->valid);
//	return (check_vertex(edge->opposite->face,ref_vertex)!=PLANE_OUTSIDE);
}

gen_half_edge_t* convex_hull_generator::find_next_horizon_edge(gen_half_edge_t* edge, const dvec3& ref_vertex)
{
	CONSISTENCY_ASSERT(!edge->next->face->valid);
	while (!is_horizon_edge(edge->next,ref_vertex))
	{
		CONSISTENCY_ASSERT(!edge->next->face->valid && !edge->next->opposite->face->valid);
		edge=edge->next->opposite;
	}

	CONSISTENCY_ASSERT(!edge->next->face->valid && edge->next->opposite->face->valid);

	return edge->next;
}

void convex_hull_generator::calculate_horizon(vector<gen_half_edge_t*>& edge_array, const dvec3& ref_vertex)
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

void convex_hull_generator::insert_vertex(const vector<gen_half_edge_t*>& edge_array)
{
	CONSISTENCY_ASSERT(edge_array.size()>2);
	//megkeressuk a faszomsagot
	int edge_count=edge_array.size();
	gen_face_t* f=edge_array[edge_count-1]->opposite->face;
	int edge_index=0;
	for (int n=0; n<edge_count-1; ++n)
	{
		if (edge_array[n]->opposite->face==f)
			++edge_index;
		else
			break;
	}


	first_face=faces.size();
	int fasszam=0;



	for (int nnn=0; nnn<edge_count;)
	{
		gen_face_t* face=new gen_face_t();
		faces.push_back(face);
		++fasszam;
		gen_half_edge_t* firste=edge_array[(nnn+edge_index) % edge_count];
		int v1=firste->opposite->head_vertex;
		gen_half_edge_t* edge1=new gen_half_edge_t(face,v1);
		face->edges.push_back(edge1);

		int fostalicska;

		if (!triangle_output && check_vertex(firste->opposite->face,work_array[vertex_index])==PLANE_ON)
			fostalicska=-1;
		else
			fostalicska=1;

		for (; fostalicska && nnn<edge_count && edge_array[(nnn+edge_index) % edge_count]->opposite->face==firste->opposite->face; ++nnn,--fostalicska)
		{
			gen_half_edge_t* edge2=new gen_half_edge_t(face,edge_array[(nnn+edge_index) % edge_count]->head_vertex);
			face->edges.push_back(edge2);
			edge2->opposite=edge_array[(nnn+edge_index) % edge_count]->opposite;
			edge2->opposite->opposite=edge2;
			new_horizon.push_back(edge2);

		}
		int v2=edge_array[(nnn-1+edge_index) % edge_count]->head_vertex;
		int v3=vertex_index;

		gen_half_edge_t* edge3=new gen_half_edge_t(face,v3);

		face->edges.push_back(edge3);
		face->set(work_array[v1],cross(work_array[v3]-work_array[v1],work_array[v2]-work_array[v1]));
	}

	for (int n=0; n<fasszam; ++n)
	{
		int prevface=((n+fasszam-1) %fasszam)+first_face;
		int actface=n+first_face;
		int nextface=((n+1) %fasszam)+first_face;

		faces[actface]->edges.first()->opposite=faces[prevface]->edges.last();
		faces[actface]->edges.last()->opposite=faces[nextface]->edges.first();
	}
}

convex_hull_t generate_convex_hull(const convex_hull_desc& hull_desc)
{
	convex_hull_generator chg;
	chg.init(hull_desc);
	while (!chg.generate());
	chg.get_result();

	return chg.ch;
}

double g_dist;
int convex_hull_generator::check_vertex(gen_face_t* face, const dvec3& v)
{
	double dist=face->get_distance(v);
	g_dist=dist;
	if (dist>plane_thickness)
		return PLANE_OUTSIDE;
	if (dist<-plane_thickness)
		return PLANE_INSIDE;
	else
		return PLANE_ON;
}

int get_vertexedge_count(gen_half_edge_t* startedge)
{
	int count=0;

	gen_half_edge_t* e=startedge;

	do 
	{
		++count;
		e=e->opposite->prev;
	} while (e!=startedge);

	return count;
}

void convex_hull_generator::merge_faces()
{
	dvec3 v=work_array[vertex_index];
	vector<gen_half_edge_t*> nevevan;
	for (unsigned n=0; n<new_horizon.size(); ++n)
	{
		gen_half_edge_t* firste=new_horizon[n];
		int firsteindex=n;
		if (check_vertex(firste->opposite->face,v)==PLANE_ON) //fos az el, meg kell szuntetni
		{
			gen_half_edge_t* preve=firste->prev;
			gen_half_edge_t* prevnext=firste->opposite->next;

			for (; n<new_horizon.size() && new_horizon[n]->opposite->face==firste->opposite->face; ++n){}
			--n;
			gen_half_edge_t* laste=new_horizon[n];
			int lasteindex=n;

			gen_half_edge_t* nexte=laste->next;
			preve->next=firste->opposite->next;
			nexte->prev=laste->opposite->prev;
			firste->opposite->next->prev=preve;
			laste->opposite->prev->next=nexte;
			//most mar e-re es e->opposite-ra senki sem hivatkozik
			firste->opposite->face->edges.clear();

			//a szomszed face eleinek face-e en leszek am
			firste->opposite->face->valid=false;

			for (int m=firsteindex; m<=lasteindex; ++m)
			{
				delete new_horizon[m]->opposite;
				delete new_horizon[m];
			}


			gen_half_edge_t* e=preve->next;

			do 
			{
				e->face=preve->face;
				e=e->next;
			} while (e!=nexte);
		}
	}


	for (unsigned n=first_face; n<faces.size(); ++n)
	{
		if (!faces[n]->valid)
			continue;
		gen_half_edge_t* e=faces[n]->edges.first();

		do
		{
			if (get_vertexedge_count(e)==2)
			{
				gen_half_edge_t* nexte=e->next;
#if 0//def CHECK_CONSISTENCY
				dvec3 v1,v2;
				v1=work_array[nexte->head_vertex]-work_array[e->head_vertex]; v1.normalize();
				v2=work_array[e->head_vertex]-work_array[e->prev->head_vertex]; v2.normalize();
				double l=cross(v1,v2).squarelength();
				CONSISTENCY_ASSERT(l<0.0001);
#endif
//				nevevan.push_back(nexte);

				gen_half_edge_t* prev=nexte->prev;
				gen_half_edge_t* oppprev=nexte->opposite;

				if (prev==oppprev)
					continue;
				nexte->face->edges.erase(prev);
				gen_half_edge_t* new_opposite=oppprev->next;
				new_opposite->opposite=nexte;
				nexte->opposite=new_opposite;

				delete prev;
				new_opposite->face->edges.erase(oppprev);
				delete oppprev;

				CONSISTENCY_ASSERT(nexte->face->valid);
				CONSISTENCY_ASSERT(new_opposite->face->valid);
				break;
			}

			e=e->next;
		}
		while (e!=faces[n]->edges.first());
	}
}

void convex_hull_generator::init(const convex_hull_desc& hull_desc)
{
	plane_thickness=hull_desc.face_thickness;
	vertex_min_dist=hull_desc.vertex_min_dist;
	triangle_output=hull_desc.triangle_output;
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

	dvec3 normal; normal.cross(work_array[2]-work_array[0],work_array[1]-work_array[0]); normal.normalize();

	face1->set(work_array[2],normal);
	face2->set(work_array[2],-normal);

	vertex_index=3;

	state=0;
}

void convex_hull_generator::do_all(const convex_hull_desc& hull_desc)
{
	clear();
	init(hull_desc);

	while(!generate());
	get_result();
}

double convex_hull_generator::get_signed_volume(gen_face_t* face, const vec3& point)
{
	return 0;
}

bool convex_hull_generator::generate()
{
	vector<gen_half_edge_t*> horizon_edge_array;

	if (!state)
	{
		out_face_array.clear();
		gen_half_edge_t* horizon_edge=NULL;
		bool vanjoface=false;

		dvec3 act_vertex=work_array[vertex_index];

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
			}
		}

		if (horizon_edge)
		{
			horizon_edge_array.clear();
			new_horizon.clear();
			horizon_edge_array.push_back(horizon_edge);
			calculate_horizon(horizon_edge_array,act_vertex);

#ifdef CHECK_CONSISTENCY
			for (unsigned n=0; n<horizon_edge_array.size(); ++n)
			{
				gen_half_edge_t* e=horizon_edge_array[n];
				CONSISTENCY_ASSERT(!e->face->valid && e->opposite->face->valid);
			}
#endif
			insert_vertex(horizon_edge_array);

			if (!triangle_output)
			{
				state=1;
				--vertex_index;
			}
//				merge_faces();
		}
		else
		{
			CONSISTENCY_ASSERT(!vanjoface);//,"ez bizony nem annyira josagos");
			//valid_vertex[vertex_index]=false;
		}

		++vertex_index;
	}
	else
	{
		merge_faces();
		++vertex_index;
		state=0;
	}


	return (vertex_index==work_array.size() && state==0);
}

void convex_hull_generator::get_result()
{
	ch.clear();
	vector<int > valid_vertex;
	valid_vertex.assign(work_array.size(),0);

	//toroljuk a nem valid face-eket
	int face_index=0;
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

		face->face_index=face_index++;

		gen_half_edge_t* edge=face->edges.first();

		do 
		{
			CONSISTENCY_ASSERT(edge->opposite->face->valid);
			++valid_vertex[edge->head_vertex];
			edge=edge->next;
		} while (edge!=face->edges.first());
	}

	//elkeszitjuk a vertexremap tablat
	vector<int> vertex_remap;
	vertex_remap.resize(valid_vertex.size());

	int act_index=0;

	for (unsigned n=0; n<work_array.size(); ++n)
	{
		CONSISTENCY_ASSERT(valid_vertex[n]!=1 && valid_vertex[n]!=2);//ha egy vertexet hasznalatban van, akkor legyen mar rajta legalabb 3 face-en

		if (valid_vertex[n])
			vertex_remap[n]=act_index++;
		else
			vertex_remap[n]=-1;
	}


	list_allocator<sizeof(gen_half_edge_t),2048>::iterator it;
	ch.edges.resize(edge_allocator.size());
	{
	int edge_index=0;
	for (it=edge_allocator.begin(); it!=edge_allocator.end(); ++it)
	{
		gen_half_edge_t& edge=*(gen_half_edge_t*)*it;

		if (edge.edge_index==-1)
		{
			CONSISTENCY_ASSERT(edge.opposite->edge_index==-1);
			edge.edge_index=edge_index;
			edge_index++;
			edge.opposite->edge_index=edge_index;
			edge_index++;
		}
	}
	}

	for (it=edge_allocator.begin(); it!=edge_allocator.end(); ++it)
	{
		gen_half_edge_t& edge=*(gen_half_edge_t*)*it;
		edge_data& act_edge_data=ch.edges[edge.edge_index];
		act_edge_data.head_vertex_index=vertex_remap[edge.head_vertex];
		act_edge_data.next_edge=edge.next->edge_index;
		act_edge_data.prev_edge=edge.prev->edge_index;
		act_edge_data.opp_edge=edge.opposite->edge_index;
		act_edge_data.face_index=edge.face->face_index;
	}

	ch.vertices.reserve(act_index);
	aabb_t aabb;aabb.min.set(FLT_MAX,FLT_MAX,FLT_MAX); aabb.max=-aabb.min;
	for (unsigned n=0; n<vertex_remap.size(); ++n)
	{
		if (valid_vertex[n])
		{
			ch.vertices.push_back(vertex_data());
			ch.vertices.back().x=(f32)work_array[n].x;
			ch.vertices.back().y=(f32)work_array[n].y;
			ch.vertices.back().z=(f32)work_array[n].z;
			aabb.extend(ch.vertices.back());
		}
	}
	ch.center=.5f*(aabb.min+aabb.max);
	ch.half_extent=.5f*(aabb.max-aabb.min);

	ch.faces.resize(faces.size());
	for (unsigned n=0; n<faces.size(); ++n)
	{
		gen_face_t* face=faces[n];
		ch.faces[n].normal.x=(f32)face->normal.x;
		ch.faces[n].normal.y=(f32)face->normal.y;
		ch.faces[n].normal.z=(f32)face->normal.z;

/*egyelore kiszedve
		gen_half_edge_t* edge=face->edges.first();
		ch.faces[n].adj_edge_index=edge->edge_index;

		do 
		{
			ch.vertices[vertex_remap[edge->head_vertex]].adj_edge_index=edge->edge_index;
			edge=edge->next;
		} while (edge!=face->edges.first());
*/
	}
}

void convex_hull_generator::clear()
{
	for (unsigned int n=0; n<faces.size(); ++n)
		delete faces[n];

	faces.clear();
	work_array.clear();
}

convex_hull_generator::~convex_hull_generator()
{
	clear();
}
