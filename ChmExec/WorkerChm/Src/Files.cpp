/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: Модуль реализации основной задачи WorkerChm.
	Слой работы с фалами и директориями

	class TFile - класс работы с файлами и директориями
*/
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "..\Include\Files.h"

using namespace FilesOp;

//////////////////////////////////////////////////////////////////////////
//Возращает расширение m_strName.
cstring TFile::GetExt()
{
	TCHAR ext[_MAX_EXT];

	_tsplitpath_s( m_strName.c_str(), 0, 0, 0, 0, 0, 0, ext, _MAX_EXT ); 

	return ext;
}

//////////////////////////////////////////////////////////////////////////
//Возращает директорию хранения m_strName. 
cstring TFile::GetParentDir()
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];

	_tsplitpath_s( m_strName.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, 0, 0, 0, 0 ); 

	TFile parentDir( cstring( drive ).append( dir ) );
	parentDir.CleanEndBackSlash();

	return parentDir.m_strName;
}

//////////////////////////////////////////////////////////////////////////
//Возращает имя файла m_strName.
cstring TFile::GetShortName()
{
	TCHAR name[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];

	_tsplitpath_s( m_strName.c_str(), 0, 0, 0, 0, name, _MAX_FNAME, ext, _MAX_EXT ); 

	return cstring( name ).append( ext );
}

