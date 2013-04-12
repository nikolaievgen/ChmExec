/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: Модуль исполнения задач Worker.

	class CWorker - класс исполнитель задачи CTask.
*/
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\include\User.h"
#include "LogErrHandler.h"
#include "WorkQueue.h"

//////////////////////////////////////////////////////////////////////////
//Модуль исполнения задач Worker
//////////////////////////////////////////////////////////////////////////
namespace Worker
{
	using namespace std;
	using namespace LogErr;
	using namespace WorkerUser;

	//////////////////////////////////////////////////////////////////////////
	//class CWorker - класс исполнитель задачи CTask.
	//////////////////////////////////////////////////////////////////////////
	template<typename Item >
	class CWorker 
	{
	public:
		//
		//strLog - журнал, task - задача
		//
		CWorker(cstring& strLog, CTask<Item>& task )
			:m_errorHandler( strLog ), m_task(task), m_nCountThreadsOperation1(1), m_nCountThreadsOperation2(1) {}

		//установить число рабочих потоков на операцию 1 и 2
		void SetCountThreads( int nCountOperation1, int nCountOperation2 );

		//Запустить исполнение задачи в рабочих потоках. 
		virtual bool ExecuteThreads();
		
		//Выполнить задачу в текущем потоке
		virtual bool Execute();

	private:
		//задача
		CTask<Item>& m_task;
		//Журнал и обработчик ошибок
		CLogErrHandler m_errorHandler;

		//число рабочих потоков на операцию
		int m_nCountThreadsOperation1;
		int m_nCountThreadsOperation2;

		//////////////////////////////////////////////////////////////////////////
		//class CContext - контекст исполнения рабочего потока.
		//////////////////////////////////////////////////////////////////////////
		template<typename Item >
		class CContext
		{
		public:
			//
			//operation - операция, errorHandler - журнал, 
			//queueItemsInput - входная очередь элементов
			//queueItemsOutput - выходная очередь элементов
			//
			CContext(COperation<Item>& operation, CLogErrHandler& errorHandler, 
				cworkqueue<Item>& queueItemsInput, cworkqueue<Item>& queueItemsOutput )
				:m_operation( operation ), 
				m_errorHandler(errorHandler), 
				m_queueItemsInput(queueItemsInput), 
				m_queueItemsOutput(queueItemsOutput)
			{}

			//операция
			COperation<Item>& m_operation;
			//журнал
			CLogErrHandler& m_errorHandler;
			//входная очередь элементов
			cworkqueue<Item>& m_queueItemsInput;
			//выходная очередь элементов
			cworkqueue<Item>& m_queueItemsOutput;
		};

		//функция рабочего потока
		static unsigned __stdcall ThreadOperation( void* pArg );
	};

	//////////////////////////////////////////////////////////////////////////
	//установить число рабочих потоков на операцию 1 и 2
	template<typename Item >
	void CWorker<Item>::SetCountThreads( int nCountOperation1, int nCountOperation2 )
	{
		m_nCountThreadsOperation1 = nCountOperation1;
		m_nCountThreadsOperation2 = nCountOperation2;
	}

	//////////////////////////////////////////////////////////////////////////
	//функция рабочего потока
	template<typename Item >
	unsigned __stdcall CWorker<Item>::ThreadOperation( void* pArg )
	{
		CContext<Item>* pContext = static_cast<CContext<Item>*>( pArg );

		if ( !pContext )
			return false;

		bool bSuccess = true;

		Item* pItem = 0;

		try
		{
			while ( pItem = pContext->m_queueItemsInput.popitem() )
			{
				pContext->m_operation.Do( *pItem );

				pContext->m_queueItemsOutput.pushitem( pItem );
			}
		}
		catch( CAppException* e )
		{
			pContext->m_errorHandler.OnError( *e );

			e->Delete();

			bSuccess = false;

			delete pItem;
		}
		catch( CException* e )
		{
			TCHAR szErrBuff[ nErrBufferSize ];
			e->GetErrorMessage( szErrBuff , nErrBufferSize );

			pContext->m_errorHandler.OnError( cstring( szErrBuff ) );

			e->Delete();

			bSuccess = false;

			delete pItem;
		}
		catch( exception e )
		{
			TCHAR szErrBuff[ nErrBufferSize ];

			MultiByteToWideChar( CP_THREAD_ACP, 0, e.what(), -1 , szErrBuff , nErrBufferSize / sizeof( TCHAR ) );

			pContext->m_errorHandler.OnError( cstring( szErrBuff ) );

			bSuccess = false;
		}
		catch( ... )
		{
			pContext->m_errorHandler.OnError( cstring() );

			bSuccess = false;

			delete pItem;
		}

		return bSuccess;
	}

