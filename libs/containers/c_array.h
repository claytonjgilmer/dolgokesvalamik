#ifndef _c_array_h_
#define _c_array_h_

#include "utils/misc.h"

template <typename T, unsigned size>
struct c_array
{
	T buf[size];
	T& operator[](unsigned index)
	{
		assertion(index < size);
		return buf[index];
	}
	const T& operator[](unsigned index) const
	{
		assertion(index < size);
		return buf[index];
	}
};
#endif//_c_array_h_