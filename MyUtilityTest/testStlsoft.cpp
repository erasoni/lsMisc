#include "stdafx.h"

#if 0 //_CPPLIB_VER < 600

#include <windows.h>
#include <string>

#include "../stlsoft/include/platformstl/synch/process_mutex.hpp"

void testStlMutex()
{
	stlsoft::platformstl_project::process_mutex pm;
	pm.lock();
}

#endif // #if _CPPLIB_VER < 600