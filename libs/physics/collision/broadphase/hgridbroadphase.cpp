#include "hgridbroadphase.h"
#include "math/geometry/intersection.h"
#include "threading/taskmanager.h"
#include "physics/system/physicssystem.h"

#define h1  2376512323u
#define h2  3625334849u
#define h3  3407524639u
MLINLINE unsigned compute_hash_bucket_index(int x, int y, int z)
{
    return unsigned (h1 * x + h2 * y + h3 * z) & (NUM_BUCKETS-1);
}

MLINLINE unsigned compute_hash_bucket_index(float x, float y, float z)
{
	return unsigned (h1 * floorf(x) + h2 * floorf(y) + h3 * floorf(z)) & (NUM_BUCKETS-1);
}

hgridobject::hgridobject(void* i_userdata, const aabb_t& i_bounding, uint32 i_static):
broadphaseobject(i_userdata,i_bounding,i_static)
{
    this->pos=i_bounding.get_center();
    vec3 extent=i_bounding.get_extent();
    this->radius=max(extent.x,max(extent.y,extent.z));
}

hgridbroadphase::hgridbroadphase()
{
    f32 size=MIN_CELL_SIZE;
    for (int n=0; n<HGRID_MAX_LEVELS;++n)
    {
        size*=CELL_TO_CELL_RATIO;

        objects_at_level[n]=0;
        oo_cell_size[n]=1/size;

        for (uint32 m=0; m<NUM_BUCKETS;++m)
        {
            object_bucket[n][m]=NULL;
            time_stamp[n][m]=0;
        }
    }

    occupied_levels_mask=0;
    tick=0;
    pair_num=0;
}

void add_object_to_hgrid(hgridbroadphase* hgrid, hgridobject *obj)
{
    int level;
    f32 diameter = 2.0f * obj->radius;

    for (level = 0 ; SPHERE_TO_CELL_RATIO < diameter*hgrid->oo_cell_size[level] ; level++){}

    assertion(level < HGRID_MAX_LEVELS);

    const f32 oosize=hgrid->oo_cell_size[level];
//    int bucket = compute_hash_bucket_index((int)(obj->pos.x *oosize), (int)(obj->pos.y *oosize), (int)(obj->pos.z *oosize));
	int bucket = compute_hash_bucket_index((int)(obj->pos.x *oosize), (int)(obj->pos.y *oosize), (int)(obj->pos.z *oosize));
    obj->bucket= bucket;
    obj->level = level;
    obj->next_object= hgrid->object_bucket[level][bucket];
    obj->prev_object=NULL;

    if (hgrid->object_bucket[level][bucket])
        hgrid->object_bucket[level][bucket]->prev_object=obj;

    hgrid->object_bucket[level][bucket] = obj;
    hgrid->objects_at_level[level]++;
    hgrid->occupied_levels_mask |= (1 << level);
}

void remove_object_from_hgrid(hgridbroadphase* hgrid, hgridobject *obj)
{
    int level=obj->level;
    if (--hgrid->objects_at_level[level] == 0)
        hgrid->occupied_levels_mask &= ~(1 << level);

    int bucket= obj->bucket;
    if (obj->next_object) obj->next_object->prev_object=obj->prev_object;
    if (obj->prev_object) obj->prev_object->next_object=obj->next_object;

    if (hgrid->object_bucket[level][bucket] == obj)
        hgrid->object_bucket[level][bucket] = obj->next_object;

    return;
/*
    hgridobject *p = this->object_bucket[level][bucket];

    while (p)
    {
        hgridobject*q = p;
        p = p->next_object;

        if (p == obj)
        {
            q->next_object= p->next_object;
            return;
        }
    }
    assertion(0);
*/
}


