#ifndef _renderstatebuffer_h_
#define _renderstatebuffer_h_

#include "renderstate.h"
#include "rendersystem.h"

namespace render
{
	class statebuffer
	{
	public:
		void apply_states();
		void add_state(const state& i_elem);
	private:
		ctr::vector<state> m_buf;
	};

	MLINLINE void statebuffer::apply_states()
	{
		for (unsigned n=0; n<m_buf.size(); ++n)
			render::system::instance()->set_renderstate(m_buf[n]);
	}

	MLINLINE void statebuffer::add_state(const state& i_elem)
	{
		m_buf.push_back(i_elem);
	}
}
#endif//_renderstatebuffer_h_