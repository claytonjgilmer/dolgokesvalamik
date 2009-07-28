#ifndef _auto_ptr_h_
#define _auto_ptr_h_

	template <typename T>
	struct auto_ptr
	{
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

		T& operator *(){
			return *m_ptr;}

		const T& operator*()const{
			return *m_ptr;}

		explicit auto_ptr(const auto_ptr& i_other):m_ptr(i_other.m_ptr)
		{
			((auto_ptr&)i_other).m_ptr=(T*)NULL;
		}

		T* m_ptr;

		void operator=(const auto_ptr* i_other)
		{
		}
	};
#endif//_auto_ptr_h_
