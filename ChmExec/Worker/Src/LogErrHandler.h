/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: Модуль исполнения задач Worker

	class CAppException - класс исключение модуля исполнения

	class CLogErrHandler - Журнал и обработчик ошибок
*/
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\include\defs.h"

//////////////////////////////////////////////////////////////////////////
//Модуль исполнения задач Worker
//////////////////////////////////////////////////////////////////////////
namespace LogErr
{
	const short nErrBufferSize = 256;

	//////////////////////////////////////////////////////////////////////////
	//class CAppException - класс исключение модуля исполнения
	//////////////////////////////////////////////////////////////////////////
	class CAppException : public CException
	{
	public:
		//szError - сообщение ошибки
		CAppException( PCTSTR szError )
			:m_strError( szError ){}

		//Ошибка
		cstring m_strError;
	};

	//////////////////////////////////////////////////////////////////////////
	//class CLogErrHandler - Журнал и обработчик ошибок
	//////////////////////////////////////////////////////////////////////////
	class CLogErrHandler
	{
	public:
		//strErrLog - вывод журнала и ошибок
		CLogErrHandler( cstring& strErrLog )
			:m_strErrLog( strErrLog ){}

		//обработка ошибок
		void OnError( const cstring& strError );
		//
		void OnError( const CAppException& e );
		
	private:
		//вывод журнала и ошибок
		cstring& m_strErrLog;
		//синх. доступа
		CCriticalSection m_cs;
	};

	//
	//Сообщения о ошибках
	//

	//Ошибка создания операции
	extern PCTSTR szErrMessageCreateOperation;

	//Некорректые входные данные
	extern PCTSTR szErrMessageInvalidArguments;

	//Ошибка исполнения задачи
	extern PCTSTR szErrMessageExecution;
}
