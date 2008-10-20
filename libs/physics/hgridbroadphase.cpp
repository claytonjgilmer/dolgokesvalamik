#include "stdafx.h"
#include "hgridbroadphase.h"

namespace Dyn
{
	hgridobject::hgridobject(void* i_userdata, const aabb& i_bounding, bool i_static):
	BroadPhaseObject(i_UserData,i_Bounding,i_Static)
	{
		m_Pos=i_Bounding.GetCenter();
		m_Radius=i_Bounding.GetExtent().MaxElem();
	}

	HGridBroadPhase::HGridBroadPhase()
	{
#if 0
		m_ObjectsAtLevel.resize(HGRID_MAX_LEVELS);
		m_CellSize.resize(HGRID_MAX_LEVELS);
		m_ObjectBucket.resize(HGRID_MAX_LEVELS);
		m_TimeStamp.resize(HGRID_MAX_LEVELS);
#endif

		float size=MIN_CELL_SIZE;
		for (int n=0; n<HGRID_MAX_LEVELS;++n)
		{
#if 0
			m_ObjectBucket[n].resize(NUM_BUCKETS);
			m_TimeStamp[n].resize(NUM_BUCKETS);
#endif

			size*=CELL_TO_CELL_RATIO;

#ifdef SOA
			m_ObjectsAtLevel[n]=0;
			m_CellSize[n]=size;

			for (int m=0; m<NUM_BUCKETS;++m)
			{
				m_ObjectBucket[n][m]=NULL;
				m_TimeStamp[n][m]=0;
			}
#else
			m_Layer[n].m_CellSize=size;
			m_Layer[n].m_ObjectsAtLevel=0;

			for (int m=0; m<NUM_BUCKETS;++m)
			{
				m_Layer[n].m_ObjectBucket[m]=NULL;
				m_Layer[n].m_TimeStamp[m]=0;
			}
#endif

		}

		m_OccupiedLevelsMask=0;
		m_Tick=0;
	}

	void HGridBroadPhase::AddObjectToHGrid(HGridObject *obj)
	{
		int level;
		float diameter = 2.0f * obj->m_Radius;

#ifdef SOA
		for (level = 0; m_CellSize[level]* SPHERE_TO_CELL_RATIO < diameter; level++);

		DYNASSERT(level < HGRID_MAX_LEVELS);

		float size=m_CellSize[level];
		int bucket = ComputeHashBucketIndex((int)(obj->m_Pos.x /	 size), (int)(obj->m_Pos.y / size), (int)(obj->m_Pos.z / size));
		obj->m_Bucket= bucket;
		obj->m_Level = level;
		obj->m_NextObject = m_ObjectBucket[level][bucket];
		m_ObjectBucket[level][bucket] = obj;
		m_ObjectsAtLevel[level]++;
#else
		for (level = 0; m_Layer[level].m_CellSize* SPHERE_TO_CELL_RATIO < diameter; level++);

		DYNASSERT(level < HGRID_MAX_LEVELS);

		float size=m_Layer[level].m_CellSize;
		int bucket = ComputeHashBucketIndex((int)(obj->m_Pos.x /	 size), (int)(obj->m_Pos.y / size), (int)(obj->m_Pos.z / size));
		obj->m_Bucket= bucket;
		obj->m_Level = level;

		obj->m_NextObject = m_Layer[level].m_ObjectBucket[bucket];
		m_Layer[level].m_ObjectBucket[bucket] = obj;

		m_Layer[level].m_ObjectsAtLevel++;
#endif
		m_OccupiedLevelsMask |= (1 << level);
	}

	void HGridBroadPhase::RemoveObjectFromHGrid(HGridObject *obj)
	{
#ifdef SOA
		int level=obj->m_Level;
		if (--m_ObjectsAtLevel[level] == 0)
			m_OccupiedLevelsMask &= ~(1 << level);

		int bucket= obj->m_Bucket;
		HGridObject *p = m_ObjectBucket[level][bucket];

		if (p == obj)
		{
			m_ObjectBucket[level][bucket] = obj->m_NextObject;
			return;
		}
#else
		if (--m_Layer[obj->m_Level].m_ObjectsAtLevel == 0)
			m_OccupiedLevelsMask &= ~(1 << obj->m_Level);

		int bucket= obj->m_Bucket;

		HGridObject *p = m_Layer[obj->m_Level].m_ObjectBucket[bucket];

		if (p == obj)
		{
			m_Layer[obj->m_Level].m_ObjectBucket[bucket] = obj->m_NextObject;
			return;
		}
#endif
		while (p)
		{
			HGridObject*q = p;
			p = p->m_NextObject;

			if (p == obj)
			{
				q->m_NextObject= p->m_NextObject;
				return;
			}
		}

		DYNASSERT(0);
	}


	void HGridBroadPhase::CheckObjAgainstGrid(HGridObject* obj) //csak teljes teszt eseten hasznalhato
	{
		const int startLevel = obj->m_Level;
		int loccupiedLevelsMask = this->m_OccupiedLevelsMask >> startLevel;
		const Vector3 pos = obj->m_Pos;
		const Vector3 halfextent=obj->BoundingWorld().GetExtent();

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