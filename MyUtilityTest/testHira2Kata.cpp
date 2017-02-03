#include "stdafx.h"
#include <afx.h>
#include <windows.h>
#include <string>

#include "../LCMapCommon.h"

void testHira2Kata()
{
	std::wstring as(L"aaaa", 4);
	LPCWSTR p=L"あああアアア ｱｱｱ";
	CString s = Kata2Hira(p);
	TRACE(s);
	s=Hira2Kata(p);
	TRACE(s);
}