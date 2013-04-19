/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: Модуль реализации основной задачи WorkerChm.
	Слой работы с фалами и директориями

	class TFile - класс работы с файлами и директориями
*/
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\..\Worker\Include\defs.h"

//////////////////////////////////////////////////////////////////////////
//Модуль реализации основной задачи WorkerChm.
//Слой работы с фалами и директориями
//////////////////////////////////////////////////////////////////////////
namespace FilesOp
{
	using namespace std;

	typedef vector<cstring> Files;

	//////////////////////////////////////////////////////////////////////////
	//class TFile - класс работы с файлами и директориями
	//////////////////////////////////////////////////////////////////////////
	class TFile 
	{
	public:
		//szFileName - имя файла
		TFile( PCTSTR szFileName )
			:m_strName( szFileName )
		{}

		//strFileName - имя файла
		TFile( const cstring& strFileName)
			:m_strName( strFileName )
		{}

		//имя файла
		cstring m_strName;

		//
		operator PCTSTR()
		{ return (PCTSTR) m_strName.c_str(); }

		//Возращает директорию хранения m_strName. 
		cstring GetParentDir();
		//Возращает имя файла m_strName.
		cstring GetShortName();
		//Возращает расширение m_strName.
		cstring GetExt();

		//Проверка наличия заверш. разделителя дир
		bool IsEndBackSlash();
		//Проверка наличия и добавление заверш. разделителя дир
		void CheckEndBackSlash();
		//Проверка наличия и удаление заверш. разделителя дир
		void CleanEndBackSlash();

		//Поиск всех файлов и директорий в m_strName
		void GetAll( Files& allFiles );
		//Поиск всех файлов в m_strName с фильтром filter
		void GetFiles( Files& files, const cstring& filter );
		//Поиск всех директорий в m_strName
		void GetDirs( Files& dirs );

		//Удаление m_strName
		bool Delete();
		//Копирование m_strName 
		bool Copy( const cstring& strDest );
		//Переименование m_strName
		bool Rename( const cstring& strNewName );
		//Перемещение m_strName
		bool Move( const cstring& strPath );

		//Проверка наличия файла
		bool IsFileExsist();
		//Проверка наличия директории
		bool IsDirExsist();

		//Вызов стандартного диалога выбора директории
		//Возращает true -  в случае выбора
		//strPath - выбранная дир.
		//hParent - окно владелец диалога
		//strTitle - заголовок диалога
		static bool SelectFolder( cstring& strPath, HWND hParent, const cstring& strTitle );

	private:
		//реализация поиска файлов
		void GetFilesImpl( Files& files, const cstring& path, const cstring& filter, bool bIncludeFiles, bool bIncludeDirs );
		//реализация операций с файлами и дир.
		//FO_COPY , FO_DELETE, FO_MOVE, FO_RENAME
		bool ShellOperation( short nOperation, const cstring& strTo );
	};
}
