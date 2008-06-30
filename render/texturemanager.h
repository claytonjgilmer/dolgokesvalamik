#ifndef _texturemanager_h_
#define _texturemanager_h_

#include "base/singleton.h"

namespace render
{
	class texturemanager:public base::singleton<texturemanager>
	{
	public:
	private:
		texturemanager();
		~texturemanager();
	};
}
#endif//_texturemanager_h_