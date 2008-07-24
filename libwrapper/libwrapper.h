// libwrapper.h

#ifndef _libwrapper_h_
#define _libwrapper_h_
#pragma  once
#include "testapp.h"

using namespace System;

namespace libwrapper {

	public ref class game
	{
	public:
		static void init(IntPtr^ i_ptr)
		{
			init_app((HWND)i_ptr->ToPointer());
		}

		static void update()
		{
			update_app();
		}

		static void exit()
		{
			exit_app();
		}
	};

}

#endif//_libwrapper_h_
