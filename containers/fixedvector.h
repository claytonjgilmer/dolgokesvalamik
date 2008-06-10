#ifndef _fixedvector_h_
#define _fixedvector_h_

namespace ctr
{
	template <class T, unsized bufsize=128> class fixedvector
	{
	public:
		fixedvector();
		unsigned size() const;
		void push_back(const T&);
		void pop_back();
		T& operator[](int i_index);
		const T& operator[](int i_index) const;
		
		
		
}
#endif//_fixedvector_h_