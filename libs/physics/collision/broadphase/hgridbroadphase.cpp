#include "hgridbroadphase.h"

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

#ifdef SOA
			float ooSize = 1.0f / m_CellSize[level];
#else
			float ooSize = 1.0f / m_Layer[level].m_CellSize;
#endif


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
#ifdef SOA
						HGridObject* p = m_ObjectBucket[level][bucket];
#else
						HGridObject* p = m_Layer[level].m_ObjectBucket[bucket];
#endif
						while (p)
						{
							if (level!=startLevel || p <obj)
							{
								if (BoxBoxIntersection(obj->BoundingWorld(),p->BoundingWorld()))
								{
									m_Pair.push_back(BroadPhasePair(obj,p));
								}
							}
							p = p->m_NextObject;
						}
					}
				}
			}
		}
	}

//	static int move;
//	static int remove;

	void HGridBroadPhase::MoveObject(HGridObject* obj)
	{
//		move++;
		obj->m_Pos=obj->BoundingWorld().GetCenter();

#ifdef SOA
		const int level=obj->m_Level;
		float size= m_CellSize[level];
		int bucket = ComputeHashBucketIndex((int)(obj->m_Pos.x / size), (int)(obj->m_Pos.y / size), (int)(obj->m_Pos.z / size));

		if (bucket==obj->m_Bucket)
			return;

		RemoveObjectFromHGrid(obj);
		obj->m_Bucket= bucket;

		obj->m_NextObject = m_ObjectBucket[level][bucket];
		m_ObjectBucket[level][bucket] = obj;

		m_ObjectsAtLevel[level]++;
#else
		float size= m_Layer[obj->m_Level].m_CellSize;
		int bucket = ComputeHashBucketIndex((int)(obj->m_Pos.x / size), (int)(obj->m_Pos.y / size), (int)(obj->m_Pos.z / size));

		if (bucket==obj->m_Bucket)
			return;

		RemoveObjectFromHGrid(obj);
		obj->m_Bucket= bucket;

		const int level=obj->m_Level;
		obj->m_NextObject = m_Layer[level].m_ObjectBucket[bucket];
		m_Layer[level].m_ObjectBucket[bucket] = obj;
		m_Layer[level].m_ObjectsAtLevel++;
#endif
		m_OccupiedLevelsMask |= (1 << level);
	}


	/*inherited*/
	BroadPhaseObject* HGridBroadPhase::CreateObject(void* i_UserData, const AABB& i_Bounding, bool i_Static)
	{
		HGridObject* newobj= i_Static ? m_Static.AllocatePlace() : m_Dynamic.AllocatePlace();

		new (newobj) HGridObject(i_UserData,i_Bounding,i_Static);

		AddObjectToHGrid(newobj);

		return newobj;
	}

	/*inherited*/
	void HGridBroadPhase::ReleaseObject(BroadPhaseObject* i_Object)
	{
		HGridObject* obj=(HGridObject*)i_Object;

		RemoveObjectFromHGrid(obj);

		if (i_Object->IsStatic())
			m_Static.Deallocate(obj);
		else
			m_Dynamic.Deallocate(obj);
			
	}


	/*inherited*/
	void HGridBroadPhase::Update()
	{
		m_Pair.clear();

		IntrusiveList<HGridObject>::iterator it;

//		move=0;
//		remove=0;

		for (it=m_Dynamic.Begin(); it!=m_Dynamic.End(); ++it)
		{
			MoveObject(*it);
		}

//		PRINT("MOVE:%d REMOVE:%d\n",move,remove);

		for (it=m_Dynamic.Begin(); it!=m_Dynamic.End(); ++it)
		{
			CheckObjAgainstGrid(*it);
		}
	}

	/*inherited*/
	void HGridBroadPhase::UpdateObject(BroadPhaseObject* i_Object)
	{
		HGridObject* obj=(HGridObject*)i_Object;

		MoveObject(obj);
	}

	/*inherited*/
	int HGridBroadPhase::GetBroadPhasePairNum() const
	{
		return (int)m_Pair.size();
	}

	/*inherited*/
	BroadPhasePair* HGridBroadPhase::GetFirstPair()
	{
		return &*m_Pair.begin();
	}

	/*inherited*/
	BroadPhasePair* HGridBroadPhase::GetNextPair(BroadPhasePair* i_Object)
	{
		unsigned index=i_Object-&m_Pair[0]+1;

		if (index>=m_Pair.size())
			return NULL;
		return &m_Pair[index];
	}

	/*inherited*/
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

#ifdef SOA
			float ooSize = 1.0f / m_CellSize[level];
#else
			float ooSize = 1.0f / m_Layer[level].m_CellSize;
#endif

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

#ifdef SOA
						if (m_TimeStamp[level][bucket]==m_Tick)
							continue;

						m_TimeStamp[level][bucket]=m_Tick;
						HGridObject* p=m_ObjectBucket[level][bucket];
#else
						if (m_Layer[level].m_TimeStamp[bucket]==m_Tick)
							continue;

						m_Layer[level].m_TimeStamp[bucket]=m_Tick;

						HGridObject* p = m_Layer[level].m_ObjectBucket[bucket];
#endif
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
}