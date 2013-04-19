/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: Модуль исполнения задач Worker.

	class cworkqueue, cworkqueuesafe, cworkqueuesafewaitableinput - классы очередей исполнения.

	class cworkqueue - интерфес использования очереди исполнения.

		class cworkqueuesafe - очередь исполнения между несколькими рабочими потоками.

			class cworkqueuesafewaitableinput - входная очередь исполнения и добавления элементов 
												между несколькими рабочими потоками.

*/
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\include\defs.h"

//////////////////////////////////////////////////////////////////////////
//Модуль исполнения задач Worker
//////////////////////////////////////////////////////////////////////////
namespace Worker
{
	using namespace std;

	//////////////////////////////////////////////////////////////////////////
	//class CWorker - класс интерфес использования очереди исполнения.
	//////////////////////////////////////////////////////////////////////////
	template<typename Item >
	class cworkqueue : public queue<Item*>
	{
	public:
		//
		cworkqueue(){};
		//
		virtual ~cworkqueue()
		{
			Item* pItem = NULL;

			while ( pItem = popitem() )
				delete pItem;
		};

		//получить элемент исполнения. Возращает NULL при отсутствии элементов
		virtual Item* popitem()
		{
			if ( empty() )
				return NULL;

			Item* pItem = front();

			pop();

			return pItem;
		}
		//добавить элемент в очередь 
		virtual void pushitem( Item* pItem )
		{
			push( pItem );

			return;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//class cworkqueuesafe - очередь исполнения между несколькими рабочими потоками.
	//////////////////////////////////////////////////////////////////////////
	template<typename Item >
	class cworkqueuesafe : public cworkqueue<Item>
	{
	public:
		//получить элемент исполнения. Возращает NULL при отсутствии элементов
		virtual Item* popitem()
		{
			CSingleLock lock( &m_cs , TRUE );

			return cworkqueue::popitem();;
		}
		//добавить элемент в очередь 
		virtual void pushitem( Item* pItem )
		{
			CSingleLock lock( &m_cs , TRUE );

			queue::push( pItem );

			return;
		}

	protected:
		//объект синхронизации доступа
		CCriticalSection m_cs;
	};

	//////////////////////////////////////////////////////////////////////////
	//class cworkqueuesafewaitableinput - входная очередь исполнения и добавления элементов 
	//между несколькими рабочими потоками.
	//////////////////////////////////////////////////////////////////////////
	template<typename Item >
	class cworkqueuesafewaitableinput : public cworkqueuesafe<Item>
	{
	public:
		//
		cworkqueuesafewaitableinput()
			:m_bEndWaitInput( false )
		{
			InitializeConditionVariable( &m_cvReadyToPop );
		};

		//выставить флаг прекращения ожидания
		void setendwait()
		{
			m_cs.Lock();

			m_bEndWaitInput = true;

			m_cs.Unlock();

			WakeAllConditionVariable( &m_cvReadyToPop );
		}

		//получить элемент исполнения. Возращает NULL при отсутствии элементов
		virtual Item* popitem()
		{
			CSingleLock lock( &m_cs , TRUE );

			while( empty() && !m_bEndWaitInput )
			{
				//пока очередь пуста и не выставлен флаг прекращения ожидания
				//необходимо ждать поступления элементов
				SleepConditionVariableCS( &m_cvReadyToPop, &m_cs.m_sect, INFINITE );
			}

			return cworkqueuesafe::popitem();
		}

		//добавить элемент в очередь 
		virtual void pushitem( Item* pItem )
		{
			cworkqueuesafe::pushitem( pItem );

			WakeConditionVariable( &m_cvReadyToPop );

			return;
		}

	protected:
		//флаг прекращения ожидания
		bool m_bEndWaitInput;
		//объект ожидания поступдения элементов
		CONDITION_VARIABLE m_cvReadyToPop; 
	};
}