	//////////////////////////////////////////////////////////////////////////
	//Запустить исполнение задачи в рабочих потоках
	template<typename Item >
	bool CWorker<Item>::ExecuteThreads()
	{
		bool bSuccess = true;

		try
		{
			if ( !m_nCountThreadsOperation1 && !m_nCountThreadsOperation2 )
				throw new CAppException( szErrMessageInvalidArguments );

			//очереди элементов исполнения
			cworkqueuesafe<Item>				inputQueue;
			cworkqueuesafewaitableinput<Item>	operationQueue1;
			cworkqueuesafe<Item>				operationQueue2;

			m_task.FillQueue( inputQueue );

			//вспомогательные объекты хранения
			CAutoFreeVectorPtrs<COperation<Item>*>	operations;
			CAutoFreeVectorPtrs<CContext<Item>*>	contexts;
			CAutoFreeHandles						handlesOperation1( m_nCountThreadsOperation1 );
			CAutoFreeHandles						handlesOperation2( m_nCountThreadsOperation2 );

			COperation<Item>* pOperation = NULL;

			for ( int i = 0 ; i < m_nCountThreadsOperation1; i++ )
			{//запуск рабочих потоков 1 операции

				if( !(pOperation = m_task.CreateOperation1()) )
					throw new CAppException( szErrMessageCreateOperation );
				
				operations.push_back( pOperation );
				contexts.push_back( new CContext<Item>( *pOperation, m_errorHandler, inputQueue, operationQueue1 ) );


				handlesOperation1[ i ] =
					(HANDLE)_beginthreadex( 0, 0, CWorker<Item>::ThreadOperation, contexts.back(), 0, 0 );
			}

			for ( int i = 0 ; i < m_nCountThreadsOperation2; i++ )
			{//запуск рабочих потоков 2 операции

				if( !(pOperation = m_task.CreateOperation2()) )
					throw new CAppException( szErrMessageCreateOperation );

				operations.push_back( pOperation );
				contexts.push_back(  new CContext<Item>( *pOperation, m_errorHandler, operationQueue1 , operationQueue2 ) );

				handlesOperation2[ i ] = 
					(HANDLE)_beginthreadex( 0, 0, CWorker<Item>::ThreadOperation, contexts.back(), 0, 0 );
			}

			WaitForMultipleObjects( handlesOperation1.size(), &handlesOperation1[ 0 ], TRUE, INFINITE );

			//исполнение 1 операции закончилось, поэтому прекращаем ожидание элементов
			operationQueue1.setendwait();

			WaitForMultipleObjects( handlesOperation2.size(), &handlesOperation2[ 0 ], TRUE, INFINITE );
		}
		catch( CAppException* e )
		{
			m_errorHandler.OnError( *e );

			e->Delete();

			bSuccess = false;
		}
		catch( ... )
		{
			m_errorHandler.OnError( cstring() );

			bSuccess = false;
		}

		return bSuccess;
	}

	//////////////////////////////////////////////////////////////////////////
	//Выполнить задачу в текущем потоке
	template<typename Item >
	bool CWorker<Item>::Execute()
	{
		bool bSuccess = true;

		try
		{
			//очереди элементов исполнения
			cworkqueue<Item> operationQueue1;
			cworkqueue<Item> operationQueue2;

			COperation<Item>* pOperation = NULL;

			m_task.FillQueue( operationQueue1 );

			//вспомогательные объекты хранения
			CAutoFreeVectorPtrs<COperation<Item>*> operations;
			CAutoFreeVectorPtrs<CContext<Item>*> contexts;

			if( !(pOperation = m_task.CreateOperation1()) )
				throw new CAppException( szErrMessageCreateOperation );

			operations.push_back( pOperation );
			contexts.push_back(  new CContext<Item>( *pOperation, m_errorHandler, operationQueue1, operationQueue2 ) );

			//выполнение 1 операции
			if( !CWorker<Item>::ThreadOperation( contexts.back() ) )
				throw new CAppException( szErrMessageExecution );

			if( !(pOperation = m_task.CreateOperation2()) )
				throw new CAppException( szErrMessageCreateOperation );

			operations.push_back( pOperation );
			contexts.push_back( new CContext<Item>( *pOperation, m_errorHandler, operationQueue2 , operationQueue1 ) );

			//выполнение 2 операции
			if( !CWorker<Item>::ThreadOperation( contexts.back() ) )
				throw new CAppException( szErrMessageExecution );
		}
		catch( CAppException* e )
		{
			m_errorHandler.OnError( *e );

			e->Delete();

			bSuccess = false;
		}
		catch( ... )
		{
			m_errorHandler.OnError( cstring() );

			bSuccess = false;
		}

		return bSuccess;
	}

	//////////////////////////////////////////////////////////////////////////
}