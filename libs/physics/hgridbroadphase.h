#ifndef _hgridbroadphase_h_
#define _hgridbroadphase_h_

#include "DynBroadPhaseCollision.h"
#include "broadphaseobject.h"
#include "DynBroadPhasePair.h"
#include "containers/listallocator.h"

namespace physics
{
#define SOA
const unsigned NUM_BUCKETS=16384;

	// Computes hash bucket index in range [0, NUM_BUCKETS-1]
	MLINLINE unsigned compute_hash_bucket_index(int x, int y, int z)
	{
		const unsigned h1 = 0x8da6b343; // Large multiplicative constants;
		const unsigned h2 = 0xd8163841; // here arbitrarily chosen primes
		const unsigned h3 = 0xcb1ab31f;
		return unsigned (h1 * x + h2 * y + h3 * z) % NUM_BUCKETS;
	}

	struct hgridobject:public broadphaseobject
	{
	public:
		hgridobject(void* i_userdata, const math::aabb& i_boundingworld, uint32 i_is_static);
		hgridobject* next_object;
		math::vec3 pos;
		float radius;
		int bucket;
		int level;
	};

	const float SPHERE_TO_CELL_RATIO = 0.2f;

	const float CELL_TO_CELL_RATIO = 1.5f;

	const float MIN_CELL_SIZE=1.0f;
	const int HGRID_MAX_LEVELS=31;


	struct hgridbroadphase
	{
	public:
		hgridbroadphase();
		broadphaseobject* create_object(void* i_userdata, const math::aabb& i_bounding, uint32 i_static);
		void release_object(broadphaseobject*);
		void update_object(broadphaseobject*);

		void update() ;

		inherited int GetBroadPhasePairNum() const ;
		inherited BroadPhasePair* GetFirstPair();
		inherited BroadPhasePair* GetNextPair(BroadPhasePair*);
		inherited void GetObjectsInRange(Dyn::vector<BroadPhaseObject*>& o_ObjList, const AABB& i_Range);

	protected:
		void AddObjectToHGrid(HGridObject *obj);
		void RemoveObjectFromHGrid(HGridObject *obj);
		void CheckObjAgainstGrid(HGridObject *obj);//csak teljes teszt eseten hasznalhato

		void MoveObject(HGridObject* obj);

		unsigned m_OccupiedLevelsMask;

#ifdef SOA
#if 1
		int m_ObjectsAtLevel[HGRID_MAX_LEVELS];
		float m_CellSize[HGRID_MAX_LEVELS];
		HGridObject* m_ObjectBucket[HGRID_MAX_LEVELS][NUM_BUCKETS];
		int m_TimeStamp[HGRID_MAX_LEVELS][NUM_BUCKETS];
#else
		vector<int> m_ObjectsAtLevel;
		vector<float> m_CellSize;

		vector<vector<HGridObject*>> m_ObjectBucket;
		vector<vector<int>> m_TimeStamp;
#endif

#else
		struct Layer
		{
			HGridObject* m_ObjectBucket[NUM_BUCKETS];
			float	m_CellSize;
			int m_TimeStamp[NUM_BUCKETS];
			int m_ObjectsAtLevel;
		} m_Layer[HGRID_MAX_LEVELS];
#endif

		int m_Tick;

		Dyn::IntrusiveList<HGridObject> m_Static;
		Dyn::IntrusiveList<HGridObject> m_Dynamic;

		Dyn::vector<BroadPhasePair> m_Pair;

	};
}
#endif//_hgridbroadphase_h_