/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: class CChmExecDlg - класс основного диалога приложения.
	ChmExecDlg.cpp - часть реализации CChmExecDlg, отвечающаяя за функциональную часть.
*/
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ChmExec.h"
#include "ChmExecDlg.h"
#include "WorkerChm/Include/WorkerUserChm.h"

using namespace std;
using namespace FilesOp;
using namespace WorkerChmUser;

extern LONG nMessageEndOperation;

//////////////////////////////////////////////////////////////////////////

cstring			CChmExecDlg::CChmExecMediator::m_strResDir;
FilesOp::Files	CChmExecDlg::CChmExecMediator::m_files;
bool			CChmExecDlg::CChmExecMediator::m_bThreadsOperations;
HWND			CChmExecDlg::CChmExecMediator::m_hWndNotify;
cstring			CChmExecDlg::CChmExecMediator::m_strLogErrs;
CCriticalSection CChmExecDlg::CChmExecMediator::m_cs;

//////////////////////////////////////////////////////////////////////////

CChmExecDlg::CChmExecDlg(CWnd* pParent )
	: CDialog(CChmExecDlg::IDD, pParent)
	, m_nConfig(N_CONFIG_THREAD_MAIN)
	, m_strResDir(_T(""))
	, m_strLogErrs(_T(""))
	, m_bOperation(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

//////////////////////////////////////////////////////////////////////////

void CChmExecDlg::OnOK()
{
	if ( m_bOperation )
		return;

	UpdateData( TRUE );

	UpdateDlgOpeartion( true );

	//запуск задачи на выполнение 
	switch ( m_nConfig )
	{
	case N_CONFIG_THREAD_MAIN:
		{//Запустить в главном потоке
			ExecuteThreadMain();
		}
		break;
	case N_CONFIG_THREAD_SINGLE:
		{//Выполнить в рабочем потоке
			ExecuteThread( false );
		}
		break;
	case N_CONFIG_THREAD_MULTI:
		{//Запустить в рабочем, а операции выполнять в отдельных рабочих
			ExecuteThread( true );
		}
		break;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
//выполнение задачи в главном потоке
void CChmExecDlg::ExecuteThreadMain()
{
	Files	files;
	cstring	strLogErrs;

	GetFiles( files );

	CWorkerChmFiles worker( strLogErrs, false );
	//выполнение задачи
	worker.Execute( files , m_strResDir );

	m_strLogErrs.Insert( 0, strLogErrs.c_str() );
	UpdateData( FALSE );
		
	UpdateDlgOpeartion( false );

	return;
}

//////////////////////////////////////////////////////////////////////////
//запуск на выполнение задачи в рабочем потоке
//bThreadsOperationsPerProcessors - флаг запуска операций 
void CChmExecDlg::ExecuteThread(bool bThreadsOperationsPerProcessors )
{
	Files	files;
	GetFiles( files );

	//запуск задачи в главном потоке
	CChmExecMediator::ExecThread( m_strResDir , files, GetSafeHwnd(), bThreadsOperationsPerProcessors );

	return;
}

//////////////////////////////////////////////////////////////////////////
//Выполнить задачу 
void CChmExecDlg::CChmExecMediator::ExecThread(const cstring& strResDir, Files& files, HWND hWnd, bool bThreadsOperations )
{
	CSingleLock lock( &m_cs, TRUE );

	m_files.swap( files );
	m_hWndNotify = hWnd;
	m_strResDir = strResDir;
	m_bThreadsOperations = bThreadsOperations;
	m_strLogErrs.clear();

	//запуск рабочего выполняющего потока
	::CloseHandle( (HANDLE)_beginthreadex( 0, 0, CChmExecMediator::ThreadOperation, 0, 0, 0 ) );

	return;
}

//////////////////////////////////////////////////////////////////////////
//Получение журнала выполнения задачи
cstring CChmExecDlg::CChmExecMediator::GetLogErrs()
{
	CSingleLock lock( &m_cs, TRUE );

	return m_strLogErrs;
}

//////////////////////////////////////////////////////////////////////////
//проверка корректности аргументов
bool CChmExecDlg::CChmExecMediator::Check()
{
	return	m_hWndNotify && ::IsWindow( m_hWndNotify ) &&
			m_files.size() &&
			m_strResDir.size();
}

//////////////////////////////////////////////////////////////////////////
//функция запускающего потока 
unsigned __stdcall CChmExecDlg::CChmExecMediator::ThreadOperation( void* pArg )
{
	CSingleLock lock( &m_cs, TRUE );

	if ( !Check() )
		return false;

	CWorkerChmFiles worker( m_strLogErrs, m_bThreadsOperations );

	//выполнение задачи
	worker.Execute( m_files , m_strResDir );

	//уведомить UI о выполнении задачи
	::PostMessage( m_hWndNotify , nMessageEndOperation, NULL, NULL );

	return true;
}

//////////////////////////////////////////////////////////////////////////

LONG CChmExecDlg::OnMessageEndOperation(UINT wParam, LONG lParam )
{
	UpdateDlgOpeartion( false );

	m_strLogErrs.Insert( 0, CChmExecMediator::GetLogErrs().c_str() );
	UpdateData( FALSE );

	return 0l;
}

//////////////////////////////////////////////////////////////////////////




