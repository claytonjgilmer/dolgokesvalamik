#include "performancemeter.h"

#ifdef measure_performance
DEFINE_SINGLETON(perf_analizer_t);
//////////////////////////////////////////////////////////////////////////
//	perf_meter_t
//////////////////////////////////////////////////////////////////////////

perf_meter_t::perf_meter_t(const char* i_name,perf_enum_t i_id)
{
	m_elem=perf_analizer_t::ptr->get_elem(i_name, i_id);
	perf_analizer_t::ptr->set_act_elem(m_elem);
	m_timer.reset();
}

perf_meter_t::~perf_meter_t()
{
	m_timer.stop();
	unsigned ticktoadd=m_timer.get_tick();
	m_elem->add_time(ticktoadd);
	m_elem->inc_call_num();
	perf_analizer_t::ptr->go_to_parent();
}



//////////////////////////////////////////////////////////////////////////
//  perf_elem_t
//////////////////////////////////////////////////////////////////////////

perf_elem_t::perf_elem_t(const char* i_name, perf_enum_t i_id):
m_name(i_name),
	m_id(i_id)
{
	//		m_parent=null;
	m_sumtime=0;
	m_sumcallnum=0;
	m_acttime=0;
}

void perf_elem_t::add_time(unsigned i_time)
{
	m_acttime=i_time;
	m_sumtime+=i_time;
}

void perf_elem_t::inc_call_num()
{
	++m_sumcallnum;
}


perf_elem_t* perf_elem_t::create_child(const char* i_name, perf_enum_t i_id)
{
	m_children.push_back(new perf_elem_t(i_name,i_id));
	//		m_children.back()->m_parent=this;

	return m_children.back();
}

void perf_elem_t::erase()
{
	for (perf_elem_array::iterator it=m_children.begin(); it!=m_children.end(); ++it)
		(*it)->erase();

	delete this;
}

void perf_elem_t::clear_sum()
{
	perf_elem_array::iterator it;
	for (it=m_children.begin(); it!=m_children.end(); ++it)
	{
		(*it)->clear_sum();
	}

	m_sumtime=0;
	m_sumcallnum=0;
}

void perf_elem_t::draw_performance(draw_func* i_fn, int& i_x, int& i_y)
{
	char str[1024];
	unsigned wholetime=perf_analizer_t::ptr->m_elems[perf_simulate]->m_sumtime;
	double rate=100.0*(double)m_sumtime/(double)wholetime;
	sprintf_s(str,"%s: calls:%d time:%d perc:%.2f%%%%",m_name.c_str(),m_sumcallnum,(int)(m_sumtime),rate);
	i_fn(i_x,i_y+=20,str);

	perf_elem_array::iterator it;
	for (it=m_children.begin(); it!=m_children.end(); ++it)
	{
		int tx(i_x+20);
		(*it)->draw_performance(i_fn,tx,i_y);
	}

}

//////////////////////////////////////////////////////////////////////////
//  perf_analizer_t
//////////////////////////////////////////////////////////////////////////

perf_analizer_t::perf_analizer_t():
m_root(new perf_elem_t("root",perf_root))
{
	m_framecount=0;
	m_actelem=m_root;
	//		gpperf_analizer_t=this;

	memset(m_elems,0,perf_last*(sizeof(perf_elem_t*)));
}

perf_analizer_t::~perf_analizer_t()
{
	m_root->erase();
}

perf_elem_t* perf_analizer_t::get_elem(const char* i_name, perf_enum_t i_id)
{
	if (!m_elems[i_id])
		m_elems[i_id]=m_actelem->create_child(i_name, i_id);

	assertion(m_elems[i_id]->m_id==i_id);

	return m_elems[i_id];
}

void perf_analizer_t::set_act_elem(perf_elem_t* i_elem)
{
	i_elem->m_parent=m_actelem;
	m_actelem=i_elem;
}

void perf_analizer_t::go_to_parent()
{
	m_actelem=m_actelem->m_parent;
}

void perf_analizer_t::flush()
{
	m_root->clear_sum();
	++m_framecount;
}

void perf_analizer_t::draw_performance(draw_func* i_fn, int x, int y)
{
	m_root->draw_performance(i_fn,x,y);
}
#else
void perf_analizer_t::flush()
{
}

void perf_analizer_t::draw_performance(draw_func* i_fn, int x, int y)
{
}
#endif
