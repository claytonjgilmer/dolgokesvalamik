#ifndef _performancemeter_h_
#define _performancemeter_h_

#include <string>
#include <memory>
#include "timer.h"
#include "containers/vector.h"
#include "utils/singleton.h"

typedef void(draw_func)(int i_X, int i_Y, const char* i_Str);
#define measure_performance

#ifdef measure_performance
#define perf_meter(_name_)  perf_meter_t perf##_name_(#_name_,_name_)
enum perf_enum_t
{
	perf_root,
	perf_simulate,
	perf_collide,
	perf_broadphase,
	perf_nearphase,
	perf_broadphaseupdate,
	perf_sapupdatearray,
	perf_sapupdateoverlap,
	perf_sapaddactivepair,
	perf_saperaseactivepair,
	perf_sapsort,
	perf_sapsweep,
	perf_sapobjectnum,
	perf_sappossiblepairnum,
	perf_sappairnum,
	perf_manifoldupdate,
	perf_intersectloop,
	perf_getmanifold,
	perf_testintersectloop,
	perf_testintersect,
	perf_addcontact,
	perf_getcontactinfo,
	perf_predictposition,
	perf_creategroups,
	perf_createbatches,
	perf_inertia,

	perf_solve,
	perf_solverprestep,
	perf_solveconstraints,
	perf_solve_contact,
	perf_solve_friction,

	perf_updateposition,
	perf_sleepgroups,
	perf_getuserdata,
	perf_getnextbroadpair,
	perf_1,
	perf_2,
	perf_3,
	perf_4,
	perf_5,


	perf_last
};



struct perf_analizer_t;
struct perf_elem_t;

struct perf_meter_t
{

	perf_meter_t(const char* i_name, perf_enum_t i_id);
	~perf_meter_t();

	timer_t m_timer;
	perf_elem_t* m_elem;

};//struct perf_meter_t

struct perf_elem_t
{
	void add_time(unsigned i_time);
	void inc_call_num();
	perf_elem_t* create_child(const char* i_name, perf_enum_t i_id);
	void erase();
	void clear_sum();

	void draw_performance(draw_func* i_fn, int& i_x, int& i_y);


	perf_elem_t(const char*i_name, perf_enum_t i_id);
	perf_elem_t* m_parent;
	typedef vector<perf_elem_t*> perf_elem_array;
	perf_elem_array m_children;

	const std::string m_name;
	const perf_enum_t m_id;
	unsigned m_acttime;
	unsigned m_sumtime;
	int	  m_sumcallnum;

};//struct perf_elem_t

struct perf_analizer_t
{
	DECLARE_SINGLETON(perf_analizer_t);
	perf_analizer_t();
	~perf_analizer_t();

	void flush();
	void draw_performance(draw_func* i_fn);

	perf_elem_t* get_elem(const char* i_name, perf_enum_t i_id);
	void set_act_elem(perf_elem_t* i_elem);
	void go_to_parent();

	perf_elem_t* const m_root;

	perf_elem_t* m_actelem;
	//		perf_elem_t* m_parentelem;
	int	m_framecount;

	perf_elem_t* m_elems[perf_last];

};//struct perf_analizer_t


//	extern perf_analizer_t* gpperf_analizer_t;
#else
struct perf_analizer_t
{

	void flush();
	void draw_performance(draw_func* i_fn);
};
#define  perf_meter_t(_name_)
#endif
#endif//_performancemeter_h_