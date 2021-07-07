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

#ifndef PCH_INCLUDE_FILE
// Define PCH_INCLUDE_FILE outside of the code
#define PCH_INCLUDE_FILE "StdAfx.h"
#endif
#include PCH_INCLUDE_FILE

#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>

#include "GetFormattedTime.h"

BOOL GetFormattedTime(LONGLONG llSec, LPTSTR szFormatted)
{
	if( szFormatted==NULL )
		return FALSE;


	UINT nHour = (UINT)llSec / 3600;
	llSec -= nHour*3600;
	assert(llSec < 3600);


	UINT nMinutes = (UINT)llSec / 60;
	llSec -= nMinutes*60;
	assert(llSec < 60);

	if( nHour > 99 )
		_stprintf(szFormatted, _T("??:%02d:%02I64u"), nMinutes, llSec);
	else
		_stprintf(szFormatted, _T("%02d:%02d:%02I64u"), nHour, nMinutes, llSec);

	return TRUE;
}
