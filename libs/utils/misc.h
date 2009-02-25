#ifndef _misc_h_
#define _misc_h_

#define MLINLINE __forceinline

#ifndef NULL
#define  NULL 0
#endif//NULL

#define SAFE_DELETE(_PTR_) {if (_PTR_){delete _PTR_; _PTR_=NULL;}}
#define array_elemcount(_ARRAY_) (sizeof(_ARRAY_)/sizeof(_ARRAY_[0]))

#define member(_T_,_V_) private: _T_ m_##_V_ ; public: const _T_ &get_##_V_ () const{return m_##_V_ ;} void set_##_V_ (const _T_ &i_##_V_ ){m_##_V_ =i_##_V_ ;}

void PRINT(const char* msg,...);

#endif//_misc_h_
