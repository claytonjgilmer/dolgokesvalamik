#ifndef _resource_h_
#define _resource_h_

#include "referencedobject.h"
#include "containers/string.h"

	struct resource:referencedobject
	{
		resource(const string& i_name):
		  m_name(i_name)
		{

		}

		const string& get_name(){return m_name;}
		string m_name;

	};
#endif//_resource_h_
