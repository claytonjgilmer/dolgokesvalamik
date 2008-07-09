#ifndef _misc_h_
#define _misc_h_
#endif//_misc_h_

namespace base
{
	#define MLINLINE __forceinline
	#define  NULL 0

	#define SAFE_DELETE(_PTR_) {if (_PTR_){delete _PTR_; _PTR_=NULL;}}
}