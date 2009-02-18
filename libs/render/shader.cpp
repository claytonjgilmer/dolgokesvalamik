#include "shader.h"
#include "rendersystem.h"
#include "shadermanager.h"

	shader::shader(const void* i_buf, unsigned i_size, const char* i_name):
	resource(i_name)
	{
		Next=NULL;
		LPD3DXBUFFER errors;;
		if (D3DXCreateEffect(rendersystem::ptr->device(),i_buf,i_size,NULL,NULL,0,NULL,&m_effect,&errors)!=D3D_OK)
		{
			const char* v=(const char*)(errors->GetBufferPointer());
			assertion(0,v);
		}
	}

	shader::~shader()
	{
		if (m_effect)
			m_effect->Release();

		shadermanager::ptr->erase_shader(this);
	}

	void shader::reload(const void* i_buf, unsigned i_bufsize)
	{
		LPD3DXBUFFER errors;;
		LPD3DXEFFECT neweffect;
		if (D3DXCreateEffect(rendersystem::ptr->device(),i_buf,i_bufsize,NULL,NULL,0,NULL,&neweffect,&errors)!=D3D_OK)
		{
			const char* v=(const char*)(errors->GetBufferPointer());
			PRINT("hibas shader: %s \n",get_name().c_str());//,v);
//			assertion(0,v);
		}
		else
		{
			if (m_effect)
				m_effect->Release();

			m_effect=neweffect;
		}
	}

	void shader::set_constants(const shader_param_array_t& param_array)
	{
		for (int n=0; n<param_array.param_count; ++n)
			m_effect->SetValue(param_array.param_array[n].m_name.c_str(),param_array.param_array[n].m_addr,param_array.param_array[n].m_size);
	}
