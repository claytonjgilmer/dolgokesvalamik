#ifndef _inertia_h_
#define _inertia_h_

MLINLINE mtx3x3 get_inertia_box(float mass, vec3 extent)
{
	mtx3x3 ret; ret.identity();
	ret._11=mass/3*(extent.y*extent.y+extent.z*extent.z);
	ret._22=mass/3*(extent.x*extent.x+extent.z*extent.z);
	ret._33=mass/3*(extent.x*extent.x+extent.y*extent.y);

	return ret;
}

MLINLINE mtx3x3 get_inertia_sphere(float mass, float radius)
{
	mtx3x3 ret; ret.identity();

	ret._11=ret._22=ret._33=0.4f*mass*radius*radius;
	return ret;
}
#endif//_inertia_h_