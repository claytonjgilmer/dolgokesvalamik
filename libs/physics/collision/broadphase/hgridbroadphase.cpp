#include "hgridbroadphase.h"
#include "math/intersection.h"
#include "threading/taskmanager2.h"
#include "physics/system/physicssystem.h"

	hgridobject::hgridobject(void* i_userdata, const aabb& i_bounding, uint32 i_static):
	broadphaseobject(i_userdata,i_bounding,i_static)
	{
		this->pos=i_bounding.get_center();
		vec3 extent=i_bounding.get_extent();
		this->radius=Max(extent.x,Max(extent.y,extent.z));
	}

	hgridbroadphase::hgridbroadphase()
	{
		float size=MIN_CELL_SIZE;
		for (int n=0; n<HGRID_MAX_LEVELS;++n)
		{
			size*=CELL_TO_CELL_RATIO;

			objects_at_level[n]=0;
			oo_cell_size[n]=1/size;

			for (int m=0; m<NUM_BUCKETS;++m)
			{
				object_bucket[n][m]=NULL;
				time_stamp[n][m]=0;
			}
		}

		occupied_levels_mask=0;
		tick=0;
		pair_num=0;
	}

	void hgridbroadphase::add_object_to_hgrid(hgridobject *obj)
	{
		int level;
		float diameter = 2.0f * obj->radius;

		for (level = 0; SPHERE_TO_CELL_RATIO < diameter*this->oo_cell_size[level]; level++);

		assertion(level < HGRID_MAX_LEVELS);

		const float oosize=this->oo_cell_size[level];
		int bucket = compute_hash_bucket_index((int)(obj->pos.x *oosize), (int)(obj->pos.y *oosize), (int)(obj->pos.z *oosize));
		obj->bucket= bucket;
		obj->level = level;
		obj->next_object= this->object_bucket[level][bucket];
		obj->prev_object=NULL;

		if (this->object_bucket[level][bucket])
			this->object_bucket[level][bucket]->prev_object=obj;

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
		if (obj->next_object) obj->next_object->prev_object=obj->prev_object;
		if (obj->prev_object) obj->prev_object->next_object=obj->next_object;

		if (this->object_bucket[level][bucket] == obj)
			this->object_bucket[level][bucket] = obj->next_object;

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
*/
		assertion(0);
	}


	void hgridbroadphase::check_obj_against_grid(hgridobject* obj) //csak teljes teszt eseten hasznalhato
	{
		const int startLevel = obj->level;
		int loccupiedLevelsMask = this->occupied_levels_mask>> startLevel;
		const vec3 pos = obj->pos;
		const vec3 halfextent=obj->bounding_world.get_extent();

		for (int level = startLevel; level < HGRID_MAX_LEVELS && loccupiedLevelsMask; loccupiedLevelsMask >>= 1, level++)
		{
			if ((loccupiedLevelsMask & 1) == 0)
				continue;

			const float ooSize = this->oo_cell_size[level];
//			float ooSize = 1.0f / this->cell_size[level];

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
								if (aabb_aabb_intersect(obj->bounding_world,p->bounding_world))
								{
//									this->checkobj_mutex.lock();
									LONG index=_InterlockedExchangeAdd(&pair_num,1);
									new (this->pair_array+index) broadphasepair(obj,p);
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

	void hgridbroadphase::move_object(hgridobject* obj)
	{
		obj->pos=obj->bounding_world.get_center();

		const int level=obj->level;
		const float oosize= this->oo_cell_size[level];
		int bucket = compute_hash_bucket_index((int)(obj->pos.x *oosize), (int)(obj->pos.y *oosize), (int)(obj->pos.z *oosize));

		if (bucket==obj->bucket)
			return;

		this->remove_object_from_hgrid(obj);
		obj->bucket= bucket;

		obj->next_object = this->object_bucket[level][bucket];
		obj->prev_object=NULL;

		if (this->object_bucket[level][bucket])
			this->object_bucket[level][bucket]->prev_object=obj;

		this->object_bucket[level][bucket] = obj;

		this->objects_at_level[level]++;
		this->occupied_levels_mask |= (1 << level);
	}


	broadphaseobject* hgridbroadphase::create_object(void* i_userdata, const aabb& i_bounding, uint32 i_static)
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
		this->pair_num=0;

		listallocator<hgridobject>::iterator it;

		if (!physicssystem::ptr()->parallel_processing)
		{
			for (it=this->dynamic_list.begin(); it!=this->dynamic_list.end(); ++it)
				this->move_object(*it);

			for (it=this->dynamic_list.begin(); it!=this->dynamic_list.end(); ++it)
				this->check_obj_against_grid(*it);
		}
		else
		{
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
						broad_phase->check_obj_against_grid(buf[n]);
					}
				}

				hgridbroadphase* broad_phase;
				hgridobject** buf;
			};


			const unsigned obj_count=this->dynamic_list.size();
			hgridobject** obj_array=(hgridobject**)_alloca(obj_count*sizeof(hgridobject*));

			listallocator<hgridobject>::iterator it=this->dynamic_list.begin();

			for (unsigned n=0; n<obj_count;++n,++it)
			{
				obj_array[n]=*it;
				this->move_object(obj_array[n]);
			}

			taskmanager::ptr()->process_buffer(obj_count,10,hgrid_update_process(this,obj_array));
		}
	}

	void hgridbroadphase::update_object(broadphaseobject* i_object)
	{
		hgridobject* obj=(hgridobject*)i_object;
		this->move_object(obj);
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
