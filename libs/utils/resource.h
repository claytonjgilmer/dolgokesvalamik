#ifndef _resource_h_
#define _resource_h_

#include "referencedobject.h"
#include "containers/string.h"

	class resource:public referencedobject
	{
	public:
		resource(const string& i_name):
		  m_name(i_name)
		{

		}

	const string& get_name(){return m_name;}
	protected:
		~resource(){}
	private:
		string m_name;

	};
#endif//_resource_h_