#ifndef _lcpsolver_h_
#define _lcpsolver_h_

#include "physics/system/physicssystem.h"
#include "physics\collision\system\contact.h"
#include "containers\vector.h"
struct jacobi
{
	vec3 v1;
	vec3 w1;
	vec3 v2;
	vec3 w2;
};

struct body_index_t
{
	unsigned short i[2];
};


struct lcp_data_t
{
	vector<jacobi> J;
	vector<jacobi> B;
	vector<body_index_t>	body_index;
	vector<f32>  right_side;
	vector<f32>  right_side_poscorr;
	vector<f32>  diag;
	vector<f32>  lambda;
	vector<f32>  lambda_poscorr;
	vector<f32>  friction_coeff;
	vector<char> constraintnum;

#if 0
	jacobi* J;
	jacobi* B;
	body_index_t*	body_index;
	f32*  right_side;
	f32*  right_side_poscorr;
	f32*  diag;
	f32*  lambda;
	f32*  lambda_poscorr;
	f32*  friction_coeff;
	char* constraintnum;
#endif
};

struct lcp_solver_t
{
    void process(contact_surface_t** i_contact_array, int i_contact_count, f32 i_dt);

	void allocate_buffer();
	void pre_step();
	void pre_step_contacts_and_frictions();
	void solve_constraints();
	void solve_contacts();
	void solve_frictions();
	void clean();

	void cache_lambda();

//	static accel_t accel[65536];

	lcp_data_t lcp_data_contact;
	lcp_data_t lcp_data_friction;
	lcp_data_t lcp_data_joint;
	contact_surface_t** contact_array;
	uint16 contact_index[10000];
	int contact_count;
	int constraint_count;
	f32 dt;

	int body_count;
};
#endif//_lcpsolver_h_
