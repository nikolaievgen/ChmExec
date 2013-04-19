/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: class CChmExecApp - класс приложения
*/
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ChmExec.h"
#include "ChmExecDlg.h"
#include <tchar.h>
#include <locale.h>
#include <locale>

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CChmExecApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

CChmExecApp::CChmExecApp()
{
}

//////////////////////////////////////////////////////////////////////////

CChmExecApp theApp;

//////////////////////////////////////////////////////////////////////////

BOOL CChmExecApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof( InitCtrls );
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx( &InitCtrls );

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	AfxOleInit();

	//текущие лок.
	SetThreadLocale( LOCALE_USER_DEFAULT );
	_tsetlocale( LC_ALL, _T("") );
	std::locale::global( std::locale( "" ) );

	CChmExecDlg dlg;
	
	m_pMainWnd = &dlg;
	
	//диалог приложения
	dlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

