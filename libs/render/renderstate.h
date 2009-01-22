#ifndef _renderstate_h_
#define _renderstate_h_

#include <d3d9.h>

	struct state
	{
		D3DRENDERSTATETYPE m_type;
		unsigned m_value;

		state(D3DRENDERSTATETYPE i_type, unsigned i_value):
		m_type(i_type),
			m_value(i_value)
		{
		}
	};
#endif//_renderstate_h_
