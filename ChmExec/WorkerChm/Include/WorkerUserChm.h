/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: Модуль реализации основной задачи WorkerChm.

	class CItem				- класс элемент исполнения 
	class COperationParse	- операция 1 - операция разбора файла chm
	class COperationSave	- операция 2 - операция сохранения
	class CTaskChm			- задача выполнения 
	class CWorkerChmFiles	- класс исполнитель задачи CTaskChm.
*/
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\..\Worker\Include\WorkerUser.h"
#include "..\..\Worker\src\LogErrHandler.h"
#include "..\..\Worker\src\Helpers.h"
#include "Files.h"
#include "..\src\Consts.h"

//////////////////////////////////////////////////////////////////////////
//Модуль реализации основной задачи WorkerChm.
//////////////////////////////////////////////////////////////////////////
namespace WorkerChmUser
{
	using namespace std;
	using namespace WorkerUser;
	using namespace FilesOp;
	using namespace LogErr;
	using Worker::GetNewStrGUID;

	//////////////////////////////////////////////////////////////////////////
	//class CHtmlPageInfo - класс информации разбора отдельной страницы.
	//////////////////////////////////////////////////////////////////////////
	class CHtmlPageInfo
	{
	public:
		//имя файла страницы
		cstring m_strHtmlName;
		//значение тега <Title> - заголовок страницы
		cstring m_strHtmlTitle;
	};

	//////////////////////////////////////////////////////////////////////////
	//class CItem - класс элемент исполнения 
	//////////////////////////////////////////////////////////////////////////
	class CItem
	{
	public:
		//имя файла chm
		cstring m_strNameChm;
		//результирующая директория
		cstring m_strResDir;
		//массив разбора 
		vector< CHtmlPageInfo > m_arrHtmlInfo;
	};

	//////////////////////////////////////////////////////////////////////////
	//class COperationParse - операция 1 - операция разбора файла chm
	//////////////////////////////////////////////////////////////////////////
	class COperationParse : public COperation< CItem >
	{
	public:
		//Выполнение операции разбора над элементом item
		virtual void Do( CItem& item );

	private:
		//////////////////////////////////////////////////////////////////////////
		//class COperationParse - вспомог. класс разбора страницы
		//////////////////////////////////////////////////////////////////////////
		class CParseHtmlPage : public unary_function< cstring , void >
		{
		public:
			//item - элемент разбора
			CParseHtmlPage( CItem& item )
				:m_item( item )
			{}

			//выполнение разбора
			void operator()( cstring strHtmlPage );

		private:
			//Получение относительного пути страницы в конт. chm
			//Возращает относит путь
			cstring GetRelationName( cstring strHtmlPage );

			//элемент разбора
			CItem& m_item;
		};

		//////////////////////////////////////////////////////////////////////////
		//class CParseDirectory - вспомог. класс разбора директории контейнера
		//////////////////////////////////////////////////////////////////////////
		class CParseDirectory : public unary_function< cstring , void >
		{
		public:
			//item - элемент разбора
			CParseDirectory( CItem& item )
				:m_item( item )
			{}

			//выполнение разбора
			void operator()( cstring strDirectory );

		private:
			//элемент разбора
			CItem& m_item;
		};
	};

	//////////////////////////////////////////////////////////////////////////
	//class COperationSave - операция 2 - операция сохранения
	//////////////////////////////////////////////////////////////////////////
	class COperationSave : public COperation< CItem >
	{
	public:
		//Выполнение операции сохранения над элементом item
		virtual void Do( CItem& item );
		
	private:
		//////////////////////////////////////////////////////////////////////////
		//class CSavePageInfo - вспомог. класс сохранения
		//////////////////////////////////////////////////////////////////////////
		class CSavePageInfo : public unary_function< CHtmlPageInfo , void >
		{
		public:
			//file - место хранения
			CSavePageInfo( cfstream& file )
				:m_file( file )
			{}

			////выполнение сохранения
			void operator()( CHtmlPageInfo info );

		private:
			//место хранения
			cfstream& m_file;
		};
	};

	//////////////////////////////////////////////////////////////////////////
	//class CTaskChm - задача выполнения 
	//////////////////////////////////////////////////////////////////////////
	class CTaskChm : public CTask<CItem>
	{
	public:
		//filesChm - входные файлы chm
		//strResDir - результирующая директория
		CTaskChm( Files& filesChm, const cstring& strResDir )
			:m_files(filesChm), 
			m_strResDir( strResDir )
		{}

		//заполнение входной очереди исполнения workQueue
		virtual void FillQueue( queue<CItem*>& workQueue );

		//создание операции 1 - разбора. Возращает операцию
		virtual COperation<CItem>* CreateOperation1();
		
		//создание операции 2 - сохранения. Возращает операцию
		virtual COperation<CItem>* CreateOperation2();

	private:
		//////////////////////////////////////////////////////////////////////////
		//class CNewItem - вспомогат. класс заполнения очереди 
		//////////////////////////////////////////////////////////////////////////
		class CNewItem : public unary_function< cstring , void >
		{
		public:
			//workQueue - очередь
			//strResDir - результ. директория
			CNewItem( queue<CItem*>& workQueue, const cstring& strResDir )
				:m_workQueue( workQueue ), 
				m_strResDir( strResDir )
			{}

			//Добавить элемент исполнения strChmName в очередь
			void operator()( cstring strChmName );
						
		private:
			//очередь заполнения
			queue< CItem* >& m_workQueue;
			//результ. директория
			const cstring& m_strResDir;
		};

		//входные файлы chm
		Files& m_files;
		//результирующая директория
		cstring m_strResDir;
	};

	//////////////////////////////////////////////////////////////////////////
	//class CWorkerChmFiles - класс исполнитель задачи CTaskChm.
	//Задачу выполняет модуль исполнения задач Worker.
	//////////////////////////////////////////////////////////////////////////
	class CWorkerChmFiles
	{
	public:
		//strLog - журнал
		//bThreadsOperationsPerProcessors - флаг запуска операций в отдельных рабочих потоках
		CWorkerChmFiles( cstring& strLog, bool bThreadsOperationsPerProcessors = false )
			:m_worker( strLog , bThreadsOperationsPerProcessors ),
			m_strLog( strLog )
		{}
	
		//Выполнение задачи над файлами chm - files, рез. дир - strResDir
		void Execute( Files& files, const cstring& strResDir );

	private:
		//Ведение журнала выполнения
		void LogStart();
		void LogEnd();

		void CleanResDir( const cstring& strResDir );

		//журнал
		cstring& m_strLog;
		//Исполнитель задачи
		CWorkerUser< CItem > m_worker;
	};
}