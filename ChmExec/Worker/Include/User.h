﻿/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: Модуль исполнения задач Worker. Интерфейс пользователя.

	User.h - Интерфейс задачи
*/
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Defs.h"

//////////////////////////////////////////////////////////////////////////
//Модуль исполнения задач Worker
//////////////////////////////////////////////////////////////////////////
namespace WorkerUser
{
	using namespace std;
		
	//////////////////////////////////////////////////////////////////////////
	//class COperation - интерфейс операции.
	//////////////////////////////////////////////////////////////////////////
	template<typename Item >
	class COperation
	{
	public:
		//
		COperation(){};
		//
		virtual ~COperation(){};

		//выполнение операции над элементом Item
		virtual void Do(Item& item ) = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	//class COperation - интерфейс задачи.
	//////////////////////////////////////////////////////////////////////////
	template<typename Item >
	class CTask 
	{
	public:
		//
		CTask(){};
		//
		virtual ~CTask(){};

		//заполнение входной очереди исполнения workQueue
		virtual void FillQueue( queue<Item*>& workQueue ) = 0;

		//создание операции 1
		virtual COperation<Item>* CreateOperation1() = 0;

		//создание операции 2
		virtual COperation<Item>* CreateOperation2() = 0;
	};
}