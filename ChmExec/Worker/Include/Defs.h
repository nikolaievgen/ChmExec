/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: Модуль исполнения задач Worker. Общие определения
*/
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <tchar.h>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <vector>
#include <queue>
#include <fstream>
#include <functional>

//
typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > cstring;
//
typedef std::basic_stringstream<TCHAR> cstringstream;
//
typedef std::basic_fstream<TCHAR> cfstream;
