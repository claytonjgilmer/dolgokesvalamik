#ifndef _filesystem_h_
#define _filesystem_h_

#include "containers/string.h"
#include "containers/stringmap.h"
#include "containers/listallocator.h"

namespace file
{
	class system
	{
	public:
		system(){}
		~system(){}

		void register_path(const char* i_group,const char* i_path);
		void unregisterpath(const char* i_group, const char* i_path);
	private:
		struct groupstruct
		{
			groupstruct(const char* i_group):
			Name(i_group)
			{
			}

			ctr::tstring<16> Name; //group
			groupstruct* Next;
			ctr::vector<ctr::tstring<64>> m_pathbuf;
		} ;

		ctr::stringmap<groupstruct,128> m_pathmap;
		ctr::listallocator<groupstruct> m_groupbuf;
	};


	MLINLINE void system::register_path(const char* i_group,const char* i_path)
	{
		groupstruct* group=m_pathmap.get_data(i_group);

		if (!group)
		{
			group=new (m_groupbuf) groupstruct(i_group);
			m_pathmap.add_data(group);
		}

		group->m_pathbuf.push_back(i_path);
	}

	MLINLINE void system::unregisterpath(const char* i_group, const char* i_path)
	{
		groupstruct* group=m_pathmap.get_data(i_group);

		if (group)
		{
			for (unsigned n=0; n<group->m_pathbuf.size(); ++n)
			{
				if (group->m_pathbuf[n]==i_path)
				{
					group->m_pathbuf.eraseunordered(group->m_pathbuf.begin()+n);
					break;
				}
			}

			if (group->m_pathbuf.empty())
			{
				m_pathmap.remove_data(i_group);
				delete group;
//				delete <ctr::listallocator<groupstruct>> (m_groupbuf) group;
			}
		}

	}
}
#endif//_filesystem_h_