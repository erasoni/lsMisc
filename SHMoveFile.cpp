#include "stdafx.h"

#include <windows.h>
#include <tchar.h>
#include <assert.h>

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
	static BOOL SHCopyOrMoveFile(BOOL bCopy, LPCTSTR lpFileTo, LPCTSTR lpzzFileFrom, int* pnRet)
	{
		size_t destCharCount = wcslen(lpFileTo);
		size_t destByteAlloc = (destCharCount + 1 + 1) * sizeof(TCHAR);
		LPTSTR pTo = (LPTSTR)myAlloc(destByteAlloc);
		stlsoft::scoped_handle<void*> ha(pTo, myFree);

		// wcscpy_s(pTo, destByteAlloc, lpFileTo);
		memcpy_s(pTo, destByteAlloc, lpFileTo, destCharCount * sizeof(TCHAR));

		SHFILEOPSTRUCT sfo = { 0 };
		sfo.hwnd = NULL;
		sfo.wFunc = bCopy ? FO_COPY:FO_MOVE;
		sfo.pFrom = lpzzFileFrom;
		sfo.pTo = pTo;
		sfo.fFlags = 
			FOF_ALLOWUNDO | 
			FOF_FILESONLY | 
			FOF_NOERRORUI |
			0;

		int nRet = SHFileOperation(&sfo);//&& !sfo.fAnyOperationsAborted;
		if (pnRet)
			*pnRet = nRet;
		// it seems SHFileOperation returns 113 when same path specified.
		return nRet == 0 || nRet == 113;
	}



	static LPCTSTR CreateDNString(const vector<wstring>& sourcefiles, int* pnRet)
	{
		size_t size = 0;
		for (STRINGVECTOR::const_iterator it = sourcefiles.begin(); it != sourcefiles.end(); ++it)
		{
			size += (it->size() + 1) * sizeof(WCHAR);
		}

		size += sizeof(WCHAR);
		BYTE* p = (BYTE*)myAlloc(size);
		LPCTSTR pStart = (LPCTSTR)p;
		for (STRINGVECTOR::const_iterator it = sourcefiles.begin(); it != sourcefiles.end(); ++it)
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

	BOOL SHMoveOneFile(LPCTSTR lpFileTo, LPCTSTR lpFileFrom, int* pnRet)
	{
		assert(lpFileTo && lpFileFrom);

		size_t fromCharCount = wcslen(lpFileFrom);
		size_t pFromAllocSize = (fromCharCount + 1 + 1) * sizeof(TCHAR);
		LPTSTR pFrom = (LPTSTR)myAlloc(pFromAllocSize);
		stlsoft::scoped_handle<void*> ha(pFrom, myFree);
		memcpy_s(pFrom, pFromAllocSize, lpFileFrom, (fromCharCount + 1)*sizeof(wchar_t));
		//wcscpy_s(pFrom, fromCharCount, lpFileTo);

		return SHCopyOrMoveFile(false, lpFileTo, pFrom, pnRet);
	}
	BOOL SHCopyOneFile(LPCTSTR lpFileTo, LPCTSTR lpFileFrom, int* pnRet)
	{
		assert(lpFileTo && lpFileFrom);

		size_t fromCharCount = wcslen(lpFileFrom);
		size_t pFromAllocSize = (fromCharCount + 1 + 1) * sizeof(TCHAR);
		LPTSTR pFrom = (LPTSTR)myAlloc(pFromAllocSize);
		stlsoft::scoped_handle<void*> ha(pFrom, myFree);
		memcpy_s(pFrom, pFromAllocSize, lpFileFrom, (fromCharCount + 1)*sizeof(wchar_t));
		//wcscpy_s(pFrom, fromCharCount, lpFileTo);

		return SHCopyOrMoveFile(true, lpFileTo, pFrom, pnRet);
	}
	BOOL SHMoveFile(LPCTSTR lpFileTo, const std::vector<std::wstring>& sourcefiles, int* pnRet)
	{
		LPCTSTR pStart = CreateDNString(sourcefiles, pnRet);
		if(!pStart)
			return FALSE;
		stlsoft::scoped_handle<void*> ha((void*)pStart, myFree);
		return SHCopyOrMoveFile(false, lpFileTo, pStart, pnRet);
	}
	BOOL SHCopyFile(LPCTSTR lpFileTo, const std::vector<std::wstring>& sourcefiles, int* pnRet)
	{
		LPCTSTR pStart = CreateDNString(sourcefiles, pnRet);
		if(!pStart)
			return FALSE;
		stlsoft::scoped_handle<void*> ha((void*)pStart, myFree);
		return SHCopyOrMoveFile(true, lpFileTo, pStart, pnRet);
	}

} // namespace