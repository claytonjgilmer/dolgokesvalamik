#ifndef _auto_ptr_h_
#define _auto_ptr_h_

namespace utils
{
	template <typename T>
	class auto_ptr
	{
	public:
		auto_ptr(){m_ptr=NULL;}
		auto_ptr(T* i_ptr):m_ptr(i_ptr){}
		~auto_ptr(){if (m_ptr) delete m_ptr;}
		void operator=(T* i_ptr){
			if (m_ptr)
				delete m_ptr;
			m_ptr=i_ptr;}
		T* operator->(){
			return m_ptr;}
		const T* operator->() const{
			return m_ptr;}

		operator T*(){
			return m_ptr;}
		operator const T*() const{
			return m_ptr;}
	private:
		auto_ptr(const auto_ptr& i_other){}
		void operator=(const auto_ptr& i_other){}
		T* m_ptr;
	};
}
#endif//_auto_ptr_h_
