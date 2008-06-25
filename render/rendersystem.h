#ifndef _rendersystem_h_
#define _rendersystem_h_

#include "containers/listallocator.h"
#include "renderqueue.h"
#include "threading/taskmanager.h"

#include "rendervertexbuffer.h"
#include "renderindexbuffer.h"

namespace render
{
	class systemdesc
	{
	public:
		threading::taskmanager* m_taskmanager;
	};
	class system
	{
	public:
		system(const systemdesc& i_systemdesc);
		~system();

		vertexbuffer* createvertexbuffer(unsigned i_vertexnum);
		indexbuffer* createindexbuffer(unsigned i_indexnum);


		void renderqueues();

//	private:
		threading::taskmanager* m_taskmanager;
		ctr::listallocator<queue> m_queues;
	};

	extern system* g_system;
}
#endif//_rendersystem_h_