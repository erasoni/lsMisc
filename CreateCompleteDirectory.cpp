#include "stdafx.h"

#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>

#include "CreateCompleteDirectory.h"

static BOOL GetCountedDir( LPTSTR szBuff, LPCTSTR lpszDir, size_t sizeBuf, int nCount )
{
	TCHAR buf[_MAX_PATH];
	TCHAR* p = buf;
	int nLengOrg = lstrlen(lpszDir);
	if( nLengOrg > sizeof(buf) )
		return FALSE;

	lstrcpy( buf, lpszDir );
	int nFindCount = 0;
	while(*p)
	{
		if (*p == _T('/') || *p == _T('\\'))
		{
			nFindCount ++;
			if( nFindCount == nCount )
			{
				*(p+1) = _T('\0');
				if( (size_t)lstrlen(buf) > sizeBuf )
					return FALSE;
				else
				{
					lstrcpy(szBuff, buf);
					return TRUE;
				}
			}
		}

//        if (IsDBCSLeadByte((BYTE)*p))
  //          p++;

	//	p++;
		p = _tcsinc(p);
	}
	return FALSE;
}


BOOL CreateCompleteDirectory( LPCTSTR lpszDir, LPSECURITY_ATTRIBUTES pSA )
{
	if( PathIsDirectory(lpszDir) )
		return TRUE;

	TCHAR szBuff[_MAX_PATH];
	int i;
	for( i = 0; GetCountedDir( szBuff, lpszDir, sizeof(szBuff), i+1 ); i++ )
	{
		if( !PathIsDirectory(szBuff) )
		{//Dir not exist
			if(!CreateDirectory(szBuff, pSA))
				return FALSE;
		}
	}
	
	return CreateDirectory(lpszDir, pSA);
}