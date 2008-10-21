#include "hgridbroadphase.h"
#include "math/intersection.h"

namespace physics
{
	hgridobject::hgridobject(void* i_userdata, const math::aabb& i_bounding, uint32 i_static):
	broadphaseobject(i_userdata,i_bounding,i_static)
	{
		this->pos=i_bounding.get_center();
		math::vec3 extent=i_bounding.get_extent();
		this->radius=math::Max(extent.x,math::Max(extent.y,extent.z));
	}

	hgridbroadphase::hgridbroadphase()
	{
		float size=MIN_CELL_SIZE;
		for (int n=0; n<HGRID_MAX_LEVELS;++n)
		{
			size*=CELL_TO_CELL_RATIO;

			objects_at_level[n]=0;
			cell_size[n]=size;

			for (int m=0; m<NUM_BUCKETS;++m)
			{
				object_bucket[n][m]=NULL;
				time_stamp[n][m]=0;
			}
		}

		occupied_levels_mask=0;
		tick=0;
	}

	void hgridbroadphase::add_object_to_hgrid(hgridobject *obj)
	{
		int level;
		float diameter = 2.0f * obj->radius;

		for (level = 0; cell_size[level]* SPHERE_TO_CELL_RATIO < diameter; level++);

		utils::assertion(level < HGRID_MAX_LEVELS);

		float size=cell_size[level];
		int bucket = compute_hash_bucket_index((int)(obj->pos.x /	 size), (int)(obj->pos.y / size), (int)(obj->pos.z / size));
		obj->bucket= bucket;
		obj->level = level;
		obj->next_object= this->object_bucket[level][bucket];
		this->object_bucket[level][bucket] = obj;
		this->objects_at_level[level]++;
		this->occupied_levels_mask |= (1 << level);
	}

	void hgridbroadphase::remove_object_from_hgrid(hgridobject *obj)
	{
		int level=obj->level;
		if (--this->objects_at_level[level] == 0)
			this->occupied_levels_mask &= ~(1 << level);

		int bucket= obj->bucket;
		hgridobject *p = this->object_bucket[level][bucket];

		if (p == obj)
		{
			this->object_bucket[level][bucket] = obj->next_object;
			return;
		}

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

		utils::assertion(0);
	}


	void hgridbroadphase::check_obj_against_grid(hgridobject* obj) //csak teljes teszt eseten hasznalhato
	{
		const int startLevel = obj->level;
		int loccupiedLevelsMask = this->occupied_levels_mask>> startLevel;
		const math::vec3 pos = obj->pos;
		const math::vec3 halfextent=obj->bounding_world.get_extent();

		for (int level = startLevel; level < HGRID_MAX_LEVELS && loccupiedLevelsMask; loccupiedLevelsMask >>= 1, level++)
		{
			if ((loccupiedLevelsMask & 1) == 0)
				continue;

			float ooSize = 1.0f / this->cell_size[level];

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
						hgridobject* p = this->object_bucket[level][bucket];
						while (p)
						{
							if (level!=startLevel || p <obj)
							{
								if (math::aabb_aabb_intersect(obj->bounding_world,p->bounding_world))
									this->pair_array.push_back(broadphasepair(obj,p));
							}
							p = p->next_object;
						}
					}
				}
			}
		}
	}

	void hgridbroadphase::move_object(hgridobject* obj)
	{
		obj->pos=obj->bounding_world.get_center();

		const int level=obj->level;
		float size= this->cell_size[level];
		int bucket = compute_hash_bucket_index((int)(obj->pos.x / size), (int)(obj->pos.y / size), (int)(obj->pos.z / size));

		if (bucket==obj->bucket)
			return;

		this->remove_object_from_hgrid(obj);
		obj->bucket= bucket;

		obj->next_object = this->object_bucket[level][bucket];
		this->object_bucket[level][bucket] = obj;

		this->objects_at_level[level]++;
		this->occupied_levels_mask |= (1 << level);
	}


	broadphaseobject* hgridbroadphase::create_object(void* i_userdata, const math::aabb& i_bounding, uint32 i_static)
	{
		hgridobject* newobj= i_static ? this->static_list.allocate_place() : this->dynamic_list.allocate_place();
		new (newobj) hgridobject(i_userdata,i_bounding,i_static);
		this->add_object_to_hgrid(newobj);

		return newobj;
	}

	void hgridbroadphase::release_object(broadphaseobject* i_object)
	{
		hgridobject* obj=(hgridobject*)i_object;

		this->remove_object_from_hgrid(obj);

		if (i_object->is_static)
			this->static_list.deallocate(obj);
		else
			this->dynamic_list.deallocate(obj);
			
	}


	void hgridbroadphase::update()
	{
		this->pair_array.clear();

		ctr::listallocator<hgridobject>::iterator it;

		for (it=this->dynamic_list.begin(); it!=this->dynamic_list.end(); ++it)
			this->move_object(*it);

		for (it=this->dynamic_list.begin(); it!=this->dynamic_list.end(); ++it)
			this->check_obj_against_grid(*it);
	}

	void hgridbroadphase::update_object(broadphaseobject* i_object)
	{
		hgridobject* obj=(hgridobject*)i_object;
		this->move_object(obj);
	}

	int hgridbroadphase::get_broadphasepairnum() const
	{
		return (int)this->pair_array.size();
	}

	broadphasepair* hgridbroadphase::get_pairs()
	{
		return &this->pair_array[0];
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

			float ooSize = 1.0f / m_CellSize[level];

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
}//namespace  physics
