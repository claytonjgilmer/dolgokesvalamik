#ifndef hashgrid2d_h_
#define hashgrid2d_h_

template<typename T>
struct grid_cell
{
	T data;
	int pos[2];
	hash_elem* next;
	hash_elem* prev;
};

const int HASHBUF_SIZE=16384;

template<typename T, int CELL_SIZE=300>
struct hashgrid_2d
{
	hashgrid_2d();
	~hashgrid_2d();
	void insert_object(const T& obj, float bounding_min[2], float bounding_max[2]);
	void remove_object(const T& obj);
};
#endif
