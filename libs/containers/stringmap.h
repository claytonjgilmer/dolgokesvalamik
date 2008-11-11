#ifndef _stringmap_h_
#define _stringmap_h_

#include "utils/misc.h"
#include "utils/stringhash.h"

//extern int szamlalo;
//extern int maxkeynum;

	struct stringmapstat
	{
		float m_avg;
		int m_max;
	};
	template <class T, unsigned bufsize=1024>
	class stringmap
	{
	public:
		stringmap();
		unsigned add_data(T* i_data);
		void remove_data(const char* i_name);
		T* get_data(const char* i_name) const;

		stringmapstat statistics() const;
		T** get_buffer(){return m_buf;}

	private:
		T* m_buf[bufsize];
	};


#define hashfn BKDRHash

	template<class T, unsigned bufsize>
	MLINLINE stringmap<T,bufsize>::stringmap()
	{
		for (unsigned n=0; n<bufsize; ++n)
			m_buf[n]=NULL;
	}

	template<class T, unsigned bufsize>
	MLINLINE unsigned stringmap<T,bufsize>::add_data(T* i_data)
	{
		unsigned hashkey=hashfn(i_data->get_name()) & (bufsize-1);
//		unsigned hashkey=hashfn(i_data->get_name()) % bufsize;
		i_data->Next=m_buf[hashkey];
//		if (m_buf[hashkey]!=NULL)
//			szamlalo++;
		m_buf[hashkey]=i_data;

//		int szam=0;
//		T* ptr=m_buf[hashkey];

//		while (ptr)
//		{
//			++szam;
//			ptr=ptr->Next;
//		}

//		if (szam>maxkeynum) maxkeynum=szam;


		return hashkey;
	}

	template<class T, unsigned bufsize>
	MLINLINE void stringmap<T,bufsize>::remove_data(const char* i_name)
	{
		unsigned hashkey=hashfn(string(i_name)) & (bufsize-1);
//		unsigned hashkey=hashfn(i_name) % bufsize;

		T* ptr=m_buf[hashkey];
		T* prevptr=NULL;

		while (ptr)
		{
			if (ptr->get_name()==i_name)
			{
				if (prevptr)
					prevptr->Next=ptr->Next;
				else
					m_buf[hashkey]=ptr->Next;

				break;

				prevptr=ptr;
			}
			ptr=ptr->Next;
		}
	}

	template<class T, unsigned bufsize>
	MLINLINE T* stringmap<T,bufsize>::get_data(const char* i_name) const
	{
		unsigned hashkey=hashfn(string(i_name)) & (bufsize-1);
//		unsigned hashkey=hashfn(i_name) % bufsize;

		T* ptr=m_buf[hashkey];

		while (ptr)
		{
			if (ptr->get_name()==i_name)
			{
				return ptr;
			}
			ptr=ptr->Next;
		}

		return NULL;
	}

	template<class T, unsigned bufsize>
	MLINLINE stringmapstat stringmap<T,bufsize>::statistics() const
	{
		stringmapstat stat;
		stat.m_avg=0;
		stat.m_max=0;

		unsigned count=0;

		for (unsigned int n=0; n<bufsize;++n)
		{
			if (m_buf[n])
			{
				++count;
				int act=0;
				T* ptr=m_buf[n];

				while (ptr)
				{
					ptr=ptr->Next;
					++act;
				}

				stat.m_avg+=(float)act;

				if (stat.m_max<act)
					stat.m_max=act;
			}
		}

		stat.m_avg/=(float)count;
		return stat;
	}
#endif//_stringmap_h_