void check_obj_against_grid(hgridbroadphase* hgrid, hgridobject* obj) //csak teljes teszt eseten hasznalhato
{
    const int startLevel = obj->level;
    int loccupiedLevelsMask = hgrid->occupied_levels_mask>> startLevel;
    const vec3 pos = obj->pos;
    const vec3 halfextent=obj->bounding_world.get_extent();

    for (int level = startLevel; level < HGRID_MAX_LEVELS && loccupiedLevelsMask; loccupiedLevelsMask >>= 1, level++)
    {
        if ((loccupiedLevelsMask & 1) == 0)
            continue;

        const f32 ooSize = hgrid->oo_cell_size[level];
//			f32 ooSize = 1.0f / this->cell_size[level];

        const int xs = (int)((pos.x-halfextent.x)*ooSize-SPHERE_TO_CELL_RATIO);
        const int ys = (int)((pos.y-halfextent.y)*ooSize-SPHERE_TO_CELL_RATIO);
        const int zs = (int)((pos.z-halfextent.z)*ooSize-SPHERE_TO_CELL_RATIO);
        const int xe = (int)((pos.x+halfextent.x)*ooSize+SPHERE_TO_CELL_RATIO);
        const int ye = (int)((pos.y+halfextent.y)*ooSize+SPHERE_TO_CELL_RATIO);
        const int ze = (int)((pos.z+halfextent.z)*ooSize+SPHERE_TO_CELL_RATIO);

        for (int x = xs; x <= xe; ++x)
        {
            for (int y = ys; y <= ye; ++y)
            {
                for (int z= zs; z<=ze; ++z)
                {
                    int bucket = compute_hash_bucket_index(x, y, z);
                    hgridobject* p = hgrid->object_bucket[level][bucket];
                    while (p)
                    {
                        if (level!=startLevel || p <obj)
                        {
                            if (aabb_aabb_intersect(obj->bounding_world,p->bounding_world))
                            {
//									this->checkobj_mutex.lock();
                                LONG index=InterlockedExchangeAdd(&hgrid->pair_num,1);
                                new (hgrid->pair_array+index) broadphasepair(obj,p);
//									this->checkobj_mutex.unlock();
                            }
                        }
                        p = p->next_object;
                    }
                }
            }
        }
    }
}

void move_object(hgridbroadphase* hgrid, hgridobject* obj)
{
    obj->pos=obj->bounding_world.get_center();

    const int level=obj->level;
    const f32 oosize= hgrid->oo_cell_size[level];
    int bucket = compute_hash_bucket_index((int)(obj->pos.x *oosize), (int)(obj->pos.y *oosize), (int)(obj->pos.z *oosize));

    if (bucket==obj->bucket)
        return;

    remove_object_from_hgrid(hgrid, obj);
    obj->bucket= bucket;

    obj->next_object = hgrid->object_bucket[level][bucket];
    obj->prev_object=NULL;

    if (hgrid->object_bucket[level][bucket])
        hgrid->object_bucket[level][bucket]->prev_object=obj;

    hgrid->object_bucket[level][bucket] = obj;

    hgrid->objects_at_level[level]++;
    hgrid->occupied_levels_mask |= (1 << level);
}


broadphaseobject* hgridbroadphase::create_object(void* i_userdata, const aabb_t& i_bounding, uint32 i_static)
{
    hgridobject* newobj=(hgridobject*)(i_static ? this->static_list.allocate_place() : this->dynamic_list.allocate_place());
    new (newobj) hgridobject(i_userdata,i_bounding,i_static);
    add_object_to_hgrid(this, newobj);

    return newobj;
}

void hgridbroadphase::release_object(broadphaseobject* i_object)
{
    hgridobject* obj=(hgridobject*)i_object;

    remove_object_from_hgrid(this, obj);

    if (i_object->is_static)
        this->static_list.deallocate_place(obj);
    else
        this->dynamic_list.deallocate_place(obj);

	obj->~hgridobject();

}


struct hgrid_update_process
{
    hgrid_update_process(hgridbroadphase* i_broadphase,hgridobject** i_buf):
    broad_phase(i_broadphase),
    buf(i_buf)
    {
    }

