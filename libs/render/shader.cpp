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

	void shader::reload(const void* i_buf, unsigned i_bufsize)
	{
		LPD3DXBUFFER errors;;
		LPD3DXEFFECT neweffect;
		if (D3DXCreateEffect(system::instance()->device(),i_buf,i_bufsize,NULL,NULL,0,NULL,&neweffect,&errors)!=D3D_OK)
		{
			const char* v=(const char*)(errors->GetBufferPointer());
			utils::PRINT("hibas shader: %s \n",get_name().c_str());//,v);
//			utils::assertion(0,v);
		}
		else
		{
			if (m_effect)
				m_effect->Release();

			m_effect=neweffect;
		}
	}
}