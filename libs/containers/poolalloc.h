#ifndef _poolalloc_h_
#define _poolalloc_h_

namespace ctr
{
	template<class T, unsigned poolsize=128> //legyen mar kettohatvany
	class poolalloc
	{
	public:
		poolalloc();
		~poolalloc();

		void push_back(const T& i_elem);
		void pop_back(const T& i_elem);



	protected:
		ctr::vector<T*> m_poolbuf;
	};
}
#endif//_poolalloc_h_