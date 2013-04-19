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

#include "stdafx.h"
#include "..\Include\WorkerUserChm.h"

using namespace WorkerChmUser;

//////////////////////////////////////////////////////////////////////////
//Получение относительного пути страницы в конт. chm
//Возращает относит путь
cstring COperationParse::CParseHtmlPage::GetRelationName( cstring strHtmlPage )
{
	cstring strRelationName = TFile( strHtmlPage ).GetShortName();

	for ( 
			TFile filePage = TFile( strHtmlPage ).GetParentDir(); 
			filePage.m_strName.length() && TFile( filePage.GetParentDir() ).GetShortName() != szTechDirName;
			filePage.m_strName = filePage.GetParentDir() 
		)
	{
		cstring strDir = filePage.GetShortName();
		strDir += _T("\\");

		strRelationName.insert( 0, strDir.c_str() );
	}

	return strRelationName;
}

//////////////////////////////////////////////////////////////////////////
//выполнение разбора страницы strHtmlPage
void COperationParse::CParseHtmlPage::operator()( cstring strHtmlPage )
{
	if ( !TFile( strHtmlPage ).IsFileExsist() )
		throw new CAppException( szErrMessageInvalidPathOrFileName );

	CHtmlPageInfo info;

	info.m_strHtmlName = GetRelationName( strHtmlPage );

	cfstream htmlPage( strHtmlPage, ios_base::in );

	cstring strLine;

	while ( getline( htmlPage , strLine ).good() )
	{//разобрать строку html файла

		//uppercase
		cstring strUpperLine;
		transform( strLine.begin(), strLine.end(), back_inserter( strUpperLine ), ::toupper );

		//поиск и сохранение тега <Title>
		cstring::size_type nTitle = strUpperLine.find( szTechTokenTitleBegin );

		if ( nTitle == cstring::npos )
			continue;

		nTitle += _tcslen( szTechTokenTitleBegin );

		cstring::size_type nTitleEnd = strUpperLine.find( szTechTokenTitleEnd, nTitle );

		if ( nTitleEnd == cstring::npos )
			continue;

		info.m_strHtmlTitle.assign( strLine, nTitle, nTitleEnd - nTitle );

		break;
	}

	if ( info.m_strHtmlTitle.size() )
		m_item.m_arrHtmlInfo.push_back( info );

	return;
}

//////////////////////////////////////////////////////////////////////////
//выполнение разбора директории strDirectory
void COperationParse::CParseDirectory::operator()( cstring strDirectory )
{
	TFile toParse( strDirectory );

	if ( !toParse.IsDirExsist() )
		throw new CAppException( szErrMessageInvalidPathOrFileName );

	Files files;

	//разобрать страницы
	toParse.GetFiles( files, _T("*.htm*") );
	for_each( files.begin(), files.end(), CParseHtmlPage( m_item ) );

	//разобрать директории
	toParse.GetDirs( files );
	for_each( files.begin(), files.end(), CParseDirectory( m_item ) );
	
	return;
}

//////////////////////////////////////////////////////////////////////////
//Выполнение операции разбора над элементом item
void COperationParse::Do(CItem& item)
{
	TFile res( item.m_strResDir );
	TFile chm( item.m_strNameChm );
	TFile workingDir( item.m_strResDir );

	try
	{
		//проверка и создание рабочей дир.
		if ( !res.IsDirExsist() || !chm.IsFileExsist() )
			throw new CAppException( szErrMessageInvalidPathOrFileName );

		res.CheckEndBackSlash();
		res.m_strName += szTechDirName;

		if ( !res.IsDirExsist() && !::CreateDirectory( res.m_strName.c_str(), NULL ) )
			throw new CAppException( szErrMessageCreateDir );

		res.CheckEndBackSlash();
		res.m_strName += GetNewStrGUID();

		workingDir.m_strName = res.m_strName;

		if ( !::CreateDirectory( workingDir.m_strName.c_str(), NULL ) )
			throw new CAppException( szErrMessageCreateDir );


		if ( !chm.Copy( workingDir.m_strName ))
			throw new CAppException( szErrMessageCopyChmFile );

		//формирование команды открытия chm контейнера
		TCHAR szBuffDecompile[ nSizeBuffDecompile ] = {};

		PCTSTR szFormatCommand = 
			_T("cmd /c ")
				_T("\"")
					_T("cd /d \"%s\" &&")
					_T("ren \"%s\" TECHCHM.CHM &&")
					_T("hh -decompile . TECHCHM.CHM")
				_T("\"")
			;

		_stprintf_s
			( 
				szBuffDecompile, sizeof( szBuffDecompile ) / sizeof( TCHAR ), 
				szFormatCommand, workingDir.m_strName.c_str(), chm.GetShortName().c_str() 
			);

		STARTUPINFO si = { sizeof( si ) };
		PROCESS_INFORMATION pi = {};

		//исполнение команды открытия chm контейнера
		if( ::CreateProcess( NULL, szBuffDecompile , NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi ) )
		{
			WaitForSingleObject( pi.hProcess, INFINITE );
			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
		}
		else
		{
			throw new CAppException( szErrMessageCallSystem );
		}

		//Выполнение разбора
		CParseDirectory parse( item );
		parse( workingDir.m_strName );
	}
	catch (...)
	{
		if ( workingDir.IsDirExsist() )
			workingDir.Delete();

		throw;
	}

	workingDir.Delete();

	return;
}

