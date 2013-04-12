/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: class CChmExecDlg - класс основного диалога приложения.
	ChmExecDlg.cpp - часть реализации CChmExecDlg, отвечающая за UI.
*/
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ChmExec.h"
#include "ChmExecDlg.h"
#include "WorkerChm/Include/WorkerUserChm.h"

using namespace FilesOp;
using namespace WorkerChmUser;

const short nBuffFileDialog		= 0xFFF;
const short nMaxChmFilesSaved	= 30;

PCTSTR szMessageEndOperation			= _T("MessageEndOperation");
static TCHAR szChmFilter[]				= _T("Файлы справки (*.chm)|*.chm||");
PCTSTR szTitle							= _T("Chmexec");
PCTSTR szTitleOperation					= _T("Chmexec : ***ВЫПОЛНЕНИЕ ОПЕРАЦИИ*** ");
PCTSTR szLogErrFileName					= _T("ChmExec.log");
PCTSTR szSectionParameters				= _T("Parameters");
PCTSTR szSectionParametersKeyThread		= _T("ConfigThread");
PCTSTR szSectionParametersChmFile		= _T("ChmFile");
PCTSTR szMessageOperation				= _T("Приложение занято выполнением операции.");

LONG nMessageEndOperation				= RegisterWindowMessage( szMessageEndOperation );

//////////////////////////////////////////////////////////////////////////

void CChmExecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CHM_FILES, m_listChmFiles);
	DDX_Radio(pDX, IDC_RADIO_THREAD_MAIN, m_nConfig);
	DDX_Text(pDX, IDC_EDIT_LOGERR, m_strLogErrs);
}

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CChmExecDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CHM_ADD, &CChmExecDlg::OnBnClickedButtonChmAdd)
	ON_BN_CLICKED(IDC_BUTTON_CHM_DEL, &CChmExecDlg::OnBnClickedButtonChmDel)
	ON_WM_DELETEITEM()
	ON_WM_DESTROY()
	ON_REGISTERED_MESSAGE( nMessageEndOperation, &CChmExecDlg::OnMessageEndOperation )
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, &CChmExecDlg::OnBnClickedButtonAbout)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

void CChmExecDlg::OnDeleteItem( int nIDCtl, LPDELETEITEMSTRUCT lpDeleteItemStruct )
{
	if ( nIDCtl == IDC_LIST_CHM_FILES )
		delete reinterpret_cast<cstring*>( lpDeleteItemStruct->itemData );

	return;
}

//////////////////////////////////////////////////////////////////////////

BOOL CChmExecDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon( m_hIcon, FALSE );
	SetIcon( m_hIcon, TRUE );
	
	Init();

	UpdateDlgOpeartion( false );
	UpdateDlgIni( false );

	return TRUE;  
}

//////////////////////////////////////////////////////////////////////////

void CChmExecDlg::OnBnClickedButtonChmAdd()
{
	TCHAR buffFileDlg[ nBuffFileDialog ] = _T("");  

	CFileDialog theDlg( TRUE, NULL, NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT,
		szChmFilter, this );

	theDlg.m_ofn.lpstrFile = buffFileDlg;
	theDlg.m_ofn.nMaxFile = nBuffFileDialog;

	if( theDlg.DoModal() != IDOK )
		return;

	POSITION pos = theDlg.GetStartPosition();

	while( pos )
		AddChmFileStr( theDlg.GetNextPathName( pos ) );

	return;
}

//////////////////////////////////////////////////////////////////////////

void CChmExecDlg::UpdateDlgOpeartion(bool bOpenOperation )
{
	m_bOperation = bOpenOperation;

	GetDlgItem( IDOK )->EnableWindow( !m_bOperation );

	SetWindowText( m_bOperation ? szTitleOperation : szTitle );

	return;
}

//////////////////////////////////////////////////////////////////////////

void CChmExecDlg::OnBnClickedButtonChmDel()
{
	int nCurrent = m_listChmFiles.GetCurSel();

	if ( nCurrent == LB_ERR )
		return;

	m_listChmFiles.DeleteString( nCurrent );
	m_listChmFiles.SetCurSel( nCurrent < m_listChmFiles.GetCount() ? nCurrent : 0 );

	return;
}

//////////////////////////////////////////////////////////////////////////

void CChmExecDlg::UpdateDlgNormal()
{
	GetDlgItem( IDC_BUTTON_CHM_DEL )->EnableWindow( m_listChmFiles.GetCurSel() != LB_ERR );
	GetDlgItem( IDOK )->EnableWindow( m_listChmFiles.GetCount() );
	
	return;
}

//////////////////////////////////////////////////////////////////////////

BOOL CChmExecDlg::ContinueModal()
{
	UpdateDlgNormal();

	return CDialog::ContinueModal();
}

//////////////////////////////////////////////////////////////////////////