//////////////////////////////////////////////////////////////////////////
//реализация поиска файлов
void TFile::GetFilesImpl( Files& files,const cstring& path, const cstring& filter, bool bIncludeFiles, bool bIncludeDirs )
{
	files.clear();

	CFileFind finder;

	cstring strFilter = !filter.empty() ? filter : _T("*.*");
	cstring strPath = path;

	BOOL bWorking = finder.FindFile( strPath.append( _T("\\") ).append( strFilter ).c_str() );

	while( bWorking )
	{
		bWorking = finder.FindNextFile();

		if ( 
				finder.IsDots() ||
				!bIncludeDirs && finder.IsDirectory() || 
				!bIncludeFiles && !finder.IsDirectory()  
			) 
			continue;

		files.push_back( cstring( static_cast<PCTSTR>( finder.GetFilePath() ) ) );
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
//Поиск всех файлов и директорий в m_strName
void TFile::GetAll(Files& files )
{
	GetFilesImpl( files, IsFileExsist() ? GetParentDir() : m_strName , cstring(), true, true );

	return;
}

//////////////////////////////////////////////////////////////////////////
//Поиск всех файлов в m_strName с фильтром filter
void TFile::GetFiles(Files& files, const cstring& filter )
{
	GetFilesImpl( files, IsFileExsist() ? GetParentDir() : m_strName, filter , true, false );

	return;
}

//////////////////////////////////////////////////////////////////////////
//Поиск всех директорий в m_strName
void TFile::GetDirs(Files& files  )
{
	GetFilesImpl( files,  IsFileExsist() ? GetParentDir() : m_strName, cstring() , false, true );

	return;
}

//////////////////////////////////////////////////////////////////////////
//Проверка наличия файла
bool TFile::IsFileExsist()
{
	DWORD dwAttrib = INVALID_FILE_ATTRIBUTES;

	return ( dwAttrib = ::GetFileAttributes( m_strName.c_str() ) ) != INVALID_FILE_ATTRIBUTES && 
			!( dwAttrib & FILE_ATTRIBUTE_DIRECTORY );
}

//////////////////////////////////////////////////////////////////////////
//Проверка наличия директории
bool TFile::IsDirExsist()
{
	DWORD dwAttrib = INVALID_FILE_ATTRIBUTES;

	return  ( dwAttrib = ::GetFileAttributes( m_strName.c_str() ) ) != INVALID_FILE_ATTRIBUTES && 
			( dwAttrib & FILE_ATTRIBUTE_DIRECTORY );
}

//////////////////////////////////////////////////////////////////////////
//реализация операций с файлами и дир.
//FO_COPY , FO_DELETE, FO_MOVE, FO_RENAME
bool TFile::ShellOperation(short nOperation, const cstring& strTo )
{
	//Требования SHFileOperation, чтобы буферы строк оканчивались 2 нулями

	short nLenBuffFrom = m_strName.length() + 2;
	short nLenBuffTo = strTo.length() + 2;

	vector< TCHAR > buffFrom( nLenBuffFrom );
	vector< TCHAR > buffTo( nLenBuffTo );

	PTSTR szBuffFrom = &buffFrom[ 0 ];
	PTSTR szBufferTo = &buffTo[ 0 ];

	_tcscpy_s( szBuffFrom, nLenBuffFrom - 1, m_strName.c_str() );
	_tcscpy_s( szBufferTo, nLenBuffTo - 1, strTo.c_str() );

	szBuffFrom[ nLenBuffFrom - 1 ] = 0;
	szBufferTo[ nLenBuffTo - 1 ] = 0;

	SHFILEOPSTRUCTW sh = { 0 };
	ZeroMemory( &sh, sizeof(SHFILEOPSTRUCTW) );
	sh.wFunc = nOperation;
	sh.pFrom = szBuffFrom; 
	sh.pTo = szBufferTo;
	//без подтверждения и строки прогресса
	sh.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI;

	return !SHFileOperation( &sh );
}

//////////////////////////////////////////////////////////////////////////
//Удаление m_strName
bool TFile::Delete()
{
	return ShellOperation( FO_DELETE , cstring() );
}

//////////////////////////////////////////////////////////////////////////
//Копирование m_strName 
bool TFile::Copy( const cstring& strDest )
{
	return ShellOperation( FO_COPY, strDest );
}

//////////////////////////////////////////////////////////////////////////
//Проверка наличия заверш. разделителя дир
bool TFile::IsEndBackSlash()
{
	return !m_strName.empty() &&  *m_strName.rbegin() == TCHAR('\\');
}

//////////////////////////////////////////////////////////////////////////
//Проверка наличия и добавление заверш. разделителя дир
void TFile::CheckEndBackSlash()
{
	if ( !IsEndBackSlash() )
		m_strName += TCHAR('\\');

	return;
}

//////////////////////////////////////////////////////////////////////////
//Проверка наличия и удаление заверш. разделителя дир
void TFile::CleanEndBackSlash()
{
	if ( IsEndBackSlash() )
		m_strName.erase( m_strName.end() - 1 );

	return;
}

//////////////////////////////////////////////////////////////////////////
//Переименование m_strName
bool TFile::Rename( const cstring& strNewName )
{
	return ShellOperation( FO_RENAME, strNewName );
}

//////////////////////////////////////////////////////////////////////////
//Перемещение m_strName
bool TFile::Move( const cstring& strPath )
{
	return ShellOperation( FO_MOVE, strPath );
}

//////////////////////////////////////////////////////////////////////////
//Вызов стандартного диалога выбора директории
//Возращает true -  в случае выбора
//strPath - выбранная дир.
//hParent - окно владелец диалога
//strTitle - заголовок диалога
bool TFile::SelectFolder(cstring& strPath, HWND hParent, const cstring& strTitle )
{
	bool bRet = FALSE;

	TCHAR szSelectedPath[ MAX_PATH ];
	TCHAR szDisplayName[ MAX_PATH ];

	LPITEMIDLIST pidl;

	BROWSEINFO bi = {0};

	bi.hwndOwner = hParent;
	bi.lpszTitle = strTitle.c_str();
	bi.pszDisplayName = szDisplayName;
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | BIF_USENEWUI;

	if ( pidl = ::SHBrowseForFolder( &bi ) )
	{
		if ( SUCCEEDED(::SHGetPathFromIDList( pidl, szSelectedPath ) ) )
		{
			bRet = TRUE;
			strPath = szSelectedPath;
		}

		LPMALLOC pMalloc = NULL;

		if ( SUCCEEDED( SHGetMalloc(&pMalloc) ) )
		{
			pMalloc->Free( pidl );
			pMalloc->Release();
		}
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
