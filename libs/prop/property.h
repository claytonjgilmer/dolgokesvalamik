#ifndef _property_h_
#define _property_h_

#include "math/vec2.h"

	/*
	mi az ami kell
	ez alapjan inicializalodik fel az objektum
	editorban lehessen allitani
	kell callback a valtozaskor

	*/
/*
	union propvalue
	{
		int* m_intvalue;
		f32* m_floatvalue;
		string* m_stringvalue;
		vec3* m_vec3value;
		vec2* m_vec2value;

	};

#define _define_prop_(_type_,_val_) const _type_& get_##_type_() const{return *m_val.##_val_;} void set(const _type_& i_val){*m_val.##_val_=i_val; Callbacks();}
	struct property
	{
		propvalue m_val;

		_define_prop_(int,m_intvalue)
		_define_prop_(f32,m_floatvalue)
		_define_prop_(string,m_stringvalue)
		_define_prop_(vec3,m_vec3value)
		_define_prop_(vec2,m_vec2value)

	};
*/

#endif//_property_h_
