namespace colli
{
	class shape
	{

	};

	class sphereshape
	{
	public:

	};
	class collider
	{

	};

	class intersectioninfo
	{

	};

	enum
	{
		shapetype_sphere,
		shapetype_num
	};

	typedef void (*intersecttestfn)(intersectioninfo&, const shape*, const mtx4x3&,const shape*,const mtx4x3&);

	class system
	{
	public:
		intersecttestfn m_intersectmatrix[shapetype_num][shapetype_num];

		system();
	};
}