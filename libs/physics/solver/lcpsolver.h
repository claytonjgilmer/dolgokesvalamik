#ifndef _lcpsolver_h_
#define _lcpsolver_h_

#include "physics/system/physicssystem.h"
#include "physics\collision\system\contact.h"

struct lcp_solver_t
{
    void process(contact_t* contact_array, int contact_num, float dt);
};
#endif//_lcpsolver_h_
