﻿/////////////////////////////////////////////////////////////////////////////
/*
	ChmExec
	Copyright (c) 03.2013 Оргеев Н.Е.
	
	Описание: 
*/
/////////////////////////////////////////////////////////////////////////////

	Приложение-пример решения прикладной задачи обработки и сохранения результатов работы.

Основное содержание задачи является отражением реализованного функционала в упрощенной и универсальной форме. Кроме этого преследовалась цель деления общего функционала задачи на части и отдельные подзадачи, которые можно использовать повторно и независимо от текущего контекста.

	Общее описание.

В общей форме задачу можно описать как выполнение некой абстрактной задачи над множеством элементов. Над каждым элементом выполняется последовательно две абстрактные операции. Данное описание отражает сущность - модуль исполнения. Конкретная реализация абстрактной задачи и использование модуля исполнения вводит сущность - модуль реализации задачи. Данные сущности и окружающие их вспомогательные компоненты используют приложение-драйвер с пользовательским интерфейсом.

Условие задачи:
1.	На вход поступает набор chm файлов-контейнеров. На практике могут быть и другие контейнеры. В данной задаче рассматриваются chm файлы.
2.	Необходимо обработать каждый файл-контейнер. Требуется распаковать контейнер, разобрать страницы для извлечения необходимых сведений. В данной задаче обрабатываются файлы html. Из страницы извлекаются, затем сохраняются название и заголовок страницы.
3.	Далее результаты необходимо сохранить. С практической стороны результаты требуется сохранять в постоянную форму, в базу данных, в соответствии с применяемой моделью данных в приложении. В данном приложении chm файлы сохраняются в директорию ResultDir. Результаты разбора для каждого chm файла сериализуются в dat файлы, также в результирующую директорию.

Пользовательский интерфейс.

Пользовательский интерфейс представляет диалоговое окно, на котором расположены элементы управления:
1.	Список выбранных файлов-контейнеров.
2.	Кнопка «Добавить…». При нажатии, открывается стандартный диалог выбора файлов с фильтром *.chm. Выбранные файлы добавляются в список.
3.	Кнопка «Удалить». При нажатии, выделенные файлы удаляются из списка.
4.	Радио-кнопки выбора способа запуска задачи. 
	o	Задача выполняется в главном потоке.
	o	Задача выполняется в рабочем потоке.
	o	Задача запускается в рабочем потоке, а далее для выполнения каждой операции задачи, запускаются рабочие потоки. Число рабочих потоков на каждую операцию равно числу процессоров на машине.
5.	Окно вывода результатов выполнения задачи. Указывается время запуска и время выполнения задачи.
6.	Кнопка «Выполнить».
7.	Кнопка «Отменить».
8.	Кнопка «О программе».

Особенности приложения:

1.	Приложение сохраняет состояние элементов пользовательского интерфейса в ini и log файлы.
2.	Выполняемая операция не отменяется. 

Функциональное деление задачи:
1.	Модуль исполнения задач Worker. Класс СWorker исполняет задачи, интерфейс которых описывается в классах CTask и COperation. Исходный код расположен в директории Worker (Интерфейс в директории Include, реализация - Src). Модуль находится в пространстве имен Worker. Реализация модуля предполагает повторное использование.
2.	Модуль реализации основной задачи WorkerChm. Класс CChmTask – реализация основной задачи. Описывает основные операции: класс COperationParse – разбора, класс COperationSave – сохранения. Исходный код расположен в директории ChmWorker (Интерфейс в директории Include, реализация - Src). Модуль находится в пространстве имен WorkerChm. Реализация использует модуль исполнения Worker.
3.	Запускающее приложение-драйвер. Класс диалогового окна СDlgChmExec формирует входные данные и передает на исполнение модулю реализации WorkerChm, с помощью посредника - класса CParamsExecute. 

Результаты:

1.	Директория сохранения результатов – ResultDir, находится в директории exe файла приложения. 
2.	Сохраняются chm файлы.
3.	Результаты разбора сохраняются в файлах Имя chm – Page Titles.dat. Формат файла: название страницы html : Заголовок страницы.

Примечание по реализации. 

При разработке делался уклон на универсализацию кода без углубления в практическое усложнение.
