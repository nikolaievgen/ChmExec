/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: class CChmExecDlg - класс основного диалога приложения.
*/
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Resource.h"
#include "Worker/Include/Defs.h"
#include "WorkerChm/Include/Files.h"

//////////////////////////////////////////////////////////////////////////
//class CChmExecDlg - класс основного диалога приложения.
//////////////////////////////////////////////////////////////////////////
class CChmExecDlg : public CDialog
{
public:
	//
	CChmExecDlg(CWnd* pParent = NULL);	

	enum { IDD = IDD_CHMEXEC_DIALOG };
	
private:
	//
	//функциональная часть 
	//
	//буфер радиокнопок выбора способа запуска задачи
	BOOL m_nConfig;
	//флаг выполнения задачи
	bool m_bOperation;
	//результирующая директория
	cstring m_strResDir;

	//выполнение задачи в главном потоке
	void ExecuteThreadMain();
	//запуск на выполнение задачи в рабочем потоке
	//bThreadsOperationsPerProcessors - флаг запуска операций в отдельных рабочих потоках
	void ExecuteThread(bool bThreadsOperationsPerProcessors );

	//////////////////////////////////////////////////////////////////////////
	//class CChmExecMediator - класс посредник запуска модуля исполнения задачи в рабочем потоке.
	//////////////////////////////////////////////////////////////////////////
	class CChmExecMediator
	{
	public:
		//Выполнить задачу 
		static void ExecThread(const cstring& strResDir, FilesOp::Files& files, HWND hWnd, bool bThreadsOperations );
		//Получение журнала выполнения задачи
		static cstring GetLogErrs();

	private:
		//проверка корректности аргументов
		static bool Check();
		//функция запускающего потока 
		static unsigned __stdcall ThreadOperation( void* pArg );

		//рез. директория
		static cstring			m_strResDir;
		//набор входных файлов chm
		static FilesOp::Files	m_files;
		//флаг запуска операций в отдельных рабочих потоках
		static bool				m_bThreadsOperations;
		//окно уведомления UI об окончании выполнения
		static HWND				m_hWndNotify;

		//журнал
		static cstring			m_strLogErrs;

		//объект синхронизации
		static CCriticalSection m_cs;
	};

	//
	//часть UI
	//
	//
	virtual BOOL OnInitDialog();
	//
	virtual void DoDataExchange(CDataExchange* pDX);	
	//
	virtual BOOL ContinueModal();
	//
	virtual void OnCancel();
	//
	virtual void OnOK();

	//заполнение массива files, выбранными файлами chm
	void GetFiles( FilesOp::Files& files);
	//
	void Init();
	//Добавление chm файла в список
	void AddChmFileStr(PCTSTR szName );
	//общее обновление интерфеса
	void UpdateDlgNormal();
	//обновление интерфеса при работе с задачей
	//bOpenOperation - флаг запуска задачи
	void UpdateDlgOpeartion(bool bOpenOperation );
	//обновление интерфеса ini
	//bSave - флаг сохнанения состояния
	void UpdateDlgIni(bool bSave );
	//
	void FreeItems();

	//имя сохраняемого файла журнала
	cstring m_strLogErrFileName;
	//окно списка файлов chm 
	CListBox m_listChmFiles;
	//буффер окна журнала
	CString m_strLogErrs;
	//
	HICON m_hIcon;

	afx_msg void OnBnClickedButtonChmAdd();
	afx_msg void OnBnClickedButtonChmDel();
	afx_msg void OnDeleteItem( int nIDCtl, LPDELETEITEMSTRUCT lpDeleteItemStruct );
	afx_msg void OnDestroy();
	afx_msg LONG OnMessageEndOperation(UINT wParam, LONG lParam);
	afx_msg void OnBnClickedButtonAbout();

	DECLARE_MESSAGE_MAP()
};

//
//Макросы обозначения способа запуска задачи
//

//Запустить в главном потоке
#define N_CONFIG_THREAD_MAIN	0
//Выполнить в рабочем потоке
#define N_CONFIG_THREAD_SINGLE	1
//Запустить в рабочем, а операции выполнять в отдельных рабочих
#define N_CONFIG_THREAD_MULTI	2
