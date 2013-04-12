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

#pragma once

#include "..\include\defs.h"

//////////////////////////////////////////////////////////////////////////
//Модуль исполнения задач Worker
//Вспомогательные средства
//////////////////////////////////////////////////////////////////////////
namespace Worker
{
	using namespace std;

	//Получить GUID. Возращает GUID в строке.
	cstring GetNewStrGUID();

	//////////////////////////////////////////////////////////////////////////
	//class CAutoFreeVectorPtrs - класс хранитель, автоматически освобождает 
	//память
	//////////////////////////////////////////////////////////////////////////
	template<typename Item>
	class CAutoFreeVectorPtrs : public vector< Item >
	{
	public:
		//
		CAutoFreeVectorPtrs(){}

		//
		~CAutoFreeVectorPtrs()
		{
			for_each( begin(), end(), CAutoFreeVectorPtrs::FreeItem );
		}
	private:
		//освободить элемент
		static void FreeItem(Item item)
		{
			delete item;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//class CAutoFreeHandles - класс хранитель, автоматически освобождает 
	//объекты системы
	//////////////////////////////////////////////////////////////////////////
	class CAutoFreeHandles : public vector< HANDLE >
	{
	public:
		//
		CAutoFreeHandles(int nCountHandles )
			:vector< HANDLE >( nCountHandles )
		{
			if ( size() )
				ZeroMemory( &(*begin()) , size() );
		}

		//
		~CAutoFreeHandles()
		{
			for_each( begin(), end(), ::CloseHandle );
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//class CTimer - класс таймер
	//////////////////////////////////////////////////////////////////////////
	class CTimer
	{
	public:
		//
		CTimer();

		//Возращает в строке время с момента создания в мс.
		cstring StopGetTime();

	private:
		LARGE_INTEGER m_nTimerStart;
	};
}