/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: Модуль исполнения задач Worker. Интерфейс пользователя.

	WorkerUser.h - Интерфейс пользователя модуля.
*/
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"
#include "..\src\Worker.h"

//////////////////////////////////////////////////////////////////////////
//Модуль исполнения задач Worker
//////////////////////////////////////////////////////////////////////////
namespace WorkerUser
{
	using namespace Worker;

	//////////////////////////////////////////////////////////////////////////
	//class CWorkerUser - класс функтор, запускающий Worker.
	//////////////////////////////////////////////////////////////////////////
	template<typename Item >
	class CWorkerUser : unary_function< CTask<Item>& , void >
	{
	public:
		//
		//strLog - журнал, bThreadsOperationsPerProcessors - флаг исполнения операций в разных рабочих потоках
		//
		CWorkerUser( cstring& strLog, bool bThreadsOperationsPerProcessors )
			:m_strLog(strLog), m_bThreadsOperationsPerProcessors(bThreadsOperationsPerProcessors)
		{}

		//
		void operator()(CTask<Item>& task );
	private:
		//журнал
		cstring& m_strLog;

		//флаг исполнения операций в разных рабочих потоках
		bool m_bThreadsOperationsPerProcessors;
	};

	//////////////////////////////////////////////////////////////////////////

	template<typename Item >
	void CWorkerUser<Item>::operator()(CTask<Item>& task )
	{
		CWorker< Item > worker( m_strLog, task );

		if ( m_bThreadsOperationsPerProcessors )
		{
			SYSTEM_INFO siSysInfo = { 0 };
			GetSystemInfo(&siSysInfo);

			int nThreads = siSysInfo.dwNumberOfProcessors > 2 ? siSysInfo.dwNumberOfProcessors / 2 : 1;

			worker.SetCountThreads( nThreads , nThreads );
			worker.ExecuteThreads();
		}
		else
		{
			worker.Execute();
		}

		return;
	}

	//////////////////////////////////////////////////////////////////////////

}