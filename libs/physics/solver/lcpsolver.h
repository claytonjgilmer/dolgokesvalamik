#ifndef _lcpsolver_h_
#define _lcpsolver_h_

#include "physics/system/physicssystem.h"
#include "physics\collision\system\contact.h"
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
	jacobi* J;
	jacobi* B;
	body_index_t*	body_index;
	float*  right_side;
	float*  right_side_poscorr;
	float*  diag;
	float*  lambda;
	float*  lambda_poscorr;
	float*  friction_coeff;
	char* constraintnum;
};

struct accel_t
{
	vec3 v;
	vec3 w;
	vec3 p;
	vec3 o;

	void clear()
	{
		v.clear();
		w.clear();
		p.clear();
		o.clear();
	}
};

struct lcp_solver_t
{
    void process(contact_surface_t* i_contact_array, int i_contact_count, float i_dt);

	void allocate_buffer();
	void pre_step();
	void pre_step_contacts_and_frictions();
	void solve_constraints();
	void solve_contacts();
	void solve_frictions();
	void clean();

	void cache_lambda();

	static accel_t accel[65536];

	lcp_data_t lcp_data_contact;
	lcp_data_t lcp_data_friction;
	lcp_data_t lcp_data_joint;
	contact_surface_t* contact_array;
	int contact_count;
	float dt;

	int body_count;
};
#endif//_lcpsolver_h_
