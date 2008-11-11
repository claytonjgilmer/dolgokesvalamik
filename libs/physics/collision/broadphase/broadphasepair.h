#ifndef _broadphasepair_h_
#define _broadphasepair_h_

#pragma  warning(disable:4351)

	struct broadphaseobject;

	struct broadphasepair
	{
		broadphasepair():object(){}
		broadphasepair(broadphaseobject* i_object1,broadphaseobject* i_object2):
		object()
		{
			((broadphaseobject*&)object[0])=i_object1;
			((broadphaseobject*&)object[1])=i_object2;
		}

		broadphaseobject*	const object[2];

	};//broadphasepair
#endif//_broadphasepair_h_
