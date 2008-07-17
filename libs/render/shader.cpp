#include "shader.h"
#include "rendersystem.h"
#include "shadermanager.h"

namespace render
{
	shader::shader(const void* i_buf, unsigned i_size, const char* i_name):
	resource(i_name)
	{
		Next=NULL;
		LPD3DXBUFFER errors;;
		if (D3DXCreateEffect(system::instance()->device(),i_buf,i_size,NULL,NULL,0,NULL,&m_effect,&errors)!=D3D_OK)
		{
			const char* v=(const char*)(errors->GetBufferPointer());
			utils::assertion(0,v);
		}
	}

	shader::~shader()
	{
		if (m_effect)
			m_effect->Release();

		shadermanager::instance()->erase_shader(this);
	}
}