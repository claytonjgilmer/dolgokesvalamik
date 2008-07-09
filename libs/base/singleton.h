#ifndef _singleton_h_
#define _singleton_h_

#define DECLARE_SINGLETON(_TYPE_) \
									public:\
										static void create(){if (!m_instance)m_instance=new _TYPE_;}\
										static void release(){if (m_instance) delete m_instance; m_instance=NULL;}\
										static _TYPE_* instance(){return m_instance;}\
									private:\
										static _TYPE_* m_instance;

#define DECLARE_SINGLETON_DESC(_TYPE_,_DESC_) \
										public:\
										static void create(const _DESC_* i_desc){if (!m_instance)m_instance=new _TYPE_(i_desc);}\
										static void release(){if (m_instance) delete m_instance; m_instance=NULL;}\
										static _TYPE_* instance(){return m_instance;}\
										private:\
										static _TYPE_* m_instance;

#define  DEFINE_SINGLETON(_TYPE_)\
									_TYPE_* _TYPE_::m_instance=NULL;

#endif//_singleton_h_