//////////////////////////////////////////////////////////////////////////
//выполнение сохранения
void COperationSave::CSavePageInfo::operator()( CHtmlPageInfo info )
{
	//формат вывода результатов разбора
	m_file << info.m_strHtmlName << _T(":") << info.m_strHtmlTitle << endl;

	return;
}

//////////////////////////////////////////////////////////////////////////
//Выполнение операции сохранения над элементом item
void COperationSave::Do( CItem& item )
{
	TFile res( item.m_strResDir );
	TFile chm( item.m_strNameChm );

	if ( !res.IsDirExsist() || !chm.IsFileExsist() )
		throw new CAppException( szErrMessageInvalidPathOrFileName );

	if ( !chm.Copy( item.m_strResDir ) )
		throw new CAppException( szErrMessageCopyChmFile );

	cfstream fileSave( item.m_strResDir + _T("\\") + chm.GetShortName() + _T("- Pages Titles.dat"), ios_base::out );

	for_each( item.m_arrHtmlInfo.begin(), item.m_arrHtmlInfo.end(), CSavePageInfo( fileSave ) );

	return;
}

//////////////////////////////////////////////////////////////////////////
//Добавить элемент исполнения strChmName в очередь
void CTaskChm::CNewItem::operator()( cstring strChmName )
{
	CItem* pItem = new CItem;

	pItem->m_strNameChm = strChmName;
	pItem->m_strResDir = m_strResDir;

	m_workQueue.push( pItem );

	return;
}

//////////////////////////////////////////////////////////////////////////
//заполнение входной очереди исполнения workQueue
void CTaskChm::FillQueue( queue<CItem*>& workQueue )
{
	for_each( m_files.begin(), m_files.end(), CNewItem( workQueue, m_strResDir ) );

	return;
}

//////////////////////////////////////////////////////////////////////////
//создание операции 1 - разбора. Возращает операцию
COperation<CItem>* CTaskChm::CreateOperation1()
{
	return new COperationParse;
}

//////////////////////////////////////////////////////////////////////////
//создание операции 2 - сохранения. Возращает операцию
COperation<CItem>* CTaskChm::CreateOperation2()
{
	return new COperationSave;
}

//////////////////////////////////////////////////////////////////////////
//Ведение журнала выполнения
void CWorkerChmFiles::LogStart()
{
	m_strLog += _T("Begin operation at ");
	m_strLog += static_cast<PCTSTR>( CTime::GetCurrentTime().Format( _T("%d-%m-%Y %H:%M:%S") ) );
	m_strLog += _T(".\r\n");

	return;
}

//////////////////////////////////////////////////////////////////////////
//Ведение журнала выполнения
void CWorkerChmFiles::LogEnd()
{
	m_strLog += _T("\r\nEnd operation.\r\n");

	return;
}

//////////////////////////////////////////////////////////////////////////

void CWorkerChmFiles::CleanResDir( const cstring& strResDir )
{
	TFile techDir( strResDir + _T("\\") + szTechDirName );

	if ( techDir.IsDirExsist() )
		techDir.Delete();

	return;
}

//////////////////////////////////////////////////////////////////////////
//Выполнение задачи над файлами chm - files, рез. дир - strResDir
void CWorkerChmFiles::Execute( Files& files, const cstring& strResDir )
{
	LogStart();

	CTimer timer;

	//исполнение задачи
	m_worker( CTaskChm( files , strResDir ) );

	m_strLog += timer.StopGetTime();

	LogEnd();

	CleanResDir( strResDir );

	return;
}

//////////////////////////////////////////////////////////////////////////
