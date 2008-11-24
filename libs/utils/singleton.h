#ifndef _singleton_h_
#define _singleton_h_

#define DECLARE_SINGLETON(_TYPE_) \
										static void create(){if (!ptr)ptr=new _TYPE_;}\
										static void release(){if (ptr) delete ptr; ptr=NULL;}\
										static _TYPE_* ptr;

#define DECLARE_SINGLETON_DESC(_TYPE_,_DESC_) \
										static void create(const _DESC_* i_desc){if (!ptr)ptr=new _TYPE_(i_desc);}\
										static void release(){if (ptr) delete ptr; ptr=NULL;}\
										static _TYPE_* ptr;

#define  DEFINE_SINGLETON(_TYPE_)\
									_TYPE_* _TYPE_::ptr=NULL;

#endif//_singleton_h_

