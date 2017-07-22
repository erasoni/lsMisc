#include "stdafx.h"
//#include <afx.h>
#include <windows.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include "../LCMapCommon.h"

using namespace std;

void testHira2Kata()
{
	std::wstring as(L"aaaa", 4);

	LPCTSTR p=_T("あああアアア ｱｱｱ");
	tstring s = Kata2Hira(p);
	OutputDebugString(s.c_str());
	OutputDebugString(_T("\r\n"));


	s=Hira2Kata(p);
	OutputDebugString(s.c_str());
	OutputDebugString(_T("\r\n"));
	
}