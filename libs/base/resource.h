#ifndef _resource_h_
#define _resource_h_

#include "referencedobject.h"
#include "containers/string.h"

namespace base
{
	class resource:public referencedobject
	{
	public:
		resource(const ctr::string& i_name):
		  m_name(i_name)
		{

		}

	const ctr::string& get_name(){return m_name;}
	protected:
		~resource(){}
	private:
		ctr::string m_name;

	};
}
#endif//_resource_h_