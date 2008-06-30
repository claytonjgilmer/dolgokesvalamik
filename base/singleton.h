#ifndef _singleton_h_
#define _singleton_h_

namespace base
{
	template <class T>
	class singleton
	{
	public:
		static T* instance();
		static void create();
		static void release();

	private:
		singleton()
		{
		}

		~singleton()
		{
		}

		static T* m_instance;
	};

	template <class T>
	static T* singleton<T>::instance()
	{
		return m_instance;
	}

	template <class T>
	static void singleton<T>::create()
	{
		if (!m_instance)
		{
			m_instance=new T;
		}
	}

	template <class T>
	static void singleton<T>::release()
	{
		if (m_instance)
		{
			delete m_instance;
			m_instance=NULL;
		}
	}

#define SINGLETON_INSTANCE(_T_) _T_* base::singleton<_T_>::m_instance
} //namespace base

#endif//_singleton_h_