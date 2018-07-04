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

// Must comment out to define NOMINMAX
// #include "stdafx.h"

#define NOMINMAX
#include <Windows.h>

#include <vector>
#include <limits>

#include "GetChildWindowBy.h"

using namespace std;

namespace Ambiesoft{
	namespace {
		struct ContextData {
			size_t maxcount;
			LPCWSTR pName;
			vector<HWND> results;
		};

		static BOOL CALLBACK enumProc(
			_In_ HWND   hwnd,
			_In_ LPARAM lParam
			)
		{
			ContextData* pCxt = (ContextData*)lParam;

			if (pCxt->pName)
			{
				TCHAR szT[1024]; szT[0] = 0;
				GetWindowText(hwnd, szT, 1024);
				if (lstrcmp(szT, pCxt->pName) == 0)
				{
					pCxt->results.push_back(hwnd);
				}
			}
			else
			{
				pCxt->results.push_back(hwnd);
			}

			if (pCxt->results.size() >= pCxt->maxcount)
			{
				// no continue;
				return FALSE;
			}

			// continue
			return TRUE;
		}
	}
	HWND GetChildWindowByName(HWND hwndParent, LPCWSTR pName)
	{
		ContextData context = {};
		context.maxcount = 1;
		context.pName = pName;

		EnumChildWindows(hwndParent, enumProc, (LPARAM)&context);

		return context.results.empty() ? NULL : context.results[0];
	}

	vector<HWND> GetChildWindows(HWND hwndParent)
	{
		ContextData context = {};
		context.maxcount = std::numeric_limits<size_t>::max();
		context.pName = NULL;

		EnumChildWindows(hwndParent, enumProc, (LPARAM)&context);

		return context.results;
	}
} // namespace