
/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: Модуль исполнения задач Worker

	class CAppException - класс исключение модуля исполнения

	class CLogErrHandler - Журнал и обработчик ошибок
*/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogErrHandler.h"

//
//Сообщения о ошибках
//

PCTSTR LogErr::szErrMessageCreateOperation	= _T("Error worker : Create Operation failed.");
PCTSTR LogErr::szErrMessageInvalidArguments	= _T("Error worker : Invalid parameters.");
PCTSTR LogErr::szErrMessageExecution		= _T("Error worker : Execution error.");

using namespace LogErr;

//////////////////////////////////////////////////////////////////////////
//обработка ошибок
void CLogErrHandler::OnError( const cstring& strError )
{
	CSingleLock lock( &m_cs, TRUE );

	if ( strError.empty() )
	{
		m_strErrLog += _T("Unknown error \r\n");
		return;
	}

	m_strErrLog += _T("ExeceptionError : ");
	m_strErrLog += strError;
	m_strErrLog += _T("\r\n");

	return;
}

//////////////////////////////////////////////////////////////////////////
//обработка ошибок
void CLogErrHandler::OnError( const CAppException& e )
{
	CSingleLock lock( &m_cs, TRUE );

	m_strErrLog += _T("ExecutionError : ");
	m_strErrLog += e.m_strError;
	m_strErrLog += _T("\r\n");

	return;
}

//////////////////////////////////////////////////////////////////////////