void CChmExecDlg::AddChmFileStr(PCTSTR szName )
{
	CString strAdd( szName );

	CRect rect;
	m_listChmFiles.GetClientRect( rect );

	CClientDC dc( &m_listChmFiles );
	dc.SelectObject( m_listChmFiles.GetFont() );

	CSize size = dc.GetOutputTextExtent( strAdd );

	if ( size.cx > rect.Width() )
	{
		strAdd.Insert( 0, _T("...") );

		while ( size.cx > rect.Width() )
		{
			strAdd.Delete( 3, 1 );
			size = dc.GetOutputTextExtent( strAdd );
		}
	}

	int nItem = m_listChmFiles.AddString( strAdd );

	if ( nItem >= 0 )
		m_listChmFiles.SetItemData( nItem , (DWORD_PTR) new cstring( szName ) );

	return;
}

//////////////////////////////////////////////////////////////////////////

void CChmExecDlg::FreeItems()
{
	for ( int i = 0 ; i < m_listChmFiles.GetCount(); i++ )
		delete reinterpret_cast<cstring*>( m_listChmFiles.GetItemData( i ) );
}

//////////////////////////////////////////////////////////////////////////

void CChmExecDlg::OnDestroy()
{
	FreeItems();

	CDialog::OnDestroy();

	return;
}

//////////////////////////////////////////////////////////////////////////

void CChmExecDlg::GetFiles(FilesOp::Files& files)
{
	files.clear();

	for( int i = 0 ; i < m_listChmFiles.GetCount(); i++ )
		files.push_back( *reinterpret_cast<cstring*>( m_listChmFiles.GetItemData( i ) ) );

	return;
}

//////////////////////////////////////////////////////////////////////////

class CDlgAbout : public CDialog
{
public:
	//
	CDlgAbout(CWnd* pParent = NULL)
		: CDialog(CDlgAbout::IDD, pParent)
	{}
	//
	enum { IDD = IDD_ABOUTBOX };
};

void CChmExecDlg::OnBnClickedButtonAbout()
{
	CDlgAbout dlg;
	dlg.DoModal();

	return;
}

//////////////////////////////////////////////////////////////////////////

void CChmExecDlg::Init()
{
	TCHAR szBuff[ _MAX_PATH ];
	::GetModuleFileName( AfxGetApp()->m_hInstance, szBuff, _MAX_PATH );

	cstring strCurrDir = TFile( szBuff ).GetParentDir();

	cstring strIniName	= strCurrDir + _T("\\") + AfxGetApp()->m_pszProfileName;
	//файл журнала
	m_strLogErrFileName = strCurrDir + _T("\\") + szLogErrFileName;
	//рез. директория
	m_strResDir			= strCurrDir + _T("\\") + szTechResultDirName;

	//ini файл
	free((void*)AfxGetApp()->m_pszProfileName);
	AfxGetApp()->m_pszProfileName =_tcsdup( strIniName.c_str() );

	if ( !TFile( m_strResDir ).IsDirExsist() )
		::CreateDirectory( m_strResDir.c_str(), NULL );

	return;
}

//////////////////////////////////////////////////////////////////////////

void CChmExecDlg::UpdateDlgIni(bool bSave )
{
	UpdateData( TRUE );

	CString strTechValue;
	CString strTechKey;

	if ( bSave )
	{//сохранение состояния
		//способ запуска задачи
		theApp.WriteProfileInt( szSectionParameters, szSectionParametersKeyThread, m_nConfig );

		//файлы chm
		for ( int i = 0; i < nMaxChmFilesSaved ; i++ )
		{
			strTechKey.Format( _T("%s%d"), szSectionParametersChmFile, i );
			strTechValue.Empty();

			if ( i < m_listChmFiles.GetCount() )
			{
				cstring* pStrChmName = reinterpret_cast<cstring*>( m_listChmFiles.GetItemData( i ));

				strTechValue = pStrChmName ? pStrChmName->c_str() : _T("");
			}

			theApp.WriteProfileStringW( szSectionParameters, strTechKey, strTechValue );
		}

		//журнал
		cfstream fileLog( m_strLogErrFileName, ios_base::out | ios_base::trunc | ios_base::binary );

		fileLog << static_cast<PCTSTR>( m_strLogErrs );
	}
	else
	{//загрузка состояния
		//способ запуска задачи
		m_nConfig = theApp.GetProfileInt( szSectionParameters, szSectionParametersKeyThread, N_CONFIG_THREAD_MAIN );

		//файлы chm
		for ( int i = 0; i < nMaxChmFilesSaved ; i++ )
		{
			strTechKey.Format( _T("%s%d"), szSectionParametersChmFile, i );

			strTechValue = theApp.GetProfileStringW( szSectionParameters, strTechKey, _T("") );

			if ( !strTechValue.IsEmpty() )
				AddChmFileStr( strTechValue );
		}

		//журнал
		cfstream fileLog( m_strLogErrFileName, ios_base::in );

		if ( fileLog.good() )
		{
			cstring strLine;

			while ( getline( fileLog , strLine ).good() )
			{
				m_strLogErrs += strLine.c_str();
				m_strLogErrs += _T("\r\n");
			}
		}
	}

	UpdateData( FALSE );

	return;
}

//////////////////////////////////////////////////////////////////////////

void CChmExecDlg::OnCancel()
{
	if ( m_bOperation )
	{
		AfxMessageBox( szMessageOperation );
		return;
	}

	UpdateDlgIni( true );

	CDialog::OnCancel();
}

//////////////////////////////////////////////////////////////////////////

