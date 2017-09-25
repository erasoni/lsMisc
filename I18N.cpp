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

#pragma warning(disable :4786)
#pragma warning(disable: 4503)

#if !defined(UNICODE)
#error UNICODE required
#endif


#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <map>
#include <set>
#include <string>
using namespace std;


#include "I18N.h"

#ifndef countof
#define countof(a) (sizeof(a)/sizeof(a[0]))
#endif

namespace Ambiesoft {

static HINSTANCE ghInst;

static CRITICAL_SECTION cs;
static void UninitCS()
{
	DeleteCriticalSection(&cs);
}
static bool InitCS();
static bool bCinit = InitCS();
static bool InitCS()
{
	if(!bCinit)
	{
		InitializeCriticalSection(&cs);
		atexit(UninitCS);
	}
	return true;
}
struct i18nlock
{
	i18nlock()
	{
		EnterCriticalSection(&cs);
	}
	~i18nlock()
	{
		LeaveCriticalSection(&cs);
	}
};

struct CFreer {
	void* p_;
	CFreer(void* p) : p_(p){}
	~CFreer() {
		free(p_);
	}
};

struct FileFreer {
	FILE* f_;
	FileFreer(FILE* f) : f_(f) {}
	~FileFreer() {
		fclose(f_);
	}
};

struct stringcomparer
{
    bool operator()(LPCTSTR x, LPCTSTR y) const
    {
         return _tcscmp(x,y)>0;
    }
};
typedef std::map<LPCWSTR,wstring,stringcomparer> I18NSTRINGMAP;
static I18NSTRINGMAP i18map;


static BYTE* UTF16toUTF8(LPCWSTR pIN)
{
	int nReqSize = WideCharToMultiByte(CP_UTF8,
		0,
		pIN,
		-1,
		NULL,
		0,
		NULL,
		NULL);

	if ( nReqSize == 0 )
		return NULL;

	BYTE* pOut = (BYTE*)malloc(nReqSize);
	int nRet = WideCharToMultiByte(CP_UTF8,
		0,
		pIN,
		-1,
		(char*)pOut,
		nReqSize,
		NULL,
		NULL);

	if ( nRet==0 || nRet != nReqSize )
		return NULL;

	return pOut;;
}
static LPWSTR UTF8toUTF16(const LPBYTE pIN)
{
	
	int nReqSize = MultiByteToWideChar(
		CP_UTF8,
		0,
		(const char*)pIN,
		-1,
		NULL,
		0);

	if ( nReqSize == 0 )
		return NULL;

	LPWSTR pOut = (LPWSTR)malloc(nReqSize*sizeof(WCHAR));
	int nRet = MultiByteToWideChar(CP_UTF8,
		0,
		(const char*)pIN,
		-1,
		pOut,
		nReqSize);

	if ( nRet==0 || nRet != nReqSize )
		return NULL;

	return pOut;;
}
#ifdef _DEBUG
static std::set<wstring> nai;
static std::set<wstring> aru;
void shownai()
{
	WCHAR szModule[MAX_PATH] = {0};
	GetModuleFileNameW(ghInst, szModule, MAX_PATH);
	wstring strModule(szModule);

	std::set<wstring>::iterator it;
	wstring message;
	for ( it = nai.begin() ; it != nai.end() ; ++it )
	{
		if(it->empty())
			continue;
		message += *it;
		message += L"\r\n";
	}

	if (!message.empty())
	{
		message = L"---------------------NOTI18N-------" + strModule + L">>>>>>>>>>>>\r\n" + message;
		message += L"---------------------NOTI18N-------" + strModule + L"<<<<<<<<<<<<\r\n";
		OutputDebugStringW(message.c_str());
	}



	message = L"";
	for ( it = aru.begin() ; it != aru.end() ; ++it )
	{
		if(it->empty())
			continue;
		message += *it;
		message += L"\r\n";
	}

	if(!message.empty())
	{
		message = L"---------------------DUPLICATEI18N-------" + strModule + L">>>>>>>>>>>>\r\n" + message;
		message += L"---------------------DUPLICATEI18N-------" + strModule + L"<<<<<<<<<<<<\r\n";
		OutputDebugStringW(message.c_str());
	}
}
#endif  // _DEBUG


static bool langinit=false;
static WCHAR stLang[4];

LPCWSTR i18nGetCurrentLang()
{
	return stLang;
}

void ClearMap()
{
	for(I18NSTRINGMAP::iterator it = i18map.begin();
		it != i18map.end();
		++it)
	{
		free((void*)it->first);
	}
	i18map.clear();
}
LPCWSTR i18nInitLangmap(HINSTANCE hInst, LPCWSTR pLang, LPCWSTR pAppName)
{
	ghInst = hInst;

	WCHAR szLang[4];
	if(!pLang || pLang[0]==0)
	{
		::GetLocaleInfoW(LOCALE_SYSTEM_DEFAULT ,
						LOCALE_SABBREVLANGNAME,
						szLang, 
						4);

		pLang = szLang;
	}

	assert(pLang[0]==0 || lstrlen(pLang)==3);
	if(lstrcmpiW(pLang,stLang)!=0)
	{
		langinit = false;
		lstrcpyW(stLang, pLang);
		ClearMap();
	}
	
	//WCHAR szAppName[MAX_PATH]; szAppName[0]=0;
	//if(pAppName == NULL)
	//{
	//	GetModuleFileNameW(hInst, szAppName, countof(szAppName));
	//	*_tcsrchr(szAppName, L'.')=0;
	//	pAppName = _tcsrchr(szAppName, L'\\')+1;
	//}

	if(!langinit)
	{
		{
			i18nlock lock;
			if(!langinit)
			{
				do
				{
					TCHAR szLang[4];
					lstrcpy(szLang, stLang);
					{
						_tcslwr_s(szLang);
						TCHAR szT[MAX_PATH]={0};
						GetModuleFileNameW(hInst,szT,(sizeof(szT)/sizeof(szT[0]))-1);
						*(_tcsrchr(szT, L'\\'))=0;

						TCHAR szTry[MAX_PATH];
						if(pAppName && pAppName[0])
							wsprintfW(szTry, _T("%s\\lang\\%s.%s.txt"),szT, pAppName, szLang);
						else
							wsprintfW(szTry, _T("%s\\lang\\%s.txt"), szT, szLang);

						
						FILE* f = NULL;
						errno_t  err = _tfopen_s(&f, szTry, _T("rb"));
						if (err || !f)
						{
#ifdef _DEBUG
							wchar_t szT[1024]; szT[0] = 0;
							_wcserror_s(szT, err);
							wstring message;
							message.append(L"Failed to open lang-file: ");
							message.append(szT);
							message.append(L": ");
							message.append(szTry);
							message.append(L"\r\n");
							OutputDebugString(message.c_str());
#endif
							break;
						}

#ifdef _DEBUG
						{
							wstring message;
							message.append(L"Lang file opened: ");
							message.append(szTry);
							message.append(L"\r\n");
							OutputDebugString(message.c_str());
						}
#endif
						FileFreer ffreer(f);

						BYTE* pB=NULL;
						BYTE b;
						size_t count=0;
						size_t bomcount=0;
						while(1==fread((void*)&b, 1, 1, f))
						{
							if(count == 0 && bomcount < 3)
							{
								if(bomcount==0)
								{
									if(b==0xEF)
									{
										++bomcount;
										continue;
									}
									else
										bomcount=4;
								}
								else if(bomcount==1)
								{
									if(b==0xBB)
									{
										++bomcount;
										continue;
									}
									else
									{
										++count;
										pB = (BYTE*)realloc(pB, count);
										pB[count-1]=0xFE;

										bomcount=4;
									}

								}
								else if(bomcount==2)
								{
									if(b==0xBF)
									{
										++bomcount;
										continue;
									}
									else
									{
										++count;
										pB = (BYTE*)realloc(pB, count);
										pB[count-1]=0xFE;

										++count;
										pB = (BYTE*)realloc(pB, count);
										pB[count-1]=0xBB;

										bomcount=4;
									}
								}
							}
							++count;
							pB = (BYTE*)realloc(pB, count);
							pB[count-1]=b;
						}
						++count;
						pB = (BYTE*)realloc(pB, count);
						pB[count-1]=0;

						
						WCHAR* pA = (WCHAR*)UTF8toUTF16(pB);
						free(pB);
						CFreer pAFreer(pA);

						wchar_t *context = NULL;
						LPCWSTR pTok = wcstok_s(pA, L"\n", &context);
						while(pTok)
						{
							wstring left;
							wstring right;
							int phase=0;
							bool done = false;
							bool esc = false;
							for( ; *pTok ; ++pTok)
							{
								if(phase==0)
								{
									if(*pTok==L'#')
									{
										done=true;
										break;
									}

									if(*pTok != L'"')
										continue;

									phase = 1;
									continue;
								}
								else if(phase==1)
								{
									if(esc)
									{
										esc = false;
										if(*pTok==L'n')
										{
											left+=L"\n";
										}
										else if(*pTok==L'r')
										{
											left+=L"\r";
										}
										else
											left+=*pTok;
										continue;
									}
									if(*pTok==L'\\')
									{
										esc = true;
										continue;
									}
									if(*pTok==L'"')
									{
										phase=2;
										continue;
									}
									left+=*pTok;
									continue;
								}
								else if(phase==2)
								{
									if(*pTok != L'=')
										continue;

									phase = 3;
								}
								else if(phase==3)
								{
									if(*pTok != L'"')
										continue;

									phase = 4;
									continue;
								}
								else if(phase==4)
								{
									if(esc)
									{
										esc = false;
										if(*pTok==L'n')
										{
											right+=L"\n";
										}
										else if(*pTok==L'r')
										{
											right+=L"\r";
										}
										else
										{
											right+=*pTok;
										}
										continue;
									}
									if(*pTok==L'\\')
									{
										esc = true;
										continue;
									}
									if(*pTok==L'"')
									{
										phase=5;
										continue;
									}
									right+=*pTok;
									continue;
								}
								else if(phase==5)
								{
									break;
								}
								else
								{
									assert(false);
								}
							}
							
							if(!left.empty())
							{
#ifdef _DEBUG
								if(!i18map[_wcsdup(left.c_str())].empty())
								{
									aru.insert(left);
								}
#endif
								i18map[_wcsdup(left.c_str())] = right;
							}

							pTok = wcstok_s(NULL, L"\n", &context);
						}

					}


				} while(0);
				
#ifdef _DEBUG
				atexit(shownai);
#endif
			}
			langinit=true;
		}
	}

	return stLang;
}

LPCWSTR I18NW(LPCWSTR pIN)
{
	if(!bCinit)
		InitCS();

	if(!langinit)
		i18nInitLangmap();

	i18nlock lock;
	wstring& ret = i18map[pIN];
	if(!ret.empty())
		return ret.c_str();

#ifdef _DEBUG
	nai.insert(pIN);
#endif
	return pIN;
}





static BOOL CALLBACK enumDlgChild(
  HWND hwnd,
  LPARAM lParam)
{
	TCHAR szClass[256];
	if(!GetClassNameW(hwnd, szClass, countof(szClass)))
		return TRUE;

	if(lstrcmpiW(szClass, _T("Static"))!=0 &&
	   lstrcmpiW(szClass, _T("Button"))!=0 &&
	   lstrcmpiW(szClass, _T("ScrollBar"))!=0 )
	{
		return TRUE;
	}

	i18nChangeWindowText(hwnd);

	return TRUE;
}

void i18nChangeWindowText(HWND hwnd)
{
	int size = ::GetWindowTextLength(hwnd);
	if(size)
	{
		LPTSTR p = new TCHAR[size+1];
		if(size == ::GetWindowTextW(hwnd, p, size+1))
		{
			::SetWindowTextW(hwnd, I18N(p));
		}
		delete[] p;
	}
}
void i18nChangeChildWindowText(HWND hDlg)
{
	EnumChildWindows(hDlg,
		enumDlgChild,
		0);
}

void i18nChangeMenuText(HMENU menu)
{
	int count = GetMenuItemCount(menu);
	for(int i=0 ; i<count ; ++i)
	{
		int len = GetMenuStringW(menu, i, NULL, 0, MF_BYPOSITION|MFT_STRING);
		int buffsize = (len+1)*sizeof(TCHAR);
		TCHAR* pBuff = (TCHAR*)malloc(buffsize);
		CFreer maBuff(pBuff);
		GetMenuStringW(menu,i,pBuff,len+1,MF_BYPOSITION|MFT_STRING);

		LPWSTR text = _wcsdup(I18N(pBuff));
		CFreer maText(text);
		

		MENUITEMINFO miiset;
		miiset.cbSize=sizeof(miiset);
		miiset.fMask = 0x00000040; //MIIM_STRING;
		miiset.dwTypeData = text;
		SetMenuItemInfoW(menu,i,TRUE, &miiset);
		


		MENUITEMINFO  mii;
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_SUBMENU ;
		
		GetMenuItemInfoW(menu, i, TRUE, &mii);

		if(mii.hSubMenu)
		{
			i18nChangeMenuText(mii.hSubMenu);
		}

		
	}
}



} // namespace Ambiesoft