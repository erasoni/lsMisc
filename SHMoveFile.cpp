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

#include "stdafx.h"

#include <windows.h>
#include <tchar.h>
#include <assert.h>

#if _MSC_VER <= 1200
#pragma warning(disable:4786)
#endif

#include <vector>
#include <string>

#include <stlsoft/smartptr/scoped_handle.hpp>

using std::vector;
using std::wstring;
typedef vector<wstring> STRINGVECTOR;

#include "SHMoveFile.h"

#pragma comment(lib, "Shell32.lib")



namespace Ambiesoft {

	static void* myAlloc(size_t size)
	{
		return calloc(1, size);
	}
	static void myFree(void* p)
	{
		free(p);
	}

	enum CopyORMove {
		CopyORMove_Copy,
		CopyORMove_Move,
		CopyORMove_Delete,
	};

	static UINT GetwFunc(CopyORMove cm)
	{
		switch (cm)
		{
		case CopyORMove_Copy:return FO_COPY;
		case CopyORMove_Move:return FO_MOVE;
		case CopyORMove_Delete:return FO_DELETE;
		}
		assert(false);
		return 0;
	}
	static BOOL SHCopyOrMoveFileImpl(
		CopyORMove copyormove, 
		bool ismultidest, 
		LPCTSTR lpzzFileTo, 
		LPCTSTR lpzzFileFrom, 
		FILEOP_FLAGS fopFlags=FOF_ALLOWUNDO,
		int* pnRet=NULL)
	{
		SHFILEOPSTRUCT sfo = { 0 };
		sfo.hwnd = NULL;
		sfo.wFunc = GetwFunc(copyormove);
		sfo.pFrom = lpzzFileFrom;
		sfo.pTo = lpzzFileTo;
		sfo.fFlags = fopFlags |
			(ismultidest ? FOF_MULTIDESTFILES : 0) |
			0;

		int nRet = SHFileOperation(&sfo);//&& !sfo.fAnyOperationsAborted;
		if (pnRet)
			*pnRet = nRet;
		// it seems SHFileOperation returns 113 when same path specified.
		return nRet == 0 || nRet == 113;
	}



	static LPTSTR CreateDNString(const vector<wstring>& files, int* pnRet)
	{
		size_t size = 0;
		STRINGVECTOR::const_iterator it;
		for ( it = files.begin(); it != files.end(); ++it)
		{
			size += (it->size() + 1) * sizeof(WCHAR);
		}

		size += sizeof(WCHAR);
		BYTE* p = (BYTE*)myAlloc(size);
		LPTSTR pStart = (LPTSTR)p;
		for ( it = files.begin(); it != files.end(); ++it)
		{
			size_t copysize = (it->size()+1) * sizeof(WCHAR);
			errno_t err = memcpy_s(p, size, it->c_str(), copysize);
			if (err != 0)
			{
				if (pnRet)
					*pnRet = err;
				myFree((void*)pStart);
				return NULL;
			}
			p += copysize;
			size -= copysize;
		}
		*p = 0;
		return pStart;
	}
	static LPTSTR CreateDNString(LPCWSTR pW, int* pnRet)
	{
		vector<wstring> vs;
		vs.push_back(pW);
		return CreateDNString(vs, pnRet);
	}

	
	static BOOL SHCopyOrMoveFile(CopyORMove cm, LPCTSTR lpFileTo, LPCTSTR lpFileFrom, FILEOP_FLAGS fopFlags, int* pnRet)
	{
		if (cm != CopyORMove_Delete)
		{
			assert(lpFileTo && lpFileFrom);
			if (!(lpFileTo && lpFileFrom))
				return FALSE;
		}
		else if (cm == CopyORMove_Delete)
		{
			assert(lpFileFrom);
			assert(!lpFileTo);
			if (!lpFileFrom)
				return FALSE;
		}

		LPTSTR pFrom = CreateDNString(lpFileFrom, pnRet);
		stlsoft::scoped_handle<void*> ha(pFrom, myFree);

		LPTSTR pTo = NULL;
		if (cm != CopyORMove_Delete)
		{
			pTo = CreateDNString(lpFileTo, pnRet);
		}
		stlsoft::scoped_handle<void*> hi(pTo, myFree);

		return SHCopyOrMoveFileImpl(cm, false, pTo, pFrom, fopFlags, pnRet);
	}
	BOOL SHMoveFile(LPCTSTR lpFileTo, LPCTSTR lpFileFrom, FILEOP_FLAGS fopFlags, int* pnRet)
	{
		return SHCopyOrMoveFile(CopyORMove_Move, lpFileTo, lpFileFrom, fopFlags,pnRet);
	}
	BOOL SHCopyFile(LPCTSTR lpFileTo, LPCTSTR lpFileFrom, FILEOP_FLAGS fopFlags, int* pnRet)
	{
		return SHCopyOrMoveFile(CopyORMove_Copy, lpFileTo, lpFileFrom, fopFlags, pnRet);
	}
	BOOL SHDeleteFile(LPCTSTR lpFile, FILEOP_FLAGS fopFlags, int* pnRet)
	{
		return SHCopyOrMoveFile(CopyORMove_Delete, NULL, lpFile, fopFlags, pnRet);
	}



