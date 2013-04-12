/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: Модуль исполнения задач Worker
	Вспомогательные средства

	class CAutoFreeVectorPtrs	- класс хранитель, автоматически освобождает память
	class CAutoFreeHandles		- класс хранитель, автоматически освобождает объекты системы
	class CTimer				- класс таймер
*/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Helpers.h"

using namespace Worker;

//////////////////////////////////////////////////////////////////////////

cstring Worker::GetNewStrGUID()
{
	GUID guid;
	cstring strGuid;
	OLECHAR strOleBuff[ 80 ]; 

	::CoCreateGuid( &guid );

	::StringFromGUID2( guid, strOleBuff, 40 );

	strGuid = strOleBuff;

	//удаление {}
	{
		cstring::size_type nErase = strGuid.find( TCHAR('{') );

		if ( nErase != string::npos )
			strGuid.erase( nErase, 1 );

		nErase = strGuid.find( TCHAR('}') );

		if ( nErase != string::npos )
			strGuid.erase( nErase, 1 );
	}

	return strGuid;
}

//////////////////////////////////////////////////////////////////////////

cstring CTimer::StopGetTime()
{
	//конец измерений
	LARGE_INTEGER nTimerStop;
	QueryPerformanceCounter( &nTimerStop );

	//частота таймера
	LARGE_INTEGER nFrequency;
	QueryPerformanceFrequency( &nFrequency );

	cstringstream strStream;

	strStream << _T("Elapsed time: ");
	strStream << ( nTimerStop.QuadPart - m_nTimerStart.QuadPart ) * 1000 / nFrequency.QuadPart;
	strStream << _T(" ms. ");

	return strStream.str();
}

//////////////////////////////////////////////////////////////////////////

CTimer::CTimer()
{
	//начало измерений
	QueryPerformanceCounter( &m_nTimerStart );
}

//////////////////////////////////////////////////////////////////////////
