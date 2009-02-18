#ifndef _shaderparam_h_
#define _shaderparam_h_


struct shader_param_t
{
	string m_name;
	void* m_addr;
	unsigned m_size;
};

struct shader_param_array_t
{
	shader_param_t param_array[20];
	int param_count;

	shader_param_array_t()
	{
		param_count=0;
	}

	void bind_param(const char* i_name, void* i_addr, unsigned i_size)
	{
		assertion(param_count<20,"tul sok a parametert akarol balyndolni");

		param_array[param_count].m_name=i_name;
		param_array[param_count].m_addr=i_addr;
		param_array[param_count].m_size=i_size;
		++param_count;
	}

};
#endif//_shaderparam_h_
