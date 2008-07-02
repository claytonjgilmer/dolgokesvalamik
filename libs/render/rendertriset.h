#ifndef _rendertriset_h_
#define _rendertriset_h_


namespace render
{
	class triset
	{
	public:
		unsigned m_firstvertex;
		unsigned m_numvertices;
		unsigned m_firstindex;
		unsigned m_numindices;

		material* m_material;

	};
}
#endif//_rendertriset_h_