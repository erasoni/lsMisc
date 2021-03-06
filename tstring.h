//Copyright (C) 2017 Ambiesoft All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//1. Redistributions of source code must retain the above copyright
//notice, this list of conditions and the following disclaimer.
//2. Redistributions in binary form must reproduce the above copyright
//notice, this list of conditions and the following disclaimer in the
//documentation and/or other materials provided with the distribution.
//
//THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
//ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
//FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//SUCH DAMAGE.

#pragma once


#pragma once
#include <string>
#include <iostream>

#ifdef UNICODE
	typedef wchar_t tchar;
	typedef std::wstring tstring;
	typedef std::wstringstream tstringstream;
	typedef std::wifstream tifstream;
	typedef std::wofstream tofstream;
    inline std::wostream& tcoutfunc() { return std::wcout; }
    #define tcout tcoutfunc()
    inline std::wostream& tcerrfunc() { return std::wcerr; }
    #define tcerr tcerrfunc()
#else
	typedef char tchar;
	typedef std::string tstring;
	typedef std::stringstream tstringstream;
	typedef std::ifstream tifstream;
	typedef std::ofstream tofstream;
	inline std::ostream& tcoutfunc() { return std::cout; }
	#define tcout tcoutfunc()
	inline std::ostream& tcerrfunc() { return std::cerr; }
	#define tcerr tcerrfunc()
#endif
