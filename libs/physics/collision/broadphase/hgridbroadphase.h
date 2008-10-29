#ifndef _hgridbroadphase_h_
#define _hgridbroadphase_h_

#include "broadphaseobject.h"
#include "broadphasepair.h"
#include "containers/listallocator.h"
#include "threading/mutex.h"

const unsigned NUM_BUCKETS=16384;

	// Computes hash bucket index in range [0, NUM_BUCKETS-1]
	MLINLINE unsigned compute_hash_bucket_index(int x, int y, int z)
	{
		const unsigned h1 = 0x8da6b343; // Large multiplicative constants;
		const unsigned h2 = 0xd8163841; // here arbitrarily chosen primes
		const unsigned h3 = 0xcb1ab31f;
		return unsigned (h1 * x + h2 * y + h3 * z) & (NUM_BUCKETS-1);
	}

	struct hgridobject:public broadphaseobject
	{
	public:
		hgridobject(void* i_userdata, const aabb& i_boundingworld, uint32 i_is_static);
		hgridobject* next_object;
		hgridobject* prev_object;
		vec3 pos;
		float radius;
		int bucket;
		int level;
	};

	const float SPHERE_TO_CELL_RATIO = 0.5f;

	const float CELL_TO_CELL_RATIO = 1.5f;

	const float MIN_CELL_SIZE=1.0f;
	const int HGRID_MAX_LEVELS=31;


	struct hgridbroadphase
	{
		hgridbroadphase();
		broadphaseobject* create_object(void* i_userdata, const aabb& i_bounding, uint32 i_static);
		void release_object(broadphaseobject*);
		void update_object(broadphaseobject*);

		void update() ;

		int get_broadphasepairnum() const ;
		broadphasepair* get_pairs();

		void add_object_to_hgrid(hgridobject *obj);
		void remove_object_from_hgrid(hgridobject *obj);
		void check_obj_against_grid(hgridobject *obj);//csak teljes teszt eseten hasznalhato

		void move_object(hgridobject* obj);

		unsigned occupied_levels_mask;

		int objects_at_level[HGRID_MAX_LEVELS];
		float oo_cell_size[HGRID_MAX_LEVELS];
		hgridobject* object_bucket[HGRID_MAX_LEVELS][NUM_BUCKETS];
		int time_stamp[HGRID_MAX_LEVELS][NUM_BUCKETS];

		int tick;

		listallocator<hgridobject> static_list;
		listallocator<hgridobject> dynamic_list;

//		vector<broadphasepair> pair_array;
		broadphasepair pair_array[65536];
		LONG pair_num;
		mutex hgrid_mutex;

	};
#endif//_hgridbroadphase_h_