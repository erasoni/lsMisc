#include "stdafx.h"
#include <tchar.h>
#include <iostream>
#include <sstream>
#include <gtest/gtest.h>
#include "../CommandLineParser.h"



using namespace Ambiesoft;
using namespace std;

template <class InputIterator>
void outvalue(wstringstream& os, InputIterator first, InputIterator last)
{
	while (first != last) {
		os << *first;
		++first;
	}
}

TEST(CommandLineParser, IterateOur)
{
	vector<wstring> vs;
	vs.push_back(L"-a");
	vs.push_back(L"-b");
	vs.push_back(L"-c");

	wstringstream wss;
	outvalue(wss, vs.begin(), vs.end());
	EXPECT_TRUE(wss.str() == L"-a-b-c");
}
#define ACTULAPATH L"c:\\aaa\ttt bbb\aa"

TEST(CommandLineParser, Basic)
{
	vector<wstring> vs;
	vs.push_back(L"-a");
	vs.push_back(L"-b");
	vs.push_back(L"-c");

	wchar_t* opXYZ[] = {
		L"-xyz",
		L"-bbb",
	};

	bool isHelp = false;
	bool isABC = false;
	bool isXYZ = false;
	wstring path;
	COption opMain(L"", ArgCount::ArgCount_Infinite);
	CCommandLineParser clp;
	clp.AddOption(L"-h", L"/?", 0, &isHelp);
	clp.AddOptionRange(vs.begin(), vs.end(), 0, &isABC);
	clp.AddOptionRange(opXYZ, opXYZ + _countof(opXYZ), 0, &isXYZ);
	clp.AddOption(L"-path", 1, &path);
	clp.AddOption(&opMain);
	wchar_t* argv[] = {
		L"exe.exe",
		L"-h",
		L"-b",
		L"-path",
		ACTULAPATH,
		L"file1.txt",
		L"file2.txt",
		L"file3.txt",
		L"space file.txt",
		NULL
	};
	clp.Parse(_countof(argv)-1, argv);

	EXPECT_TRUE(isHelp);
	EXPECT_TRUE(isABC);
	EXPECT_STREQ(path.c_str(), ACTULAPATH);
	
	EXPECT_EQ(opMain.getValueCount(), 4);
	EXPECT_STREQ(opMain.getFirstValue().c_str(), L"file1.txt");
	EXPECT_STREQ(opMain.getValue(0).c_str(), L"file1.txt");
	EXPECT_STREQ(opMain.getValue(1).c_str(), L"file2.txt");
	EXPECT_STREQ(opMain.getValue(2).c_str(), L"file3.txt");
	EXPECT_STREQ(opMain.getValue(3).c_str(), L"space file.txt");
	EXPECT_STREQ(opMain.getValueStrings().c_str(), L"file1.txt file2.txt file3.txt \"space file.txt\"");
}

TEST(CommandLineParser, OptionConstructorAll)
{
	// Default Ctor for Main arg and inifinite
	{
		CCommandLineParser parser;
		COption op1;
		parser.AddOption(&op1);

		wchar_t* argv[] = {
			L"exe.exe",
			L"-h",
			L"-b",
			L"-path",
			ACTULAPATH,  // Main arg starts here
			L"file1.txt",
			L"file2.txt",
			L"file3.txt",
			L"space file.txt",
			NULL
		};

		size_t argc = _countof(argv) - 1;
		parser.Parse(argc, argv);
		int i = 0;
		for (size_t iArg = 4; iArg < argc; ++iArg, ++i)
		{
			EXPECT_STREQ(argv[iArg], op1.getValue(i).c_str());
		}
	}
}

TEST(CommandLineParser, Bool)
{
	// Specified
	{
		CCommandLineParser parser;
		bool b = false;
		parser.AddOption(L"-b", 0, &b);

		wchar_t* argv[] = {
			L"exe.exe",
			L"-h",
			L"-b",
			L"-path",
			ACTULAPATH,  // Main arg starts here
			L"file1.txt",
			L"file2.txt",
			L"file3.txt",
			L"space file.txt",
			NULL
		};
		size_t argc = _countof(argv) - 1;
		parser.Parse(argc, argv);

		EXPECT_TRUE(b);
	}
	// UnSpecified
	{
		CCommandLineParser parser;
		bool b = false;
		parser.AddOption(L"-b", 0, &b);

		wchar_t* argv[] = {
			L"exe.exe",
			L"-h",
			L"-cb",
			L"-path",
			ACTULAPATH,  // Main arg starts here
			L"file1.txt",
			L"file2.txt",
			L"file3.txt",
			L"space file.txt",
			NULL
		};
		size_t argc = _countof(argv) - 1;
		parser.Parse(argc, argv);

		EXPECT_FALSE(b);
	}
}

TEST(CommandLineParser, Int)
{
	// Specified
	{
		CCommandLineParser parser;
		int intval = 0;
		parser.AddOption(L"-i", 1, &intval);

		wchar_t* argv[] = {
			L"exe.exe",
			L"-h",
			L"-i",
			L"1001",
			L"-path",
			ACTULAPATH,  // Main arg starts here
			L"file1.txt",
			L"file2.txt",
			L"file3.txt",
			L"space file.txt",
			NULL
		};
		size_t argc = _countof(argv) - 1;
		parser.Parse(argc, argv);

		EXPECT_EQ(intval, 1001);
	}
	// Unspecified
	{
		CCommandLineParser parser;
		int intval = -1;
		parser.AddOption(L"-i", 1, &intval);

		wchar_t* argv[] = {
			L"exe.exe",
			L"-h",
			L"-fi",
			L"1001",
			L"-path",
			ACTULAPATH,  // Main arg starts here
			L"file1.txt",
			L"file2.txt",
			L"file3.txt",
			L"space file.txt",
			NULL
		};
		size_t argc = _countof(argv) - 1;
		parser.Parse(argc, argv);

		EXPECT_EQ(intval, -1);
	}
}