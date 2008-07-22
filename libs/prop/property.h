#ifndef _property_h_
#define _property_h_

#include "math/vec2.h"

namespace prop
{
	/*
	mi az ami kell
	ez alapjan inicializalodik fel az objektum
	editorban lehessen allitani
	kell callback a valtozaskor

	*/
	union propvalue
	{
		int m_intvalue;
		float m_floatvalue;
		ctr::string m_stringvalue;
		math::vec3 m_vec3value;
		math::vec2 m_vec2value;
		
	};

#define _define_prop_(_type_,_val_) const _type_& get() const{return m_val.##_val_;} void set(const _type_& i_val){m_val.##_val_=i_val; Callbacks();}
	struct property
	{
		propvalue m_val;

		_define_prop_(int,m_intvalue)
		_define_prop_(float,m_floatvalue)
		_define_prop_(ctr::string,m_stringvalue)
		_define_prop_(math::vec3,m_vec3value)
		_define_prop_(math::vec2,m_vec2value)

	};
}

#endif//_property_h_