    void operator()(unsigned i_start,unsigned i_num) const
    {
        uint32 end=i_start+i_num;
        for (uint32 n=i_start; n<end; ++n)
        {
            check_obj_against_grid(broad_phase, buf[n]);
        }
    }

    hgridbroadphase* broad_phase;
    hgridobject** buf;
};


void hgridbroadphase::update()
{
    this->pair_num=0;

    list_allocator<sizeof(hgridobject)>::iterator it;

    if (!physicssystem::ptr->parallel_broadphase)
    {
        for (it=this->dynamic_list.begin(); it!=this->dynamic_list.end(); ++it)
            move_object(this, (hgridobject*)*it);

        for (it=this->dynamic_list.begin(); it!=this->dynamic_list.end(); ++it)
            check_obj_against_grid(this, (hgridobject*)*it);
    }
    else
    {
        unsigned obj_count=this->dynamic_list.size();

#ifdef _MSC_VER
        hgridobject** obj_array=(hgridobject**)_alloca(obj_count*sizeof(hgridobject*));
#else
        hgridobject* obj_array[obj_count];
#endif

        list_allocator<sizeof(hgridobject)>::iterator it=this->dynamic_list.begin();

        for (unsigned n=0; n<obj_count;++n,++it)
        {
            obj_array[n]=(hgridobject*)*it;
            move_object(this, obj_array[n]);
        }

        taskmanager_t::ptr->process_buffer(obj_count,10,hgrid_update_process(this,obj_array));
    }
}

void hgridbroadphase::update_object(broadphaseobject* i_object)
{
    hgridobject* obj=(hgridobject*)i_object;
    move_object(this, obj);
}

int hgridbroadphase::get_broadphasepairnum() const
{
    return (int)this->pair_num;
}

broadphasepair* hgridbroadphase::get_pairs()
{
    return this->pair_array;
}

#if 0
void HGridBroadPhase::GetObjectsInRange(Dyn::vector<BroadPhaseObject*>& o_ObjList, const AABB& i_Range)
{
    const int startLevel = 0;
    int loccupiedLevelsMask = m_OccupiedLevelsMask;;
    const Vector3 pos = i_Range.GetCenter();
    const Vector3 halfextent=i_Range.GetExtent();

    ++m_Tick;

    for (int level = startLevel; level < HGRID_MAX_LEVELS && loccupiedLevelsMask; loccupiedLevelsMask >>= 1, level++)
    {
        if ((loccupiedLevelsMask & 1) == 0)
            continue;

        f32 ooSize = 1.0f / m_CellSize[level];

        const int xs = (int)((pos.x-halfextent.x)*ooSize-SPHERE_TO_CELL_RATIO);
        const int ys = (int)((pos.y-halfextent.y)*ooSize-SPHERE_TO_CELL_RATIO);
        const int zs = (int)((pos.z-halfextent.z)*ooSize-SPHERE_TO_CELL_RATIO);
        const int xe = (int)((pos.x+halfextent.x)*ooSize+SPHERE_TO_CELL_RATIO);
        const int ye = (int)((pos.y+halfextent.y)*ooSize+SPHERE_TO_CELL_RATIO);
        const int ze = (int)((pos.z+halfextent.z)*ooSize+SPHERE_TO_CELL_RATIO);

        for (int x = xs; x <= xe; ++x)
        {
            for (int y = ys; y <= ye; ++y)
            {
                for (int z= zs; z<=ze; ++z)
                {
                    int bucket = ComputeHashBucketIndex(x, y, z);

                    if (m_TimeStamp[level][bucket]==m_Tick)
                        continue;

                    m_TimeStamp[level][bucket]=m_Tick;
                    HGridObject* p=m_ObjectBucket[level][bucket];
                    while (p)
                    {
                        if (BoxBoxIntersection(i_Range,p->BoundingWorld()))
                        {
                            o_ObjList.push_back(p);
                        }

                        p = p->m_NextObject;
                    }
                }
            }
        }
    }
}
#endif
