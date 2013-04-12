/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: Модуль реализации основной задачи WorkerChm.

	Константы и сообщения
*/
/////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////
//Модуль реализации основной задачи WorkerChm.
//Константы и сообщения
//////////////////////////////////////////////////////////////////////////
namespace WorkerChmUser
{
	const short nSizeBuffDecompile = 2 * 1024;

	extern PCTSTR szTechResultDirName;
	extern PCTSTR szTechDirName;
	extern PCTSTR szTechChmFile;
	extern PCTSTR szTechTokenTitleBegin;
	extern PCTSTR szTechTokenTitleEnd;
	
	extern PCTSTR szErrMessageInvalidPathOrFileName;
	extern PCTSTR szErrMessageCreateDir;
	extern PCTSTR szErrMessageCopyChmFile;
	extern PCTSTR szErrMessageCallSystem;
}