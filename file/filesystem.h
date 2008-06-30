#ifndef _filesystem_h_
#define _filesystem_h_

#include "containers/string.h"
#include "containers/stringmap.h"
#include "containers/listallocator.h"
#include "file/file.h"

namespace file
{
	class system
	{
	public:
		system(){}
		~system(){}

		void register_path(const char* i_group,const char* i_path);
		void unregister_path(const char* i_group, const char* i_path);
		void openfile(file& o_file, const char* i_group, const char* i_filename, const char* i_mode) const;
	private:
		struct groupstruct
		{
			groupstruct(const char* i_group):
			Name(i_group)
			{
			}

			~groupstruct()
			{
				int a=0;
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
			group=new (m_groupbuf.allocate_place()) groupstruct(i_group);
			m_pathmap.add_data(group);
		}

		group->m_pathbuf.push_back(i_path);
	}

	MLINLINE void system::unregister_path(const char* i_group, const char* i_path)
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
				m_groupbuf.deallocate(group);
			}
		}
	}

	MLINLINE void system::openfile(file& o_file, const char* i_group, const char* i_filename, const char* i_mode) const
	{
		groupstruct* group=m_pathmap.get_data(i_group);

		if (!group)
			return;

		for (unsigned int n=0; n<group->m_pathbuf.size();++n)
		{
			o_file.open((group->m_pathbuf[n]+i_filename).c_str(),i_mode);

			if (o_file.opened())
				return;
		}
	}
}
#endif//_filesystem_h_