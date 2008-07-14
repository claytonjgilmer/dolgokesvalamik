#ifndef _objectmanager_h_
#define _objectmanager_h_

namespace render
{
	struct objectmanagerdesc
	{
		objectmanagerdesc(const char* i_group):m_objectgroup(i_group){}
		ctr::string m_objectgroup;
	};
	struct objectmanager
	{
		DECLARE_SINGLETON_DESC(objectmanager,objectmanagerdesc);
	public:
		objectmanager(const objectmanagerdesc* i_desc);
		~objectmanager();

		scene::node* get_object(const char* i_name);

	private:
		ctr::string m_objectgroup;
		ctr::stringmap<
	};
}
#endif//_objectmanager_h_