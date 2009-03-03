#ifndef _hgridbroadphase_h_
#define _hgridbroadphase_h_

#include "broadphaseobject.h"
#include "broadphasepair.h"
#include "containers/listallocator.h"
#include "threading/mutex.h"

#define NUM_BUCKETS 16384
#define SPHERE_TO_CELL_RATIO  0.5f
#define CELL_TO_CELL_RATIO 1.5f
#define  MIN_CELL_SIZE 1.0f
#define HGRID_MAX_LEVELS 31

struct hgridobject:public broadphaseobject
{
    hgridobject(void* i_userdata, const aabb_t& i_boundingworld, uint32 i_is_static);
    hgridobject* next_object;
    hgridobject* prev_object;
    vec3 pos;
    f32 radius;
    int bucket;
    int level;
};



struct hgridbroadphase
{
    hgridbroadphase();
	~hgridbroadphase()
	{
	}

    broadphaseobject* create_object(void* i_userdata, const aabb_t& i_bounding, uint32 i_static);
    void release_object(broadphaseobject*);
    void update_object(broadphaseobject*);

    void update() ;

    int get_broadphasepairnum() const ;
    broadphasepair* get_pairs();



    unsigned occupied_levels_mask;

    int objects_at_level[HGRID_MAX_LEVELS];
    f32 oo_cell_size[HGRID_MAX_LEVELS];
    hgridobject* object_bucket[HGRID_MAX_LEVELS][NUM_BUCKETS];
    int time_stamp[HGRID_MAX_LEVELS][NUM_BUCKETS];

    int tick;

    list_allocator<sizeof(hgridobject)> static_list;
    list_allocator<sizeof(hgridobject)> dynamic_list;

//		vector<broadphasepair> pair_array;
    broadphasepair pair_array[65536];
    LONG pair_num;
    mutex hgrid_mutex;

};
#endif//_hgridbroadphase_h_
