#ifndef _singleton_h_
#define _singleton_h_

#define DECLARE_SINGLETON(_TYPE_) \
									public:\
									static void create(const _TYPE_##desc* i_desc);\
										static void release();\
										static _TYPE_* instance();\
									private:\
										static _TYPE_* m_instance;\
										_TYPE_(const _TYPE_##desc* i_desc);\
										~_TYPE_();

#define  DEFINE_SINGLETON(_TYPE_)\
									_TYPE_* _TYPE_::m_instance=NULL;\
									void _TYPE_::create(const _TYPE_##desc* i_desc)\
									{\
										if (!m_instance)\
											m_instance=new _TYPE_(i_desc);\
									}\
\
									void _TYPE_::release()\
									{\
										if (m_instance)\
										{\
											delete m_instance;\
											m_instance=NULL;\
										}\
									}\
\
									_TYPE_* _TYPE_::instance()\
									{\
										return m_instance;\
									}

#endif//_singleton_h_