	// Move many file to 1 folder
	static BOOL SHCopyOrMoveFile(CopyORMove cm, 
		LPCTSTR lpFileTo, const std::vector<std::wstring>& sourcefiles, 
		FILEOP_FLAGS fopFlags,
		int* pnRet)
	{
		LPTSTR pFroms = CreateDNString(sourcefiles, pnRet);
		stlsoft::scoped_handle<void*> ha(pFroms, myFree);

		LPTSTR pTo = NULL;
		if (cm != CopyORMove_Delete)
		{
			pTo = CreateDNString(lpFileTo, pnRet);
		}
		stlsoft::scoped_handle<void*> hi(pTo, myFree);

		return SHCopyOrMoveFileImpl(cm, false, pTo, pFroms, fopFlags, pnRet);
	}
	BOOL SHMoveFile(LPCTSTR lpFileTo, 
		const std::vector<std::wstring>& sourcefiles, 
		FILEOP_FLAGS fopFlags,
		int* pnRet)
	{
		return SHCopyOrMoveFile(CopyORMove_Move, lpFileTo, sourcefiles, fopFlags, pnRet);
	}
	// Copy many files to 1 folder
	BOOL SHCopyFile(LPCTSTR lpFileTo, 
		const std::vector<std::wstring>& sourcefiles, 
		FILEOP_FLAGS fopFlags,
		int* pnRet)
	{
		return SHCopyOrMoveFile(CopyORMove_Copy, lpFileTo, sourcefiles, fopFlags, pnRet);
	}
	BOOL SHDeleteFile(const std::vector<std::wstring>& files, FILEOP_FLAGS fopFlags, int* pnRet)
	{
		return SHCopyOrMoveFile(CopyORMove_Delete, NULL, files, fopFlags, pnRet);
	}

	// move multiple files to multiple files
	BOOL SHCopyOrMoveFile(CopyORMove cm, 
		const vector<wstring>& destfiles, 
		const vector<wstring>& sourcefiles, 
		FILEOP_FLAGS fopFlags,
		int* pnRet)
	{
		LPTSTR pFroms = CreateDNString(sourcefiles, pnRet);
		stlsoft::scoped_handle<void*> ha(pFroms, myFree);

		LPTSTR pTos = CreateDNString(destfiles, pnRet);
		stlsoft::scoped_handle<void*> hi(pTos, myFree);
		
		return SHCopyOrMoveFileImpl(cm, true, pTos, pFroms, fopFlags, pnRet);
	}
	BOOL SHMoveFile(const vector<wstring>& destfiles, 
		const vector<wstring>& sourcefiles, 
		FILEOP_FLAGS fopFlags,
		int* pnRet)
	{
		return SHCopyOrMoveFile(CopyORMove_Move, destfiles, sourcefiles, fopFlags, pnRet);
	}
	BOOL SHCopyFile(const vector<wstring>& destfiles, 
		const vector<wstring>& sourcefiles,
		FILEOP_FLAGS fopFlags,
		int* pnRet)
	{
		return SHCopyOrMoveFile(CopyORMove_Copy, destfiles, sourcefiles, fopFlags, pnRet);
	}

} // namespace