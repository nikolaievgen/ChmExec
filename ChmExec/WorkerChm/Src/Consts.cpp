/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: Модуль реализации основной задачи WorkerChm.

	Константы и сообщения
*/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Consts.h"

PCTSTR WorkerChmUser::szTechResultDirName				= _T("ResultDir");
PCTSTR WorkerChmUser::szTechDirName						= _T("Tech");
PCTSTR WorkerChmUser::szTechChmFile						= _T("TechChm.chm");
PCTSTR WorkerChmUser::szTechTokenTitleBegin				= _T("<TITLE>");;
PCTSTR WorkerChmUser::szTechTokenTitleEnd				= _T("</TITLE>");

PCTSTR WorkerChmUser::szErrMessageInvalidPathOrFileName	= _T("Error operation: Invalid Path or FileName.");
PCTSTR WorkerChmUser::szErrMessageCreateDir				= _T("Error operation: Directory creation.");
PCTSTR WorkerChmUser::szErrMessageCopyChmFile			= _T("Error operation: Copy chm file.");
PCTSTR WorkerChmUser::szErrMessageCallSystem			= _T("Error operation: Call system command.");

