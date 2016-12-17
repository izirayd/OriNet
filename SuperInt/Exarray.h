/* --------------------------------------------------------------
    Динамические массивы с проверкой индекса (версия 1.2c).

    http://www.imach.uran.ru/exarray

    Copyright 1998-2010, Р.Н.Шакиров, ИМаш УрО PAH.
    All Rights Reserved.

    Код предоставляется на условиях лицензии "public domain",
    т.е. допускается свободное применение, модификация и
    распространение кода при условии сохранения копирайта
    и добавления сведений о всех внесенных изменениях.

    ДАННОЕ ПРОГРАММНОЕ ОБЕСПЕЧЕНИЕ ПРЕДОСТАВЛЯЕТСЯ "КАК ЕСТЬ"
    БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ, ЯВНЫХ ИЛИ ПОДРАЗУМЕВАЕМЫХ. ВЫ
    ИСПОЛЬЗУЕТЕ ЕГО НА СВОЙ СОБСТВЕННЫЙ СТРАХ И РИСК. АВТОР
    НЕ БУДЕТ НЕСТИ ОТВЕТСТВЕННОСТЬ ЗА УТРАТУ ДАННЫХ, УЩЕРБ,
    УБЫТКИ ИЛИ ЛЮБЫЕ ДРУГИЕ ВИДЫ ПОТЕРЬ ПРИ ЛЮБЫХ СПОСОБАХ
    ИСПОЛЬЗОВАНИЯ ДАННОГО ПРОГРАММНОГО ОБЕСПЕЧЕНИЯ.
-------------------------------------------------------------- */
#ifndef EXARRAY_H
#define EXARRAY_H

#include <stddef.h>
#include <limits.h>

/* --------------------------------------------------------------
    Шаблоны предназначены для организации массивов с контролем
    индексов и автоматическим увеличением размеров.

    Динамический массив обладает свойством автоматического
    увеличения размера при выходе индекса за границу массива.

    Динамический указатель предназначен для адресации элементов
    динамического массива с автоматическим увеличением размера.

    Регулируемый массив контролирует индекс и блокирует
    обращение к элементам за пределами массива.

    Ограниченный указатель предназначен для адресации массивов
    (обычных, регулируемых и динамических) с контролем индексов
    при условии фиксации их размеров.

    Декларация:
    exarray<тип>  имя;      Динамический массив.
    expoint<тип>  имя;      Динамический указатель.
    exvector<тип> имя;      Регулируемый массив.
    exptr<тип>    имя;      Ограниченный указатель.

    Реализованы штатные операции С над массивами и указателями.
-------------------------------------------------------------- */

#ifdef  __cplusplus
template <class T> class exarray;
template <class T> class expoint;
template <class T> class exvector;
template <class T> class exptr;
#endif/*__cplusplus*/

/* ==============================================================
    Опции препроцессора.

1.  Лингвистические опции по умолчанию выключены, т.к. они
    могут создать проблемы для трансляторов, не соответствующих
    стандарту С++.

    ALLOW_EXARRAY_ADDSUB        - разрешает операции + и -
            над динамическим массивом и целочисленным индексом,
            в результате чего строится динамический указатель.
            (иначе + и - определены только для указателей).
            * Приводит к разбуханию отладочной информации
              в Borland C++ 3.1.

    ALLOW_EXARRAY_SELECTION     - разрешает оператор ->.
    ALLOW_EXARRAY_ITERATORS     - разрешает оператор ->,
            а также методы для применения шаблонов в качестве
            итераторов и контейнеров библиотеки STL.
            * Не поддерживается в Borland C++ 3.1 и 4.5.
            * Реализована часть контейнерных методов STL.

    ALLOW_EXARRAY_EXCEPTIONS    - разрешает явное применение
            средств обработки исключений (требуется, если
            возможен запуск исключений в деструкторах
            элементов массива).
            * Не поддерживается в Borland C++ 3.1.
            * В Visual C++ обработка исключений в деструкторах
              локальных объектов и встроенных массивов
              реализована некорректно (но динамические
              массивы удаляются правильно).

2.  Опции совместимости. Включите эти опции, если Ваш транслятор
    сообщает о проблемах.

    ALLOW_EXARRAY_NEW_DELETE    - переопределить new[] и delete[]
            для макросов EXNEW и EXDELETE.
            * Всегда включена, если транслятор полностью совместим
              со стандартом C++ (__cplusplus >= 199711L), также для
              Visual C++ 5.0 и выше и для Borland C++ 4.5 и выше.

    ALLOW_EXARRAY_PLACEMENT     - поддержка placement delete.
            * Всегда включена, если транслятор полностью совместим
              со стандартом C++ (__cplusplus >= 199711L), также для
              Visual C++ 6.0 и выше.

    ALLOW_EXARRAY_CHARTYPE      - тип char отличен
            от signed/unsigned char.
            * Всегда включена, если транслятор полностью совместим
              со стандартом C++ (__cplusplus >= 199711L), также для
              Visual C++ 6.0 и выше, Borland C++ 4.5 и выше.

    ALLOW_EXARRAY_SIZETYPE      - тип size_t больше всех
            стандартных целочисленных типов, включая long.
            * Всегда включена для компиляторов, определяющих
              SSIZE_MAX или _WIN64 и _I64_MAX больше ULONG_MAX,
              как Visual C++ в 64-битном режиме.

    ALLOW_EXARRAY_PTRDIFFTYPE   - тип ptrdiff_t больше всех
            стандартных целочисленных типов, включая long,
            но не больше size_t.
            * Всегда включена для компиляторов, определяющих
              SSIZE_MAX или _WIN64 и _I64_MAX больше ULONG_MAX,
              как Visual C++ в 64-битном режиме.

    DISALLOW_EXARRAY_LONGTYPE   - запретить доступ к элементам
            массива по индексам типа long и unsigned long.

    DISALLOW_EXARRAY_INTTYPE   - запретить доступ к элементам
            массива по индексам типа int и unsigned int.

    DISALLOW_EXARRAY_SHORTTYPE   - запретить доступ к элементам
            массива по индексам типа short и unsigned short.

    ALLOW_EXARRAY_USING         - использовать директивы using
            для членов родительского класса-шаблона.
            * Всегда включена, если транслятор полностью совместим
              со стандартом C++ (__cplusplus >= 199711L), также для
              Visual C++ 2005 и выше, GNU g++ 3.4 и выше.
            * Не поддерживается в Borland C++ 3.1 и 4.5.

3.  Опции оптимизации.

    ALLOW_EXARRAY_SCHEDULING    - оптимизация кода для
            суперскалярных микропроцессоров Pentium и
            Pentium MMX (не рекомендуется).
            * Контрпродуктивен для Borland C++ 3.1 и 4.5,
              в ряде случаев - для Borland C++ Builder.
            * Эффект для Visual C++ зависит от приложения,
              под Visual C++ 6.0 данная оптимизация может
              привести к ошибке распределения памяти.

    NCHECKPTR                   - отключить отладочные средства.
            * Включен при компиляции в режимах NDEBUG и С.
            * Visual C++ ставит NDEBUG в режиме Release.
============================================================== */
/*
    #define ALLOW_EXARRAY_ADDSUB
    #define ALLOW_EXARRAY_SELECTION
    #define ALLOW_EXARRAY_ITERATORS
    #define ALLOW_EXARRAY_EXCEPTIONS
    #define ALLOW_EXARRAY_NEW_DELETE
    #define ALLOW_EXARRAY_PLACEMENT
    #define ALLOW_EXARRAY_CHARTYPE
    #define ALLOW_EXARRAY_SIZETYPE
    #define ALLOW_EXARRAY_PTRDIFFTYPE
    #define DISALLOW_EXARRAY_LONGTYPE
    #define DISALLOW_EXARRAY_INTTYPE
    #define DISALLOW_EXARRAY_SHORTTYPE
    #define ALLOW_EXARRAY_USING
    #define ALLOW_EXARRAY_SCHEDULING
    #define NCHECKPTR
*/

#ifndef __cplusplus

#define NCHECKPTR

#else /*__cplusplus*/

#ifdef  NDEBUG
#define NCHECKPTR
#endif/*NDEBUG*/

#endif/*__cplusplus*/

/* --------------------------------------------------------------
    Дополнительные включаемые файлы.
-------------------------------------------------------------- */

#ifdef  ALLOW_EXARRAY_ITERATORS
#include <iterator>
#define ALLOW_EXARRAY_SELECTION
#endif/*ALLOW_EXARRAY_ITERATORS*/

/* --------------------------------------------------------------
    Распознавание транслятора.

    Compiler                        _MSC_VER  _TURBOC_
    --------                        --------  --------
    C Compiler     6.0                   600
    C/C++ compiler 7.0                   700
    Visual C++, 32-bit, version 1.0      800
    Visual C++, 32-bit, version 2.0      900
    Visual C++, 32-bit, version 4.0     1000
    Visual C++, 32-bit, version 4.2     1020
    Visual C++, 32-bit, version 5.0     1100
    Visual C++, 32-bit, version 6.0     1200
    Visual C++ .NET,    version 7.0     1300
    Visual C++ 2005,    version 8.0     1400
    Visual C++ 2008,    version 9.0     1500
    Visual C++ 2010,    version 10.0    1600
    Borland C++ 3.1                              0x410
    Borland C++ 4.5                              0x460
    Borland C++ Builder 1.0                      0x520
-------------------------------------------------------------- */
/*
    Compatible C++ ISO/IEC 14882
*/
#ifdef  __cplusplus
#if     __cplusplus >= 199711L
#define ALLOW_EXARRAY_NEW_DELETE
#define ALLOW_EXARRAY_PLACEMENT
#define ALLOW_EXARRAY_CHARTYPE
#define ALLOW_EXARRAY_USING
#endif/*__cplusplus*/
#endif/*__cplusplus*/

/*
    Borland C++
*/
#ifdef  __TURBOC__

#if     __TURBOC__ <= 0x410
#ifdef  ALLOW_EXARRAY_EXCEPTIONS
#undef  ALLOW_EXARRAY_EXCEPTIONS
#endif/*ALLOW_EXARRAY_EXCEPTIONS*/
#endif/*__TURBOC__*/

#if     __TURBOC__ >= 0x460
#define ALLOW_EXARRAY_NEW_DELETE
#define ALLOW_EXARRAY_CHARTYPE
#endif/*__TURBOC__*/

#endif/*__TURBOC__*/

/*
    Visual C++, Intel C/C++ for Windows
*/
#ifdef  _MSC_VER

#if     _MSC_VER >= 1100
#define ALLOW_EXARRAY_NEW_DELETE
#if     _MSC_VER >= 1200
#define ALLOW_EXARRAY_PLACEMENT
#define ALLOW_EXARRAY_CHARTYPE
#if     _MSC_VER >= 1400
#define ALLOW_EXARRAY_USING
#endif/*_MSC_VER*/
#endif/*_MSC_VER*/
#endif/*_MSC_VER*/

#pragma warning(disable: 4514)  // Unreferenced inline function is removed.
#pragma warning(disable: 4710)  // Function not inlined.

#ifdef  ALLOW_EXARRAY_SELECTION
#pragma warning(disable: 4284)  // Will produce errors for infix notation.
#endif/*ALLOW_EXARRAY_SELECTION*/

#endif/*_MSC_VER*/

/*
    GNU gcc, Intel C/C++ for Linux
*/
#ifdef  __GNUC__

#if     __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define ALLOW_EXARRAY_USING
#endif/*__GNUC__*/

#endif/*__GNUC__*/

/* --------------------------------------------------------------
    Макросы для декларации соглашения о связях.
    STDECL  - стандартное соглашение С
    EXDECL  - внешняя функция с фиксированным числом параметров
    FCDECL  - внешняя функция с ускоренным вызовом
    INDECL  - force-inline функция C++
    VDECL   - внешняя переменная
-------------------------------------------------------------- */

#ifdef  __STDC__
#define STDECL
#else /*__STDC__*/
#define STDECL  __cdecl
#ifdef  _MSC_VER
#if     _MSC_VER >= 1200
#define INDECL  __forceinline
#endif/*_MSC_VER*/
#endif/*_MSC_VER*/
#endif/*__STDC__*/

#ifndef EXDECL
#define EXDECL  STDECL
#endif/*EXDECL*/

#ifndef FCDECL
#define FCDECL  STDECL
#endif/*FCDECL*/

#ifndef INDECL
#define INDECL  inline
#endif/*INDECL*/

#define VDECL

/* ==============================================================
    Распределение динамической памяти для C.
============================================================== */

/* --------------------------------------------------------------
    Данные объявления могут быть продублированы
    в других включаемых файлах.
-------------------------------------------------------------- */

#ifndef EXALLOC_H_MAIN
#define EXALLOC_H_MAIN

/* --------------------------------------------------------------
    Внешние ссылки объявлены, как "C", чтобы
    заблокировать преобразование имен.
    Это позволяет переносить объектные модули
    между различными версиями компиляторов.
-------------------------------------------------------------- */

#ifdef  __cplusplus
extern "C" {
#endif/*__cplusplus*/

/* --------------------------------------------------------------
    Структура, аналогичная динамическому массиву
    по набору элементов.
-------------------------------------------------------------- */

struct  exblock_t { void* e; size_t len; };

/* --------------------------------------------------------------
    Структура, аналогичная стеку динамических массивов
    по набору элементов.
-------------------------------------------------------------- */

struct  exblockstack_t { exblock_t stack; size_t n; };

/* --------------------------------------------------------------
    Динамический NULL-массив.
-------------------------------------------------------------- */

extern struct  exblock_t VDECL exnull_c;

/* --------------------------------------------------------------
    Функция set_exalloc_handler() устанавливает
    обработчик ошибок распределения памяти,
    аналогичный set_new_handler().
-------------------------------------------------------------- */

extern  void (* set_exalloc_handler (void (*pvf) (void))) (void);

/* --------------------------------------------------------------
    Обработчики частных ошибок и сведения
    o распределении памяти.

    exalloc_status.size_handler
        Указатель диагностической функции, вызываемой
        при нехватке памяти. Функция должна возбудить
        исключение или прекратить работу программы.

    exalloc_status.grow_handler
        Указатель диагностической функции, вызываемой
        при автоматическом увеличении размера массива
        или NULL, если вызов диагностической функции
        не требуется. Может применяться для блокировки
        автоматического увеличения размера.

    exalloc_status.null_handler
        Указатель функции, вызываемой при обращении
        по NULL-указателю. Функция должна возбудить
        исключение или прекратить работу программы.

    exalloc_status.range_handler
        Указатель функции, вызываемой при выходе
        индекса за границы массива в том случае, если
        не предусмотрено автоматическое увеличение
        размера массива. Функция должна возбудить
        исключение или прекратить работу программы.
-------------------------------------------------------------- */

struct  exalloc_status_t
{
    void (* VDECL size_handler)(void);
    void (* VDECL grow_handler)(void);
    void (* VDECL null_handler)(void);
    void (* VDECL range_handler)(void);
    char*  pMinAlloc;               /* Минимальный адрес */
    char*  pMaxAlloc;               /* Максимальный адрес */
    int    nBlocksAllocated;        /* Число распределенных блоков */
    int    nBlocksFailed;           /* Число ошибок распределения */
};
extern struct exalloc_status_t VDECL exalloc_status;

/* --------------------------------------------------------------
    Функция exmalloc() выделяет блок памяти, заполненный нулями.

    Функция exmrealloc() выделяет, удлиняет, укорачивает или
    освобождает блок памяти, а также обнуляет заданный участок.

    Размер блока задается в байтах. При переполнении разрядной
    сетки или нехватке памяти вызывается обработчик ошибок,
    установленный по exalloc_set_handler, иначе присвоенный
    exalloc_status.size_handler.

    Функция exfree() освобождает блок памяти.

    Функции exaligned_ позволяют указать выравнивание.
-------------------------------------------------------------- */

void*   FCDECL  exmalloc           (size_t blocksize);
void*   FCDECL  exaligned_malloc   (size_t blocksize, size_t blockalign);

void    FCDECL  exmrealloc         (void** p,
                                    size_t blocksize,
                                    size_t memset_start,
                                    size_t memset_stop);
void    FCDECL  exaligned_mrealloc (void** p,
                                    size_t blocksize, size_t blockalign,
                                    size_t memset_start,
                                    size_t memset_stop);

void    FCDECL  exfree             (void* p);
void    FCDECL  exaligned_free     (void* p);

/* --------------------------------------------------------------
    Функция exrange() вызывает
    exalloc_status.null_handler  при p == NULL,
    exalloc_status.range_handler при p != NULL.
-------------------------------------------------------------- */

void    FCDECL  exrange (void* p);

/* --------------------------------------------------------------
    Параметры распределения динамической памяти.

    EXSIZE_T_MAX        - максимальное значение,
            выдаваемое в качестве результата
            целочисленных вычислений с контролем
            переполнения разрядной сетки.
    EXARRAY_ALIGN       - выравнивание блока памяти,
            распределяемого для динамического массива.
            Допустимые значения - 1, 2, 4, 8, 16.
    EXCALCBLOCKSIZE_SYS - размер системного
            заголовка блока памяти, зависящий
            от применяемого компилятора (если
            размер неизвестен, то можно указать
            верхнюю оценку, но не более 16).
    EXCALCBLOCKSIZE_MOD - начальный размер
            блока памяти с учетом системного
            заголовка при расчете по алгоритму
            функции excalcblocksize(), кратный
            pow(64,n)+/-16 байт.
    EXCALCBLOCKSIZE1..5 - начальный размер
            блока памяти без учета системного
            заголовка, рассчитанный по алгоритму
            функции excalcblocksize() и несколько
            последующих размеров, выдаваемых
            той же функцией.
    EXCALCBLOCKSIZE_MAX - максимальный размер
            блока памяти без учета системного
            заголовка, который допускается
            функциями распределения памяти
            (операционная система и аппаратура
            могут накладывать дополнительные
            ограничения).
-------------------------------------------------------------- */

#ifdef  _WIN64
#ifdef  _I64_MAX
#ifndef EXSIZE_T_MAX
#define EXSIZE_T_MAX        (_I64_MAX)
#endif/*EXSIZE_T_MAX*/
#if     _I64_MAX > ULONG_MAX
#define ALLOW_EXARRAY_SIZETYPE
#define ALLOW_EXARRAY_PTRDIFFTYPE
#endif/*_I64_MAX*/
#endif/*_I64_MAX*/
#endif/*_WIN64*/

#ifdef  SSIZE_MAX
#ifndef EXSIZE_T_MAX
#define EXSIZE_T_MAX        (SSIZE_MAX)
#endif/*EXSIZE_T_MAX*/
#if     SSIZE_MAX > ULONG_MAX
#define ALLOW_EXARRAY_SIZETYPE
#define ALLOW_EXARRAY_PTRDIFFTYPE
#endif/*SSIZE_MAX*/
#endif/*SSIZE_MAX*/

#ifndef EXSIZE_T_MAX
#define EXSIZE_T_MAX        ((sizeof(size_t)==2)? UINT_MAX: INT_MAX)
#endif/*EXSIZE_T_MAX*/

#define EXARRAY_ALIGN       ((sizeof(size_t)==2)? 1: 16)
#define EXCALCBLOCKSIZE_SYS (sizeof(size_t) * 2)
#define EXCALCBLOCKSIZE_MOD (sizeof(size_t) * 32 - 16)
#define EXCALCBLOCKSIZE_1   (EXCALCBLOCKSIZE_MOD * 1 - EXCALCBLOCKSIZE_SYS)
#define EXCALCBLOCKSIZE_2   (EXCALCBLOCKSIZE_MOD * 2 - EXCALCBLOCKSIZE_SYS)
#define EXCALCBLOCKSIZE_3   (EXCALCBLOCKSIZE_MOD * 3 - EXCALCBLOCKSIZE_SYS)
#define EXCALCBLOCKSIZE_4   (EXCALCBLOCKSIZE_MOD * 6 - EXCALCBLOCKSIZE_SYS)
#define EXCALCBLOCKSIZE_5   (EXCALCBLOCKSIZE_MOD * 9 - EXCALCBLOCKSIZE_SYS)
#define EXCALCBLOCKSIZE_MAX (EXSIZE_T_MAX - EXCALCBLOCKSIZE_SYS * 2)

/* --------------------------------------------------------------
    Функция excalcblocksize() рассчитывает
    оптимальный размер блока памяти в байтах,
    который должен быть не меньше требуемого.
    Если запрошенный размер слишком велик,
    то выдается EXSIZE_T_MAX.
-------------------------------------------------------------- */

size_t  FCDECL  excalcblocksize (size_t blocksize);

/* --------------------------------------------------------------
    Целочисленные вычисления с контролем
    переполнения разрядной сетки.
      exmuladd (s,n,k)   = s * n + k.
      exmul    (s,n)     = s * n.
      exadd    (n,k)     = n + k.
    При переполнении выдается EXSIZE_T_MAX.
      exchkadd (n,k,max) = n + k, если <= max, иначе ошибка.
    Для генерации оптимального кода следует использовать
    транслятор С++ и подставлять константы на место s и k.
-------------------------------------------------------------- */

#ifndef __cplusplus
#define EXALLOC_OUT_OF_LINE
#endif/*__cplusplus*/

#ifdef  EXALLOC_OUT_OF_LINE

size_t  FCDECL  exmuladd (size_t s, size_t n, size_t k);
size_t  FCDECL  exmul    (size_t s, size_t n);
size_t  FCDECL  exadd    (size_t n, size_t k);
size_t  FCDECL  exchkadd (size_t n, size_t k, size_t max);

#else /*EXALLOC_OUT_OF_LINE*/
}

inline size_t exmuladd (size_t s, size_t n, size_t k)
{
    return ((n <= (EXSIZE_T_MAX - k) / s)? (n * s + k): EXSIZE_T_MAX);
}

inline size_t exmul (size_t s, size_t n)
{
    return ((s == 1)? n: (n <= EXSIZE_T_MAX / s)? n * s: EXSIZE_T_MAX);
}

inline size_t exadd (size_t n, size_t k)
{
    return ((n <= EXSIZE_T_MAX - k)? (n + k): EXSIZE_T_MAX);
}

inline size_t exchkadd (size_t n, size_t k, size_t max)
{
    if (n > max - k) (*exalloc_status.size_handler)();
    return (n + k);
}

extern "C" {
#endif/*EXALLOC_OUT_OF_LINE*/

/* --------------------------------------------------------------
    Дальнейшие внешние ссылки будут обрабатываться
    в режиме по умолчанию.
-------------------------------------------------------------- */

#ifdef  __cplusplus
}
#endif/*__cplusplus*/

/* --------------------------------------------------------------
    Макро EXALLOC_EXTRA_ITEMS(T) задает число дополнительных
    страховочных элементов в массиве базового типа T.
    Макро может принимать значения от 0 и выше.
    Штатное значение - 1.
-------------------------------------------------------------- */

#ifndef EXALLOC_EXTRA_ITEMS
#define EXALLOC_EXTRA_ITEMS(T)  (1)
#endif/*EXALLOC_EXTRA_ITEMS*/

/* --------------------------------------------------------------
    Макро EXALLOC() выделяет заполненный нулями
    блок памяти под массив элементов.

    Макро EXREALLOC() выделяет, удлиняет,
    укорачивает или освобождает блок памяти,
    а также обнуляет заданный участок.

    Размер задается, как число элементов n типа Т.
    Дополнительно к запрошенному числу элементов
    выделяется память для EXALLOC_EXTRA_ITEMS(T)
    обнуленных элементов за последним элементом
    массива. Конструкторы не вызываются.

    При переполнении разрядной сетки или нехватке
    памяти вызывается обработчик ошибок, установленный
    по exalloc_set_handler, иначе присвоенный
    exalloc_status.size_handler.

    Макро EXFREE() освобождает блок памяти
    Деструкторы не вызываются.

    Макро EXALIGNED_ позволяют задать выравнивание.
-------------------------------------------------------------- */

#define EXALLOC(T,n) ((T*) exmalloc (                                   \
                            exmuladd (sizeof(T), (n),                   \
                                EXALLOC_EXTRA_ITEMS(T) * sizeof(T))))

#define EXREALLOC(T,p,n,nstart,nstop) (exmrealloc (                     \
                            (void**)&(p),                               \
                            exmuladd (sizeof(T), (n),                   \
                                EXALLOC_EXTRA_ITEMS(T) * sizeof(T)),    \
                            sizeof(T) * (size_t)(nstart),               \
                            sizeof(T) * (size_t)(nstop)))

#define EXFREE(p)           exfree (p)

#define EXALIGNED_ALLOC(T,n,a) ((T*) exaligned_malloc (                 \
                            exmuladd (sizeof(T), (n),                   \
                                EXALLOC_EXTRA_ITEMS(T) * sizeof(T)),    \
                            (a)))

#define EXALIGNED_REALLOC(T,p,n,nstart,nstop,a) (exaligned_mrealloc (   \
                            (void**)&(p),                               \
                            exmuladd (sizeof(T), (n),                   \
                                EXALLOC_EXTRA_ITEMS(T) * sizeof(T)),    \
                            (a),                                        \
                            sizeof(T) * (size_t)(nstart),               \
                            sizeof(T) * (size_t)(nstop)))

#define EXALIGNED_FREE(p)   exaligned_free (p)

/* --------------------------------------------------------------
    Дальнейшие объявления не будут дублироваться
    в других файлах.
-------------------------------------------------------------- */

#endif/*EXALLOC_H_MAIN*/

/* ==============================================================
    Распределение динамической памяти для C и C++.
============================================================== */

/* --------------------------------------------------------------
    Макро EXNEW() выделяет обнуленный блок памяти
    и вызывает конструкторы С++.

    Размер задается, как число элементов n типа Т.
    Дополнительно к запрошенному числу элементов
    выделяется память для EXALLOC_EXTRA_ITEMS(T)
    элементов за последним элементом массива.

    При переполнении разрядной сетки или
    нехватке памяти вызывается функция,
    установленная по set_exalloc_handler(),
    если же таковой нет, то вызывается abort().

    Макро EXDELETE() вызывает деструкторы C++
    и освобождает блок памяти.

    В режиме C эти макро аналогичны EXALLOC и EXFREE.
-------------------------------------------------------------- */

#ifdef  __cplusplus

template <class T> class exnew_c
{
    T value;
    exnew_c                (const exnew_c<T>&);
    exnew_c<T>& operator = (const exnew_c<T>&);
public:
    exnew_c                () {}
    void* operator new (size_t blocksize)
                        { return exmalloc (blocksize);  }
    void  operator delete   (void *p)   { exfree (p);   }
#ifdef  ALLOW_EXARRAY_NEW_DELETE
    void* operator new[] (size_t blocksize)
                        { return exmalloc (blocksize);  }
    void  operator delete[] (void *p)   { exfree (p);   }
#endif/*ALLOW_EXARRAY_NEW_DELETE*/
};
#define EXNEW(T,n)  ((T*) new exnew_c<T> [::exchkadd(   \
                         (n), EXALLOC_EXTRA_ITEMS(T),   \
                         (EXSIZE_T_MAX / sizeof(T)))])
#define EXDELETE(p) (::delete[] (p))

#else /*__cplusplus*/

#define EXNEW(T,n)  EXALLOC (T,n)
#define EXDELETE(p) EXFREE (p)

#endif/*__cplusplus*/

/* ==============================================================
    Шаблоны массивов и указателей для для С++.

    Динамический массив занимает непрерывную область памяти,
    состоящую из трех участков:
    1) Элементы массива. Для каждого элемента проводится
       обнуление занимаемого участка  памяти, после чего
       вызывается конструктор. К элементам допускается
       обращение по чтению и записи.
    2) Обнуленный участок, достаточный для размещения
       нескольких элементов базового типа (минимальное
       число элементов задается в EXALLOC_EXTRA_ITEMS).
       Если обнуленный участок содержит допустимые
       значения базового типа, то его можно читать при
       обращении к массиву без контроля индекса.
    3) Зарезервированная память, размер которой
       определяется автоматически.

    Наличие обнуленного участка существенно при обращении
    к массиву без контроля индекса, т.к. позволяет сделать
    предсказуемыми последствия распространенной ошибки -
    - обращение по индексу, равному числу элементов.

    Для целей экономии памяти "пустой" динамический массив,
    т.е. массив без элементов, может разделять обнуленный
    участок памяти с другими "пустыми" массивами того же типа.
============================================================== */

#ifdef  __cplusplus

/* --------------------------------------------------------------
    Вспомогательный шаблон для вызова конструкторов и
    деструкторов без распределения памяти.
-------------------------------------------------------------- */

template <class T> class exreloc_c
{
    T value;
    exreloc_c                (const exreloc_c<T>&);
    exreloc_c<T>& operator = (const exreloc_c<T>&);
public:
    exreloc_c                () {}
/*
    Placement-операторы new и delete для вызова конструкторов
    и деструкторов без распределения памяти.
*/
    void* operator new    (size_t, void* p)
                                    { return p;    }
#ifdef  ALLOW_EXARRAY_PLACEMENT
    void  operator delete (void*, void*)  { ; }
#endif/*ALLOW_EXARRAY_PLACEMENT*/
    void  operator delete (void*)         { ; }
};

/* --------------------------------------------------------------
    Шаблон массива с переменными размерами.

    Шаблон применяется для порождения производных классов.
    Возможность непосредственного создания объектов exblock
    не предусмотрена.
-------------------------------------------------------------- */

template <class T> class exblock
{
protected:
    T*          e;      /* Базовый массив */
    size_t      len;    /* Число элементов */
/*
    Статический обнуленный массив размером
    sizeof(T) * EXALLOC_EXTRA_ITEMS(T) байт.
*/
    static T* stub();
/*
    Распределение памяти для заданного числа
    элементов с указанием размера блока памяти.
*/
    void  reallocate (size_t n, size_t blocksize);
/*
    Прерывание работы программы, либо выдача
    указателя на статическую область памяти
    при обращении по индексу i.
*/
        T*  range ()            const;
        T*  range (size_t i)            { return range()-i; }
const   T*  range (size_t i)    const   { return range()-i; }
/*
    Распределение памяти для элемента с
    заданным индексом i >= len. Выдает e.
*/
        T*  grow  (size_t i)    const;
/*
    Конструктор и деструктор защищенные, т.к.
    возможность непосредственного создания
    объектов exblock не предусмотрена.
    В конструкторе производного класса следует задать
    e = stub() и len = 0, после чего, при необходимости,
    вызвать подходящий метод распределения памяти.
    В деструкторе следует вызвать метод clear().
*/
    exblock ()  { /* e = stub(); len = 0; */    }
    ~exblock()  { /* clear(); */                }

private:
/*
    Для блокировки присваивания массивов
    и их передачи в функции по значению
    объявляются приватные конструктор
    копирования и операция присваивания.
    Массивы exblock можно передавать в функции
    по ссылке, либо через ограниченный указатель.
*/
    exblock                (const exblock<T>&);
    exblock<T>& operator = (const exblock<T>&);

public:
/*
    Расчет минимального размера блока памяти
    по требуемому числу элементов в массиве.
    При переполнении выдает EXSIZE_T_MAX.
*/
    size_t min_blocksize (size_t n) const
    {
        return (exmuladd (
            sizeof(T), n,
            sizeof(T) * EXALLOC_EXTRA_ITEMS(T)));
    }
/*
    Распределение памяти точно под указанное
    число элементов, без резервирования.
*/
    void  reallocate (size_t n)
    {
        reallocate (n, min_blocksize (n));
    }
/*
    Расчет максимально возможного числа
    элементов в массиве по размеру блока
    памяти, не меньшему min_blocksize (0).
*/
    size_t max_size (size_t blocksize) const
    {
        return (blocksize / sizeof(T) -
                EXALLOC_EXTRA_ITEMS(T));
    }
/*
    STL-методы управления размером:
      size     () Число элементов.
      capacity () Число элементов, для которых распределена память.
      max_size () Максимально возможное число элементов.
      empty    () Предикат size() == 0.
      resize  (n) Установка числа элементов.
*/
    size_t  size     () const   { return len;                           }
    size_t  capacity () const;
    size_t  max_size () const   { return max_size (EXCALCBLOCKSIZE_MAX -
                                                   EXARRAY_ALIGN);      }
    int     empty    () const   { return (size() == 0);                 }
    void    resize   (size_t n);
/*
    Установка максимального индекса элемента.
*/
    void    confine  (size_t i);
/*
    Эвристические методы управления размером,
    оптимизированные по производительности.

      adjust (i)  Изменить размер массива с тем, чтобы выделить
                  память для элемента с указанным индексом или
                  удалить элементы с вдвое большим индексом.

      expand (i)  Увеличить размер массива с тем, чтобы выделить
                  память для элемента с указанным индексом.

      shrink (i)  Уменьшить размер массива с тем, чтобы удалить
                  элементы с индексом вдвое больше указанного.

    Методы expand() и shrink() являются упрощенными вариантами
    универсального метода adjust(). Методы expand() и adjust()
    гарантируют наличие в массиве элемента с указанным индексом.
    Метод shrink() сохраняет элемент с указанным индексом, если
    память для него уже выделена, иначе этот элемент останется не
    размещенным. Все методы выполняют фактическое распределение
    памяти только в том случае, если оно требуется. Итоговое число
    элементов определяется объемом распределенной памяти и может
    быть больше запрошенного.
*/
protected:
    void  adjust_c (size_t i);               /* Неоптимизированный adjust */
    void  adjust_c (size_t i, const T** p);  /* Неоптимизированный adjust */
    void  shrink_c (size_t i);               /* Неоптимизированный shrink */
public:
    void  adjust   (size_t i)
                { size_t l2 = len/2; if (l2 <= i - l2) adjust_c (i);    }
    void  adjust   (size_t i, const T** p)
                { size_t l2 = len/2; if (l2 <= i - l2) adjust_c (i,p);  }
    void  expand   (size_t i)               const
                { if (len <= i) ((exblock<T>*)this)->adjust_c (i);      }
    void  expand   (size_t i, const T** p)  const
                { if (len <= i) ((exblock<T>*)this)->adjust_c (i,p);    }
    void  shrink   (size_t i)
                { if (i < len/2) shrink_c (i);  }
/*
    Отладочные методы управления размером:

      checkadjust (i) Аналог adjust()
      checkexpand (i) Аналог expand()
      checkshrink (i) Аналог shrink()
      checkindex  (i) Проверка i < len

    При наличии макро NCHECKPTR первые три метода идентичны
    своим аналогам, а метод checkindex() не делает ничего.

    При отсутствии макро NCHECKPTR отладочные методы
    максимальный индекс массива путем вызова confine(),
    Если при обращении к методу checkindex() новое число
    элементов больше текущего, то вызывается функция,
    присвоенная ::exalloc_status.range_handler.
*/
#ifdef  NCHECKPTR
    void  checkadjust (size_t i) { adjust (i);  }
    void  checkexpand (size_t i) { expand (i);  }
    void  checkshrink (size_t i) { shrink (i);  }
    void  checkindex  (size_t i) { (void) (i);  }
#else /*NCHECKPTR*/
    void  checkadjust (size_t i) { confine (i); }
    void  checkexpand (size_t i) { confine (i); }
    void  checkshrink (size_t i) { confine (i); }
    void  checkindex  (size_t i) { if (len <= i) range (i);
                                   confine (i); }
#endif/*NCHECKPTR*/
/*
    Доступ к array без проверки index:
      item (i)      Элемент.
      base ()       Указатель базовoго array.
    Ссылка на элемент и указатели действительны
    до тех пор, пока размер array не изменится.
*/
      T&  item   (size_t i)             { return e[i];          }
const T&  item   (size_t i)     const   { return e[i];          }
      T*  base   ()                     { return (e);           }
const T*  base   ()             const   { return (e);           }
/*
    Доступ к массиву с проверкой индекса:
      at   (i)      Элемент.
    При выходе индекса за границы массива вызывается
    функция, присвоенная ::exalloc_status.range_handler.
*/
#ifdef  ALLOW_EXARRAY_SCHEDULING

INDECL       T&  at     (size_t i)      {       register T* p = e;
                if (len <= (size_t)i) p = range(i); return p[i]; }
INDECL const T&  at     (size_t i) const{ const register T* p = e;
                if (len <= (size_t)i) p = range(i); return p[i]; }

#else /*ALLOW_EXARRAY_SCHEDULING*/

INDECL       T&  at     (size_t i)
                { if (len <= (size_t)i) range (i); return e[i]; }
INDECL const T&  at     (size_t i) const
                { if (len <= (size_t)i) range (i); return e[i]; }

#endif/*ALLOW_EXARRAY_SCHEDULING*/
/*
    Доступ к массиву с проверкой индекса
    и автоматическим увеличением размера.
      access (i)    Элемент.
    При выходе индекса за границы массива
    размер массива автоматически увеличивается.
    При увеличении размера проводится обнуление
    памяти, после чего для всех новых элементов
    выполняются конструкторы по умолчанию.

    Если увеличение размера невозможно из-за
    переполнения разрядной сетки или нехватки
    памяти, то выполняется обработка ошибки с
    прерыванием выполнения программы, как в
    функции exmrealloc().
    ЗАМЕЧАНИЕ:
        отрицательный индекс интерпретируется, как
        большое положительное число, что для всех
        базовых типов T, кроме char в 16-битном
        режиме приводит к обработке ошибки.

    При успешном распределении памяти выдается
    ссылка на элемент массива для извлечения или
    присваивания значения. Ссылка действительна
    до тех пор, пока размер массива не изменится.
*/
#ifdef  ALLOW_EXARRAY_SCHEDULING

INDECL       T&  access (size_t i)      {       register T* p = e;
                if (len <= (size_t)i) p = grow (i); return p[i]; }
INDECL const T&  access (size_t i) const{ const register T* p = e;
                if (len <= (size_t)i) p = grow (i); return p[i]; }

#else /*ALLOW_EXARRAY_SCHEDULING*/

INDECL       T&  access (size_t i)
                { if (len <= (size_t)i) grow (i); return e[i]; }
INDECL const T&  access (size_t i) const
                { if (len <= (size_t)i) grow (i); return e[i]; }

#endif/*ALLOW_EXARRAY_SCHEDULING*/

#ifdef  ALLOW_EXARRAY_ITERATORS
/*
    Определения для библиотеки STL. Массив может выступать
    как контейнер с ограниченным набором методов.
*/
typedef       T         value_type;     /* Тип элемента */
typedef    size_t       size_type;      /* size() */
typedef ptrdiff_t       difference_type;/* operator - */
#ifdef  NCHECKPTR
typedef       T*        iterator;       /* begin(), end() */
typedef const T*        const_iterator; /* begin(), end() */
#else /*NCHECKPTR*/
typedef       exptr<T>  iterator;       /* begin(), end() */
typedef const exptr<T>  const_iterator; /* begin(), end() */
#endif/*NCHECKPTR*/
typedef       T*        pointer;        /* operator -> */
typedef const T*        const_pointer;  /* operator -> */
typedef       T&        reference;      /* operators *, [] */
typedef const T&        const_reference;/* operators *, [] */
/*
    Итераторы для библиотеки STL.
      begin ()    Указывает на первый элемент.
      end   ()    Указывает на элемент, следующий
                  за последним.
    Если элементов нет, то begin() == end().
    Обращаться к элементу end() нельзя.
    В роли итератора выступает ограниченный указатель,
    обеспечивающий контроль индексов, а если определено
    макро NCHECKPTR - то обычный указатель. Итераторы
    действительны до тех пор, пока размер массива не
    изменится.
*/
#ifdef  NCHECKPTR

iterator        begin ()        { return base();                }
const_iterator  begin ()  const { return base();                }

iterator        end   ()        { return base()+size();         }
const_iterator  end   ()  const { return base()+size();         }

#else /*NCHECKPTR*/

iterator        begin ()        { return exptr<T>(e,len);       }
const_iterator  begin ()  const { return exptr<T>(e,len);       }

iterator        end   ()        { return exptr<T>(e,len,len);   }
const_iterator  end   ()  const { return exptr<T>(e,len,len);   }

#endif/*NCHECKPTR*/

#endif/*ALLOW_EXARRAY_ITERATORS*/
/*
    STL-операции над массивом:
      clear()   Очистка массива.
      swap(m)   Перестановка массивов без копирования элементов.
*/
    void    clear     ()        { reallocate (0, 0);    }
    void    swap      (exblock<T>&);
/*
    Служебные методы:
      setbase_c (p) Присваивание базового массива.
      setsize_c (n) Присваивание числа элементов.
    Допускается:
    1) Присваивание нулевого указателя при условии
       установки числа элементов в 0.
    2) Присваивание базового массива, память для который
       выделялась функциями exaligned_malloc или
       exaligned_mrealloc с выравниванием EXARRAY_ALIGN,
       при этом размер блока памяти в байтах blocksize и
       число элементов n должны отвечать условию
       blocksize >= min_blocksize (n).
    Присваивать массив, полученный оператором new не следует,
    т.к. это не разрешено стандартом С++ (для типов с
    нетривиальными деструкторами new использует несовместимый
    формат блока памяти).
*/
    void    setbase_c (T* p)        { e   = p;  }
    void    setsize_c (size_t n)    { len = n;  }
};
/*
    Защищенный метод stub() выдает указатель
    на статический обнуленный массив размером
    sizeof(T) * EXALLOC_EXTRA_ITEMS(T) байт.
*/
template <class T> T* exblock<T>::stub()
{
    static char _stub [sizeof(T) *
            (EXALLOC_EXTRA_ITEMS(T)? EXALLOC_EXTRA_ITEMS(T): 1)];
    return (T*)(_stub);
}
/*
    Защищенный метод reallocate (n, blocksize)
    устанавливает размер блока памяти в байтах
    blocksize и число элементов n. Размер
    блока и число элементов должны отвечать
    условию blocksize >= min_blocksize (n),
    за исключением случая удаления массива,
    когда требуется blocksize == n == 0.

    При увеличении числа элементов выполняется
    обнуление выделенной памяти и вызываются
    конструкторы элементов. При уменьшении
    числа элементов вызываются деструкторы.
    Вызов деструкторов реализован в двух вариантах:
    первый предполагает возможность запуска
    исключений в деструкторе, второй - нет.
*/
template <class T> void exblock<T>::reallocate (size_t n, size_t blocksize)
{
/*
    if (blocksize < min_blocksize (n)) abort();
*/
    T* p = e;               /* Рабочая копия e */

    if (p == 0)             /* Диагностика обращения по */
    {                       /* динамическому 0-указателю */
        if (this == (exblock<T>*)&exnull_c)
            ::exrange (0);
    }
    else if (len == 0)
    {                       /* Если массив пустой, */
        p = 0;              /* то p следует обнулить */
    }
    else
    {
#ifdef ALLOW_EXARRAY_EXCEPTIONS
        for (;;)
        {
        try
        {
#endif
                            /* Вызов деструкторов при */
        while (len > n)     /* удалении или укорочении */
            { --len; delete (exreloc_c<T> *) (p + len); }
#ifdef ALLOW_EXARRAY_EXCEPTIONS
        break;
        }
        catch (...) { };
        }
#endif
    }
    ::exaligned_mrealloc (  /* Распределение памяти */
          (void**)&p,       /* Указатель блока памяти */
          blocksize,        /* Новый размер */
          EXARRAY_ALIGN,    /* Выравнивание */
          sizeof(T) * len,  /* Границы обнуления */
          sizeof(T) * (n + EXALLOC_EXTRA_ITEMS(T))
    );
    if ((e = p) == 0)       /* Запоминание нового e */
    {                       /* Для пустого массива e */
        e = stub();         /* задается как stub */
    }
    else
    {                       /* Вызов конструкторов при */
        while (len < n)     /* создании или удлинении */
            { new (p + len) exreloc_c<T>; ++len; }
    }
}
/*
    Защищенный метод range() вызывается для прерывания
    работы программы или выдачи "безопасного" указателя
    на статическую область памяти.
*/
template <class T> T* exblock<T>::range() const
{
    exrange (e);
    return stub();
}
/*
    Защищенный метод grow (i) вызывается
    при обращении к элементу с i >= len
    для автоматического увеличения размера.
    Перед распределением памяти вызывается функция,
    присвоенная ::exalloc_status.grow_handler.
*/
template <class T> T* exblock<T>::grow (size_t i) const
{
    (*::exalloc_status.grow_handler)();
    size_t blocksize = ::excalcblocksize (
        exmuladd (sizeof(T), i,
                  sizeof(T) * (1 + EXALLOC_EXTRA_ITEMS(T))));
    ((exblock<T>*)this)->reallocate (max_size (blocksize), blocksize);
    return (e);
}
/*
    Метод capacity() выдает число элементов,
    для которых распределена память.
    Это число всегда не меньше size().
*/
template <class T> size_t exblock<T>::capacity() const
{
    return (max_size (::excalcblocksize (min_blocksize (len))));
}
/*
    Метод resize (n) устанавливает заданное число
    элементов и распределяет память для числа
    элементов, не меньшего заданного.
*/
template <class T> void exblock<T>::resize  (size_t n)
{
    reallocate (n, ::excalcblocksize (min_blocksize (n)));
}
/*
    Метод confine (i) устанавливает максимальный индекс
    элемента, равный заданному и распределяет память для
    числа элементов, не меньшего заданного.
*/
template <class T> void exblock<T>::confine (size_t i)
{
    reallocate (i + 1, ::excalcblocksize (
        exmuladd (sizeof(T), i,
                  sizeof(T) * (1 + EXALLOC_EXTRA_ITEMS(T)))));
}
/*
    Метод adjust_c (i) устанавливает максимальный индекс
    элемента, не меньший заданного.
*/
template <class T> void exblock<T>::adjust_c (size_t i)
{
    size_t blocksize = ::excalcblocksize (
        exmuladd (sizeof(T), i,
                  sizeof(T) * (1 + EXALLOC_EXTRA_ITEMS(T))));
    reallocate (max_size (blocksize), blocksize);
}
/*
    Модификация метода adjust_c (i), позволяющая
    скорректировать указатель на элемент массива
    при перемещении массива.
    Указатель корректируется только в том случае,
    если он относится к элементу массива.
*/
template <class T> void exblock<T>::adjust_c (size_t i, const T** p)
{
    if (e <= *p && *p < e + size())
    {
       size_t k = (size_t) ((*p) - e); adjust_c (i); *p = e + k;
    }
    else adjust_c (i);
}
/*
    Метод shrink_c (i) уменьшает максимальный индекс элемента
    до значения не меньше заданного.
*/
template <class T> void exblock<T>::shrink_c (size_t i)
{
    size_t blocksize = ::excalcblocksize (
        exmuladd (sizeof(T), i,
                  sizeof(T) * (1 + EXALLOC_EXTRA_ITEMS(T))));
    size_t n = max_size (blocksize);
    if (len > n) reallocate (n, blocksize);
}
/*
    Перестановка массивов без копирования элементов.
*/
template <class T> void  exblock<T>::swap (exblock<T>& m)
{
    T*     temp_base = base();
    size_t temp_size = size();
    setbase_c   (m.base());
    setsize_c   (m.size());
    m.setbase_c (temp_base);
    m.setsize_c (temp_size);
}

/* --------------------------------------------------------------
    Шаблон массива с переменными размерами, к которому можно
    обращаться через динамический указатель с возможностью
    автоматического увеличения размера.

    Шаблон применяется для порождения производных классов.
    Возможность непосредственного создания объектов explace
    не предусмотрена.
-------------------------------------------------------------- */

template <class T> class explace: public exblock<T>
{
protected:
/*
    Конструктор и деструктор защищенные, т.к.
    возможность непосредственного создания
    объектов explace не предусмотрена.
    В конструкторе производного класса следует задать
    e = stub() и len = 0, после чего, при необходимости,
    вызвать подходящий метод распределения памяти.
    В деструкторе следует вызвать метод clear().
*/
    explace ()  { /* e = stub(); len = 0; */    }
    ~explace()  { /* clear(); */                }

private:
/*
    Для блокировки присваивания массивов
    и их передачи в функции по значению
    объявляются приватные конструктор
    копирования и операция присваивания.
    Массивы explace можно передавать в функции
    по ссылке, либо через динамический
    или ограниченный указатель.
*/
    explace                (const explace<T>&);
    explace<T>& operator = (const explace<T>&);
};

/* --------------------------------------------------------------
    Шаблон регулируемого массива с контролем индексов.
-------------------------------------------------------------- */

template <class T> class exvector: public exblock<T>
{
#ifdef  ALLOW_EXARRAY_USING
protected:
    using exblock<T>::e;
    using exblock<T>::len;
    using exblock<T>::stub;
public:
    using exblock<T>::clear;
    using exblock<T>::swap;
    using exblock<T>::base;
    using exblock<T>::at;
private:
#endif/*ALLOW_EXARRAY_USING*/
/*
    Для блокировки присваивания регулируемых
    массивов и их передачи в функции по
    значению объявляются приватные конструктор
    копирования и операция присваивания.
    Регулируемые массивы можно передавать в
    функции по ссылке, через ограниченный
    указатель, а также через указатель const T*.
*/
    exvector                (const exvector<T>&);
    exvector<T>& operator = (const exvector<T>&);

public:
/*
    Конструктор регулируемого массива.
    Поскольку e должно быть отлично от 0,
    то e присваивается указатель
    на статический массив типа T.
*/
    exvector () { e = stub(); len = 0; }
/*
    Конструктор регулируемого массива
    с заданным начальным числом элементов.
*/
/*explicit*/ exvector (size_t n);
/*
    Деструктор выполняет освобождение памяти.
*/
    ~exvector() { clear(); }
/*
    Автоматическое преобразование в const T*
    применяется при передаче базового массива
    в функцию через параметр const T* и при
    логических операциях. Логическое значение
    регулируемого массива - истина.
*/
    operator const T*()   const { return base();        }
/*
    Доступ к массиву с проверкой индекса.
*/
#ifdef  ALLOW_EXARRAY_SIZETYPE
INDECL       T& operator [] (size_t i)               { return at (i); }
INDECL const T& operator [] (size_t i)         const { return at (i); }
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
INDECL       T& operator [] (ptrdiff_t i)            { return at ((size_t)i); }
INDECL const T& operator [] (ptrdiff_t i)      const { return at ((size_t)i); }
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
INDECL       T& operator [] (unsigned long i)        { return at ((size_t)i); }
INDECL const T& operator [] (unsigned long i)  const { return at ((size_t)i); }
INDECL       T& operator [] (long i)                 { return at ((size_t)i); }
INDECL const T& operator [] (long i)           const { return at ((size_t)i); }
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
INDECL       T& operator [] (unsigned int i)         { return at (i); }
INDECL const T& operator [] (unsigned int i)   const { return at (i); }
INDECL       T& operator [] (int i)                  { return at (i); }
INDECL const T& operator [] (int i)            const { return at (i); }
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
INDECL       T& operator [] (unsigned short i)       { return at (i); }
INDECL const T& operator [] (unsigned short i) const { return at (i); }
INDECL       T& operator [] (short i)                { return at (i); }
INDECL const T& operator [] (short i)          const { return at (i); }
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/
INDECL       T& operator *  ()                       { return at (0); }
INDECL const T& operator *  ()                 const { return at (0); }
#ifdef  ALLOW_EXARRAY_SELECTION
INDECL       T* operator -> ()                { return (T*)&(at (0)); }
INDECL const T* operator -> ()        const   { return (T*)&(at (0)); }
#endif/*ALLOW_EXARRAY_SELECTION*/
/*
    Арифметические операции.
*/
#ifdef  ALLOW_EXARRAY_ADDSUB

private:
exptr<T> _add (size_t i) const  { return (exptr<T>(*(exblock<T>*)this, i));}
exptr<T> _sub (size_t i) const  { return (exptr<T>(*(exblock<T>*)this,-i));}
public:

#ifdef  ALLOW_EXARRAY_SIZETYPE
      exptr<T>  operator +  (size_t i)                { return (_add (i)); }
const exptr<T>  operator +  (size_t i)         const  { return (_add (i)); }
      exptr<T>  operator -  (size_t i)                { return (_sub (i)); }
const exptr<T>  operator -  (size_t i)         const  { return (_sub (i)); }
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
      exptr<T>  operator +  (ptrdiff_t i)             { return (_add ((size_t)i)); }
const exptr<T>  operator +  (ptrdiff_t i)      const  { return (_add ((size_t)i)); }
      exptr<T>  operator -  (ptrdiff_t i)             { return (_sub ((size_t)i)); }
const exptr<T>  operator -  (ptrdiff_t i)      const  { return (_sub ((size_t)i)); }
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
      exptr<T>  operator +  (unsigned long i)         { return (_add ((size_t)i)); }
const exptr<T>  operator +  (unsigned long i)  const  { return (_add ((size_t)i)); }
      exptr<T>  operator +  (long i)                  { return (_add ((size_t)i)); }
const exptr<T>  operator +  (long i)           const  { return (_add ((size_t)i)); }
      exptr<T>  operator -  (unsigned long i)         { return (_sub ((size_t)i)); }
const exptr<T>  operator -  (unsigned long i)  const  { return (_sub ((size_t)i)); }
      exptr<T>  operator -  (long i)                  { return (_sub ((size_t)i)); }
const exptr<T>  operator -  (long i)           const  { return (_sub ((size_t)i)); }
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
      exptr<T>  operator +  (unsigned int i)          { return (_add (i)); }
const exptr<T>  operator +  (unsigned int i)   const  { return (_add (i)); }
      exptr<T>  operator +  (int i)                   { return (_add (i)); }
const exptr<T>  operator +  (int i)            const  { return (_add (i)); }
      exptr<T>  operator -  (unsigned int i)          { return (_sub (i)); }
const exptr<T>  operator -  (unsigned int i)   const  { return (_sub (i)); }
      exptr<T>  operator -  (int i)                   { return (_sub (i)); }
const exptr<T>  operator -  (int i)            const  { return (_sub (i)); }
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
      exptr<T>  operator +  (unsigned short i)        { return (_add (i)); }
const exptr<T>  operator +  (unsigned short i) const  { return (_add (i)); }
      exptr<T>  operator +  (short i)                 { return (_add (i)); }
const exptr<T>  operator +  (short i)          const  { return (_add (i)); }
      exptr<T>  operator -  (unsigned short i)        { return (_sub (i)); }
const exptr<T>  operator -  (unsigned short i) const  { return (_sub (i)); }
      exptr<T>  operator -  (short i)                 { return (_sub (i)); }
const exptr<T>  operator -  (short i)          const  { return (_sub (i)); }
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

#else /*ALLOW_EXARRAY_ADDSUB*/
/*
    Блокировка арифметических операций.
    Если это не сделать, то будет проводиться
    преобразование в const T* и операции над
    указателями.
*/
private:

#ifdef  ALLOW_EXARRAY_SIZETYPE
void    operator +  (size_t i)                  ;
void    operator +  (size_t i)          const   ;
void    operator -  (size_t i)                  ;
void    operator -  (size_t i)          const   ;
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
void    operator +  (ptrdiff_t i)               ;
void    operator +  (ptrdiff_t i)       const   ;
void    operator -  (ptrdiff_t i)               ;
void    operator -  (ptrdiff_t i)       const   ;
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
void    operator +  (unsigned long i)           ;
void    operator +  (unsigned long i)   const   ;
void    operator +  (long i)                    ;
void    operator +  (long i)            const   ;
void    operator -  (unsigned long i)           ;
void    operator -  (unsigned long i)   const   ;
void    operator -  (long i)                    ;
void    operator -  (long i)            const   ;
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
void    operator +  (unsigned int i)            ;
void    operator +  (unsigned int i)    const   ;
void    operator +  (int i)                     ;
void    operator +  (int i)             const   ;
void    operator -  (unsigned int i)            ;
void    operator -  (unsigned int i)    const   ;
void    operator -  (int i)                     ;
void    operator -  (int i)             const   ;
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
void    operator +  (unsigned short i)          ;
void    operator +  (unsigned short i)  const   ;
void    operator +  (short i)                   ;
void    operator +  (short i)           const   ;
void    operator -  (unsigned short i)          ;
void    operator -  (unsigned short i)  const   ;
void    operator -  (short i)                   ;
void    operator -  (short i)           const   ;
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

#endif/*ALLOW_EXARRAY_ADDSUB*/

private:
/*
    Блокировка операций для выдачи более понятной диагностики.
*/
#ifdef  DISALLOW_EXARRAY_LONGTYPE
void    operator [] (unsigned long i)           ;
void    operator [] (unsigned long i)   const   ;
void    operator [] (long i)                    ;
void    operator [] (long i)            const   ;
void    operator +  (unsigned long i)           ;
void    operator +  (unsigned long i)   const   ;
void    operator +  (long i)                    ;
void    operator +  (long i)            const   ;
void    operator -  (unsigned long i)           ;
void    operator -  (unsigned long i)   const   ;
void    operator -  (long i)                    ;
void    operator -  (long i)            const   ;
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifdef  DISALLOW_EXARRAY_INTTYPE
void    operator [] (unsigned int i)            ;
void    operator [] (unsigned int i)    const   ;
void    operator [] (int i)                     ;
void    operator [] (int i)             const   ;
void    operator +  (unsigned int i)            ;
void    operator +  (unsigned int i)    const   ;
void    operator +  (int i)                     ;
void    operator +  (int i)             const   ;
void    operator -  (unsigned int i)            ;
void    operator -  (unsigned int i)    const   ;
void    operator -  (int i)                     ;
void    operator -  (int i)             const   ;
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifdef  DISALLOW_EXARRAY_SHORTTYPE
void    operator [] (unsigned short i)          ;
void    operator [] (unsigned short i)  const   ;
void    operator [] (short i)                   ;
void    operator [] (short i)           const   ;
void    operator +  (unsigned short i)          ;
void    operator +  (unsigned short i)  const   ;
void    operator +  (short i)                   ;
void    operator +  (short i)           const   ;
void    operator -  (unsigned short i)          ;
void    operator -  (unsigned short i)  const   ;
void    operator -  (short i)                   ;
void    operator -  (short i)           const   ;
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

void    operator [] (unsigned char i)           ;
void    operator [] (unsigned char i)   const   ;
void    operator [] (signed char i)             ;
void    operator [] (signed char i)     const   ;
void    operator +  (unsigned char i)           ;
void    operator +  (unsigned char i)   const   ;
void    operator +  (signed char i)             ;
void    operator +  (signed char i)     const   ;
void    operator -  (unsigned char i)           ;
void    operator -  (unsigned char i)   const   ;
void    operator -  (signed char i)             ;
void    operator -  (signed char i)     const   ;
#ifdef  ALLOW_EXARRAY_CHARTYPE
void    operator [] (char i)                    ;
void    operator [] (char i)            const   ;
void    operator +  (char i)                    ;
void    operator +  (char i)            const   ;
void    operator -  (char i)                    ;
void    operator -  (char i)            const   ;
#endif/*ALLOW_EXARRAY_CHARTYPE*/
};
/*
    Конструктор регулируемого массива
    с заданным начальным числом элементов.
    Реализация учитывает возможность запуска
    исключения при выделении памяти или
    конструировании базового массива - в этом
    случае строится пустой регулируемый массив,
    для которого не требуется деструктор.
*/
template <class T> exvector<T>::exvector (size_t n)
{
/*
    Создание пустого массива в рабочем объекте.
*/
    exvector<T> tmp;
    e = tmp.e; len = tmp.len;
/*
    Выделение памяти и конструирование базового
    массива в рабочем объекте. Если запустится
    исключение, то остатки базового массива
    будет удалены деструктором рабочего объекта.
*/
    tmp.resize (n);
/*
    Меняем местами массивы в рабочем и конструируемом
    объекте.
*/
    swap (tmp);
}

/* --------------------------------------------------------------
    Шаблон динамического массива с
    неограниченным числом элементов.
-------------------------------------------------------------- */

template <class T> class exarray: public explace<T>
{
#ifdef  ALLOW_EXARRAY_USING
protected:
    using exblock<T>::e;
    using exblock<T>::len;
    using exblock<T>::stub;
public:
    using exblock<T>::clear;
    using exblock<T>::swap;
    using exblock<T>::base;
    using exblock<T>::access;
private:
#endif/*ALLOW_EXARRAY_USING*/
/*
    Для блокировки присваивания динамических
    массивов и их передачи в функции по
    значению объявляются приватные конструктор
    копирования и операция присваивания.
    Динамические массивы можно передавать в
    функции по ссылке, через динамический
    или ограниченный указатель, а также через
    указатель const T*.
*/
    exarray                (const exarray<T>&);
    exarray<T>& operator = (const exarray<T>&);

public:
/*
    Конструктор динамического массива.
    Поскольку e должно быть отлично от 0, то e присваивается
    указатель на статический массив типа T.
*/
    exarray ()  { e = stub(); len = 0; }
/*
    Конструктор динамического массива
    с заданным начальным числом элементов.
*/
/*explicit*/ exarray (size_t n);
/*
    Деструктор выполняет освобождение памяти.
*/
    ~exarray()  { clear(); }
/*
    Автоматическое преобразование в const T*
    применяется при передаче базового массива
    в функцию через параметр const T* и при
    логических операциях. Логическое значение
    динамического массива - истина.
*/
    operator const T*()   const { return base();        }
/*
    Доступ к массиву с проверкой индекса
    и автоматическим увеличением размера.
*/
#ifdef  ALLOW_EXARRAY_SIZETYPE
INDECL       T& operator [] (size_t i)               { return access(i);}
INDECL const T& operator [] (size_t i)         const { return access(i);}
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
INDECL       T& operator [] (ptrdiff_t i)            { return access((size_t)i);}
INDECL const T& operator [] (ptrdiff_t i)      const { return access((size_t)i);}
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
INDECL       T& operator [] (unsigned long i)        { return access((size_t)i);}
INDECL const T& operator [] (unsigned long i)  const { return access((size_t)i);}
INDECL       T& operator [] (long i)                 { return access((size_t)i);}
INDECL const T& operator [] (long i)           const { return access((size_t)i);}
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
INDECL       T& operator [] (unsigned int i)         { return access(i);}
INDECL const T& operator [] (unsigned int i)   const { return access(i);}
INDECL       T& operator [] (int i)                  { return access(i);}
INDECL const T& operator [] (int i)            const { return access(i);}
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
INDECL       T& operator [] (unsigned short i)       { return access(i);}
INDECL const T& operator [] (unsigned short i) const { return access(i);}
INDECL       T& operator [] (short i)                { return access(i);}
INDECL const T& operator [] (short i)          const { return access(i);}
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/
INDECL       T& operator *  ()                       { return access(0);}
INDECL const T& operator *  ()                 const { return access(0);}
#ifdef  ALLOW_EXARRAY_SELECTION
INDECL       T* operator -> ()                { return (T*)&(access(0));}
INDECL const T* operator -> ()        const   { return (T*)&(access(0));}
#endif/*ALLOW_EXARRAY_SELECTION*/
/*
    Арифметические операции.
*/
#ifdef  ALLOW_EXARRAY_ADDSUB

private:
expoint<T> _add (size_t i) const{ return (expoint<T>(*(explace<T>*)this, i));}
expoint<T> _sub (size_t i) const{ return (expoint<T>(*(explace<T>*)this,-i));}
public:

#ifdef  ALLOW_EXARRAY_SIZETYPE
      expoint<T>  operator +  (size_t i)                { return (_add (i)); }
const expoint<T>  operator +  (size_t i)         const  { return (_add (i)); }
      expoint<T>  operator -  (size_t i)                { return (_sub (i)); }
const expoint<T>  operator -  (size_t i)         const  { return (_sub (i)); }
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
      expoint<T>  operator +  (ptrdiff_t i)             { return (_add ((size_t)i)); }
const expoint<T>  operator +  (ptrdiff_t i)      const  { return (_add ((size_t)i)); }
      expoint<T>  operator -  (ptrdiff_t i)             { return (_sub ((size_t)i)); }
const expoint<T>  operator -  (ptrdiff_t i)      const  { return (_sub ((size_t)i)); }
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
      expoint<T>  operator +  (unsigned long i)         { return (_add ((size_t)i)); }
const expoint<T>  operator +  (unsigned long i)  const  { return (_add ((size_t)i)); }
      expoint<T>  operator +  (long i)                  { return (_add ((size_t)i)); }
const expoint<T>  operator +  (long i)           const  { return (_add ((size_t)i)); }
      expoint<T>  operator -  (unsigned long i)         { return (_sub ((size_t)i)); }
const expoint<T>  operator -  (unsigned long i)  const  { return (_sub ((size_t)i)); }
      expoint<T>  operator -  (long i)                  { return (_sub ((size_t)i)); }
const expoint<T>  operator -  (long i)           const  { return (_sub ((size_t)i)); }
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
      expoint<T>  operator +  (unsigned int i)          { return (_add (i)); }
const expoint<T>  operator +  (unsigned int i)   const  { return (_add (i)); }
      expoint<T>  operator +  (int i)                   { return (_add (i)); }
const expoint<T>  operator +  (int i)            const  { return (_add (i)); }
      expoint<T>  operator -  (unsigned int i)          { return (_sub (i)); }
const expoint<T>  operator -  (unsigned int i)   const  { return (_sub (i)); }
      expoint<T>  operator -  (int i)                   { return (_sub (i)); }
const expoint<T>  operator -  (int i)            const  { return (_sub (i)); }
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
      expoint<T>  operator +  (unsigned short i)        { return (_add (i)); }
const expoint<T>  operator +  (unsigned short i) const  { return (_add (i)); }
      expoint<T>  operator +  (short i)                 { return (_add (i)); }
const expoint<T>  operator +  (short i)          const  { return (_add (i)); }
      expoint<T>  operator -  (unsigned short i)        { return (_sub (i)); }
const expoint<T>  operator -  (unsigned short i) const  { return (_sub (i)); }
      expoint<T>  operator -  (short i)                 { return (_sub (i)); }
const expoint<T>  operator -  (short i)          const  { return (_sub (i)); }
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

#else /*ALLOW_EXARRAY_ADDSUB*/
/*
    Блокировка арифметических операций.
    Если это не сделать, то будет проводиться
    преобразование в const T* и операции над
    указателями.
*/
private:

#ifdef  ALLOW_EXARRAY_SIZETYPE
void    operator +  (size_t i)                  ;
void    operator +  (size_t i)          const   ;
void    operator -  (size_t i)                  ;
void    operator -  (size_t i)          const   ;
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
void    operator +  (ptrdiff_t i)               ;
void    operator +  (ptrdiff_t i)       const   ;
void    operator -  (ptrdiff_t i)               ;
void    operator -  (ptrdiff_t i)       const   ;
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
void    operator +  (unsigned long i)           ;
void    operator +  (unsigned long i)   const   ;
void    operator +  (long i)                    ;
void    operator +  (long i)            const   ;
void    operator -  (unsigned long i)           ;
void    operator -  (unsigned long i)   const   ;
void    operator -  (long i)                    ;
void    operator -  (long i)            const   ;
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
void    operator +  (unsigned int i)            ;
void    operator +  (unsigned int i)    const   ;
void    operator +  (int i)                     ;
void    operator +  (int i)             const   ;
void    operator -  (unsigned int i)            ;
void    operator -  (unsigned int i)    const   ;
void    operator -  (int i)                     ;
void    operator -  (int i)             const   ;
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
void    operator +  (unsigned short i)          ;
void    operator +  (unsigned short i)  const   ;
void    operator +  (short i)                   ;
void    operator +  (short i)           const   ;
void    operator -  (unsigned short i)          ;
void    operator -  (unsigned short i)  const   ;
void    operator -  (short i)                   ;
void    operator -  (short i)           const   ;
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

#endif/*ALLOW_EXARRAY_ADDSUB*/

private:
/*
    Блокировка операций для выдачи более понятной диагностики.
*/
#ifdef  DISALLOW_EXARRAY_LONGTYPE
void    operator [] (unsigned long i)           ;
void    operator [] (unsigned long i)   const   ;
void    operator [] (long i)                    ;
void    operator [] (long i)            const   ;
void    operator +  (unsigned long i)           ;
void    operator +  (unsigned long i)   const   ;
void    operator +  (long i)                    ;
void    operator +  (long i)            const   ;
void    operator -  (unsigned long i)           ;
void    operator -  (unsigned long i)   const   ;
void    operator -  (long i)                    ;
void    operator -  (long i)            const   ;
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifdef  DISALLOW_EXARRAY_INTTYPE
void    operator [] (unsigned int i)            ;
void    operator [] (unsigned int i)    const   ;
void    operator [] (int i)                     ;
void    operator [] (int i)             const   ;
void    operator +  (unsigned int i)            ;
void    operator +  (unsigned int i)    const   ;
void    operator +  (int i)                     ;
void    operator +  (int i)             const   ;
void    operator -  (unsigned int i)            ;
void    operator -  (unsigned int i)    const   ;
void    operator -  (int i)                     ;
void    operator -  (int i)             const   ;
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifdef  DISALLOW_EXARRAY_SHORTTYPE
void    operator [] (unsigned short i)          ;
void    operator [] (unsigned short i)  const   ;
void    operator [] (short i)                   ;
void    operator [] (short i)           const   ;
void    operator +  (unsigned short i)          ;
void    operator +  (unsigned short i)  const   ;
void    operator +  (short i)                   ;
void    operator +  (short i)           const   ;
void    operator -  (unsigned short i)          ;
void    operator -  (unsigned short i)  const   ;
void    operator -  (short i)                   ;
void    operator -  (short i)           const   ;
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

void    operator [] (unsigned char i)           ;
void    operator [] (unsigned char i)   const   ;
void    operator [] (signed char i)             ;
void    operator [] (signed char i)     const   ;
void    operator +  (unsigned char i)           ;
void    operator +  (unsigned char i)   const   ;
void    operator +  (signed char i)             ;
void    operator +  (signed char i)     const   ;
void    operator -  (unsigned char i)           ;
void    operator -  (unsigned char i)   const   ;
void    operator -  (signed char i)             ;
void    operator -  (signed char i)     const   ;
#ifdef  ALLOW_EXARRAY_CHARTYPE
void    operator [] (char i)                    ;
void    operator [] (char i)            const   ;
void    operator +  (char i)                    ;
void    operator +  (char i)            const   ;
void    operator -  (char i)                    ;
void    operator -  (char i)            const   ;
#endif/*ALLOW_EXARRAY_CHARTYPE*/
};
/*
    Конструктор динамического массива
    с заданным начальным числом элементов.
    Реализация учитывает возможность запуска
    исключения при выделении памяти или
    конструировании базового массива - в этом
    случае строится пустой динамический массив,
    для которого не требуется деструктор.
*/
template <class T> exarray<T>::exarray (size_t n)
{
/*
    Создание пустого массива в рабочем объекте.
*/
    exarray<T> tmp;
    e = tmp.e; len = tmp.len;
/*
    Выделение памяти и конструирование базового
    массива в рабочем объекте. Если запустится
    исключение, то остатки базового массива
    будет удалены деструктором рабочего объекта.
*/
    tmp.resize (n);
/*
    Меняем местами массивы в рабочем и
    конструируемом объекте.
*/
    swap (tmp);
}

/* --------------------------------------------------------------
    Шаблон динамического указателя.

    Динамический указатель содержит смещение относительно
    начала динамического массива, которое может выходить
    за границу массива.  Смещение может быть отрицательным,
    в этом случае оно эквивалентно большому положительному
    смещению с тем же двоичным кодом.
-------------------------------------------------------------- */

template <class T> class expoint
{
protected:
    explace<T>* a;  /* Динамический массив */
    size_t      k;  /* Смещение в массиве */
                    /* Вычитание смещения */
    size_t  lf  (size_t n)    const
    {
        if (n < (size_t)k) n = k; return (n - k);
    }

public:
/*
    Конструктор динамического указателя с начальным значением 0.
    Если указателю не присвоен динамический массив, то
    при попытке обращения по указателю вызывается функция,
    присвоенная ::exalloc_status.null_handler.
*/
    expoint ()          { a = (explace<T>*)&exnull_c; k = 0;   }
/*
    Конструктор для присваивания числового значения.
    В отличие от обычного указателя, динамическому указателю
    можно без явного преобразования типа присвоить как нулевое,
    так и ненулевое числовое значение. Независимо от числового
    значения при попытке обращения по указателю вызывается функция,
    присвоенная ::exalloc_status.null_handler.
*/
    expoint (size_t i)  { a = (explace<T>*)&exnull_c; k = i;   }
/*
    Конструктор для присваивания динамического массива.
    После присваивания становится возможным обращение к
    элементам массива с автоматическим увеличением его размера.
*/
    expoint (const explace<T>&m, size_t i=0) { a = (explace<T>*)&m; k = i; }
/*
    Конструктор копирования и деструктор задаются по умолчанию.
    Операции присваивания задаются явным образом, чтобы
    разрешить присваивание константным указателям.
*/
      expoint<T>& operator = (size_t i)
        { a = (explace<T>*)&exnull_c; k = i;
          return *this;                                     }

const expoint<T>& operator = (size_t i)             const
        { return *(expoint<T>*)this = i;                    }

      expoint<T>& operator = (explace<T>& m)
        { a = &m; k = 0; return *this;                      }

const expoint<T>& operator = (explace<T>& m)        const
        { return *(expoint<T>*)this = *(explace<T>*)&m;     }

const expoint<T>& operator = (const explace<T>& m)  const
        { return *(expoint<T>*)this = *(explace<T>*)&m;     }

#ifdef __TURBOC__

      expoint<T>& operator = (      expoint<T>& m)
            { a = m.a; k = m.k; return *this;               }

const expoint<T>& operator = (      expoint<T>& m)  const
            { return *(expoint<T>*)this = *(expoint<T>*)&m; }

const expoint<T>& operator = (const expoint<T>& m)  const
            { return *(expoint<T>*)this = *(expoint<T>*)&m; }

#else /*__TURBOC__*/

      expoint<T>& operator = (const expoint<T>& m)  const
        { (*(expoint<T>*)this).a = m.a;
          (*(expoint<T>*)this).k = m.k;
          return *(expoint<T>*)this;                        }

#endif/*__TURBOC__*/
/*
    Доступ к динамическому массиву.
*/
      explace<T>& place()               { return *a; }
const explace<T>& place()       const   { return *a; }
/*
    Распределение памяти точно под указанное число элементов.
*/
    void  reallocate (size_t n)         { a->reallocate(exadd(n,k));}
/*
    STL-методы управления размером.
*/
    size_t  size     ()         const   { return lf(a->size());     }
    size_t  capacity ()         const   { return lf(a->capacity()); }
    size_t  max_size ()         const   { return lf(a->max_size()); }
    int     empty    ()         const   { return (size() == 0);     }
    void    resize   (size_t n)         { a->resize (exadd(n,k));   }
/*
    Установка максимального индекса.
*/
    void    confine  (size_t i)         { a->confine (exadd(i,k));  }
/*
    Эвристические методы управления размером.
*/
    void    adjust   (size_t i)         { a->adjust (exadd(i,k));   }
    void    adjust   (size_t i, const T** p)
                                        { a->adjust (exadd(i,k),p); }
    void    expand   (size_t i) const   { a->expand (exadd(i,k));   }
    void    expand   (size_t i, const T** p)
                                const   { a->expand (exadd(i,k),p); }
    void    shrink   (size_t i)         { a->shrink (exadd(i,k));   }
/*
    Доступ к массиву без проверки индекса.
    Ссылки на элемент и указатели действительны
    до тех пор, пока размер массива не изменится.
*/
      T&  item   (size_t i)             { return *(a->base()+k+i);  }
const T&  item   (size_t i)     const   { return *(a->base()+k+i);  }
      T*  base   ()                     { return  (a->base()+k);    }
const T*  base   ()             const   { return  (a->base()+k);    }
/*
    Доступ к массиву с проверкой индекса.
    При выходе индекса за границы массива вызывается
    функция, присвоенная ::exalloc_status.range_handler.
*/
INDECL       T&  at     (size_t i)      { return (a->at (k+i));     }
INDECL const T&  at     (size_t i) const{ return (
                                 ((const explace<T>*)a)->at (k+i)); }
/*
    Доступ к массиву с проверкой индекса
    и автоматическим увеличением размера.
    При выходе индекса за границы массива
    размер массива автоматически увеличивается.
    При увеличении размера проводится обнуление
    памяти, после чего для всех новых элементов
    выполняются конструкторы по умолчанию.
    При успешном распределении памяти выдается
    ссылка на элемент массива для извлечения или
    присваивания значения. Ссылка действительна
    до тех пор, пока размер массива не изменится.
*/
INDECL       T&  access (size_t i)      { return (a->access (k+i));     }
INDECL const T&  access (size_t i) const{ return (
                                 ((const explace<T>*)a)->access (k+i)); }

#ifdef  ALLOW_EXARRAY_ITERATORS
/*
    Определения для библиотеки STL.
    Динамический указатель может выступать как
    контейнер с ограниченным набором методов и как
    полноценный итератор с произвольным доступом.
*/
typedef std::random_access_iterator_tag iterator_category;
                                        /* Категория итератора */
typedef       T         value_type;     /* Тип элемента */
typedef    size_t       size_type;      /* size() */
typedef ptrdiff_t       difference_type;/* operator - */
#ifdef  NCHECKPTR
typedef       T*        iterator;       /* begin(), end() */
typedef const T*        const_iterator; /* begin(), end() */
#else /*NCHECKPTR*/
typedef       exptr<T>  iterator;       /* begin(), end() */
typedef const exptr<T>  const_iterator; /* begin(), end() */
#endif/*NCHECKPTR*/
typedef       T*        pointer;        /* operator -> */
typedef const T*        const_pointer;  /* operator -> */
typedef       T&        reference;      /* operators *, [] */
typedef const T&        const_reference;/* operators *, [] */
/*
    Итераторы для библиотеки STL.
      begin ()    Указывает на текущий элемент.
      end   ()    Указывает на элемент, следующий
                  за последним.
    Если динамический указатель находится за
    пределами массива, то begin() == end().
    Обращаться к элементу *end() нельзя.
    В роли итератора выступает ограниченный указатель,
    обеспечивающий контроль индексов, а если определено
    макро NCHECKPTR - то обычный указатель. Итераторы
    действительны до тех пор, пока размер массива не
    изменится.
*/
#ifdef  NCHECKPTR

iterator        begin ()        { return base();                }
const_iterator  begin ()  const { return base();                }

iterator        end   ()        { return base()+size();         }
const_iterator  end   ()  const { return base()+size();         }

#else /*NCHECKPTR*/

iterator        begin ()        { return exptr<T>(*a,k);        }
const_iterator  begin ()  const { return exptr<T>(*a,k);        }

iterator        end   ()        { return exptr<T>(*a,k+size()); }
const_iterator  end   ()  const { return exptr<T>(*a,k+size()); }

#endif/*NCHECKPTR*/

#endif/*ALLOW_EXARRAY_ITERATORS*/
/*
    Автоматическое преобразование в const T* применяется
    при передаче базового массива в функцию через параметр
    const T* и при логических операциях. Если указателю
    присвоен динамический массив или ненулевое числовое
    значение, то логическое значение указателя - истина,
    иначе - ложь. Если индекс выходит за границу массива,
    то расширение массива не проводится.
*/
    operator const T*()   const { return base();        }
/*
    Доступ к массиву с проверкой индекса
    и автоматическим увеличением размера.
*/
#ifdef  ALLOW_EXARRAY_SIZETYPE
INDECL       T& operator [] (size_t i)               { return access(i);}
INDECL const T& operator [] (size_t i)         const { return access(i);}
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
INDECL       T& operator [] (ptrdiff_t i)            { return access((size_t)i);}
INDECL const T& operator [] (ptrdiff_t i)      const { return access((size_t)i);}
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
INDECL       T& operator [] (unsigned long i)        { return access((size_t)i);}
INDECL const T& operator [] (unsigned long i)  const { return access((size_t)i);}
INDECL       T& operator [] (long i)                 { return access((size_t)i);}
INDECL const T& operator [] (long i)           const { return access((size_t)i);}
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
INDECL       T& operator [] (unsigned int i)         { return access(i);}
INDECL const T& operator [] (unsigned int i)   const { return access(i);}
INDECL       T& operator [] (int i)                  { return access(i);}
INDECL const T& operator [] (int i)            const { return access(i);}
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
INDECL       T& operator [] (unsigned short i)       { return access(i);}
INDECL const T& operator [] (unsigned short i) const { return access(i);}
INDECL       T& operator [] (short i)                { return access(i);}
INDECL const T& operator [] (short i)          const { return access(i);}
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/
INDECL       T& operator *  ()                       { return access(0);}
INDECL const T& operator *  ()                 const { return access(0);}
#ifdef  ALLOW_EXARRAY_SELECTION
INDECL       T* operator -> ()                { return (T*)&(access(0));}
INDECL const T* operator -> ()        const   { return (T*)&(access(0));}
#endif/*ALLOW_EXARRAY_SELECTION*/
/*
    Арифметические операции.
*/
private:

expoint<T> _add (size_t i) const{ return (expoint<T> (*(explace<T>*)a, k+i));}
expoint<T> _sub (size_t i) const{ return (expoint<T> (*(explace<T>*)a, k-i));}

expoint<T>& _ad (size_t i) const
           { ((expoint<T>*)this)->k+=i; return (*(expoint<T>*)this);         }
expoint<T>& _sb (size_t i) const
           { ((expoint<T>*)this)->k-=i; return (*(expoint<T>*)this);         }
public:

#ifdef  ALLOW_EXARRAY_SIZETYPE
      expoint<T>  operator +  (size_t i)                { return (_add (i)); }
const expoint<T>  operator +  (size_t i)         const  { return (_add (i)); }
      expoint<T>  operator -  (size_t i)                { return (_sub (i)); }
const expoint<T>  operator -  (size_t i)         const  { return (_sub (i)); }
      expoint<T>& operator += (size_t i)                { return (_ad  (i)); }
const expoint<T>& operator += (size_t i)         const  { return (_ad  (i)); }
      expoint<T>& operator -= (size_t i)                { return (_sb  (i)); }
const expoint<T>& operator -= (size_t i)         const  { return (_sb  (i)); }
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
      expoint<T>  operator +  (ptrdiff_t i)             { return (_add ((size_t)i)); }
const expoint<T>  operator +  (ptrdiff_t i)      const  { return (_add ((size_t)i)); }
      expoint<T>  operator -  (ptrdiff_t i)             { return (_sub ((size_t)i)); }
const expoint<T>  operator -  (ptrdiff_t i)      const  { return (_sub ((size_t)i)); }
      expoint<T>& operator += (ptrdiff_t i)             { return (_ad  ((size_t)i)); }
const expoint<T>& operator += (ptrdiff_t i)      const  { return (_ad  ((size_t)i)); }
      expoint<T>& operator -= (ptrdiff_t i)             { return (_sb  ((size_t)i)); }
const expoint<T>& operator -= (ptrdiff_t i)      const  { return (_sb  ((size_t)i)); }
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
      expoint<T>  operator +  (unsigned long i)         { return (_add ((size_t)i)); }
const expoint<T>  operator +  (unsigned long i)  const  { return (_add ((size_t)i)); }
      expoint<T>  operator +  (long i)                  { return (_add ((size_t)i)); }
const expoint<T>  operator +  (long i)           const  { return (_add ((size_t)i)); }
      expoint<T>  operator -  (unsigned long i)         { return (_sub ((size_t)i)); }
const expoint<T>  operator -  (unsigned long i)  const  { return (_sub ((size_t)i)); }
      expoint<T>  operator -  (long i)                  { return (_sub ((size_t)i)); }
const expoint<T>  operator -  (long i)           const  { return (_sub ((size_t)i)); }
      expoint<T>& operator += (unsigned long i)         { return (_ad  ((size_t)i)); }
const expoint<T>& operator += (unsigned long i)  const  { return (_ad  ((size_t)i)); }
      expoint<T>& operator += (long i)                  { return (_ad  ((size_t)i)); }
const expoint<T>& operator += (long i)           const  { return (_ad  ((size_t)i)); }
      expoint<T>& operator -= (unsigned long i)         { return (_sb  ((size_t)i)); }
const expoint<T>& operator -= (unsigned long i)  const  { return (_sb  ((size_t)i)); }
      expoint<T>& operator -= (long i)                  { return (_sb  ((size_t)i)); }
const expoint<T>& operator -= (long i)           const  { return (_sb  ((size_t)i)); }
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
      expoint<T>  operator +  (unsigned int i)          { return (_add (i)); }
const expoint<T>  operator +  (unsigned int i)   const  { return (_add (i)); }
      expoint<T>  operator +  (int i)                   { return (_add (i)); }
const expoint<T>  operator +  (int i)            const  { return (_add (i)); }
      expoint<T>  operator -  (unsigned int i)          { return (_sub (i)); }
const expoint<T>  operator -  (unsigned int i)   const  { return (_sub (i)); }
      expoint<T>  operator -  (int i)                   { return (_sub (i)); }
const expoint<T>  operator -  (int i)            const  { return (_sub (i)); }
      expoint<T>& operator += (unsigned int i)          { return (_ad  (i)); }
const expoint<T>& operator += (unsigned int i)   const  { return (_ad  (i)); }
      expoint<T>& operator += (int i)                   { return (_ad  (i)); }
const expoint<T>& operator += (int i)            const  { return (_ad  (i)); }
      expoint<T>& operator -= (unsigned int i)          { return (_sb  (i)); }
const expoint<T>& operator -= (unsigned int i)   const  { return (_sb  (i)); }
      expoint<T>& operator -= (int i)                   { return (_sb  (i)); }
const expoint<T>& operator -= (int i)            const  { return (_sb  (i)); }
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
      expoint<T>  operator +  (unsigned short i)        { return (_add (i)); }
const expoint<T>  operator +  (unsigned short i) const  { return (_add (i)); }
      expoint<T>  operator +  (short i)                 { return (_add (i)); }
const expoint<T>  operator +  (short i)          const  { return (_add (i)); }
      expoint<T>  operator -  (unsigned short i)        { return (_sub (i)); }
const expoint<T>  operator -  (unsigned short i) const  { return (_sub (i)); }
      expoint<T>  operator -  (short i)                 { return (_sub (i)); }
const expoint<T>  operator -  (short i)          const  { return (_sub (i)); }
      expoint<T>& operator += (unsigned short i)        { return (_ad  (i)); }
const expoint<T>& operator += (unsigned short i) const  { return (_ad  (i)); }
      expoint<T>& operator += (short i)                 { return (_ad  (i)); }
const expoint<T>& operator += (short i)          const  { return (_ad  (i)); }
      expoint<T>& operator -= (unsigned short i)        { return (_sb  (i)); }
const expoint<T>& operator -= (unsigned short i) const  { return (_sb  (i)); }
      expoint<T>& operator -= (short i)                 { return (_sb  (i)); }
const expoint<T>& operator -= (short i)          const  { return (_sb  (i)); }
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

      expoint<T>& operator ++ ()          { k++; return (*this);             }
const expoint<T>& operator ++ ()    const { return ++(*((expoint<T>*)this)); }

      expoint<T>& operator -- ()          { k--; return (*this);             }
const expoint<T>& operator -- ()    const { return --(*((expoint<T>*)this)); }

      expoint<T>  operator ++ (int)       { expoint<T>t=*this; k++; return t;}
      expoint<T>  operator ++ (int) const { return (*((expoint<T>*)this))++; }

      expoint<T>  operator -- (int)       { expoint<T>t=*this; k--; return t;}
      expoint<T>  operator -- (int) const { return (*((expoint<T>*)this))--; }

private:
/*
    Блокировка операций для выдачи более понятной диагностики.
*/
#ifdef  DISALLOW_EXARRAY_LONGTYPE
void    operator [] (unsigned long i)           ;
void    operator [] (unsigned long i)   const   ;
void    operator [] (long i)                    ;
void    operator [] (long i)            const   ;
void    operator +  (unsigned long i)           ;
void    operator +  (unsigned long i)   const   ;
void    operator +  (long i)                    ;
void    operator +  (long i)            const   ;
void    operator -  (unsigned long i)           ;
void    operator -  (unsigned long i)   const   ;
void    operator -  (long i)                    ;
void    operator -  (long i)            const   ;
void    operator += (unsigned long i)           ;
void    operator += (unsigned long i)   const   ;
void    operator += (long i)                    ;
void    operator += (long i)            const   ;
void    operator -= (unsigned long i)           ;
void    operator -= (unsigned long i)   const   ;
void    operator -= (long i)                    ;
void    operator -= (long i)            const   ;
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifdef  DISALLOW_EXARRAY_INTTYPE
void    operator [] (unsigned int i)            ;
void    operator [] (unsigned int i)    const   ;
void    operator [] (int i)                     ;
void    operator [] (int i)             const   ;
void    operator +  (unsigned int i)            ;
void    operator +  (unsigned int i)    const   ;
void    operator +  (int i)                     ;
void    operator +  (int i)             const   ;
void    operator -  (unsigned int i)            ;
void    operator -  (unsigned int i)    const   ;
void    operator -  (int i)                     ;
void    operator -  (int i)             const   ;
void    operator += (unsigned int i)            ;
void    operator += (unsigned int i)    const   ;
void    operator += (int i)                     ;
void    operator += (int i)             const   ;
void    operator -= (unsigned int i)            ;
void    operator -= (unsigned int i)    const   ;
void    operator -= (int i)                     ;
void    operator -= (int i)             const   ;
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifdef  DISALLOW_EXARRAY_SHORTTYPE
void    operator [] (unsigned short i)          ;
void    operator [] (unsigned short i)  const   ;
void    operator [] (short i)                   ;
void    operator [] (short i)           const   ;
void    operator +  (unsigned short i)          ;
void    operator +  (unsigned short i)  const   ;
void    operator +  (short i)                   ;
void    operator +  (short i)           const   ;
void    operator -  (unsigned short i)          ;
void    operator -  (unsigned short i)  const   ;
void    operator -  (short i)                   ;
void    operator -  (short i)           const   ;
void    operator += (unsigned short i)          ;
void    operator += (unsigned short i)  const   ;
void    operator += (short i)                   ;
void    operator += (short i)           const   ;
void    operator -= (unsigned short i)          ;
void    operator -= (unsigned short i)  const   ;
void    operator -= (short i)                   ;
void    operator -= (short i)           const   ;
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

void    operator [] (unsigned char i)           ;
void    operator [] (unsigned char i)   const   ;
void    operator [] (signed char i)             ;
void    operator [] (signed char i)     const   ;
void    operator +  (unsigned char i)           ;
void    operator +  (unsigned char i)   const   ;
void    operator +  (signed char i)             ;
void    operator +  (signed char i)     const   ;
void    operator -  (unsigned char i)           ;
void    operator -  (unsigned char i)   const   ;
void    operator -  (signed char i)             ;
void    operator -  (signed char i)     const   ;
void    operator += (unsigned char i)           ;
void    operator += (unsigned char i)   const   ;
void    operator += (signed char i)             ;
void    operator += (signed char i)     const   ;
void    operator -= (unsigned char i)           ;
void    operator -= (unsigned char i)   const   ;
void    operator -= (signed char i)             ;
void    operator -= (signed char i)     const   ;
#ifdef  ALLOW_EXARRAY_CHARTYPE
void    operator [] (char i)                    ;
void    operator [] (char i)            const   ;
void    operator +  (char i)                    ;
void    operator +  (char i)            const   ;
void    operator -  (char i)                    ;
void    operator -  (char i)            const   ;
void    operator += (char i)                    ;
void    operator += (char i)            const   ;
void    operator -= (char i)                    ;
void    operator -= (char i)            const   ;
#endif/*ALLOW_EXARRAY_CHARTYPE*/
};

/* --------------------------------------------------------------
    Шаблон ограниченного указателя с контролем индексов.

    Ограниченный указатель содержит информацию
    о начале и длине массива, а также смещение
    относительно начала массива, которое может
    выходить за границу массива. Смещение может
    быть отрицательным, в этом случае оно
    эквивалентно большому положительному
    смещению с тем же двоичным кодом.
-------------------------------------------------------------- */

template <class T> class exptr
{
protected:
    T*      e;      /* Базовый массив */
    size_t  len;    /* Число элементов */
    size_t  k;      /* Смещение в массиве */
    T*      x;      /* Указатель для производных классов (здесь NULL) */
    size_t  lf  (size_t n)      const
    {
        if (n < (size_t)k) n = k; return (n - k);
    }
/*
    Конструктор 1:1.
*/
    exptr (const T* me, size_t mlen, size_t mk, const T* mx)
            { e = (T*)me; len = mlen; k = mk; x = (T*)mx;       }
/*
    Прерывание работы программы, либо выдача
    указателя на статическую область памяти
    при обращении по индексу i >= len.
*/
      T*  range  ()             const;
      T*  range  (size_t i)             { return range()-i;     }
const T*  range  (size_t i)     const   { return range()-i;     }

public:
/*
    Проверка размера и выдача обычного указателя.
*/
protected:
      T*  check_ (size_t n)     const;
public:
      T*  check  (size_t n)             { return check_(n);     }
const T*  check  (size_t n)     const   { return check_(n);     }
/*
    Проверка доступности индексов 0..i и выдача обычного указателя.
*/
protected:
      T*  index_ (size_t i)     const;
public:
      T*  index  (size_t i)             { return index_(i);     }
const T*  index  (size_t i)     const   { return index_(i);     }
/*
    Конструктор ограниченного NULL-указателя.
    Если указателю не присвоен массив, то его логическое
    значение - ложь, а при попытке обращения по указателю
    вызывается функция, присвоенная
    ::exalloc_status.null_handler.
*/
    exptr ()          { e = NULL; len = 0; k = 0; x = NULL;     }
/*
    Конструктор для присваивания обычного указателя.
    Если исходный указатель нулевой, то при попытке
    обращения по ограниченному указателю вызывается
    функция, присвоенная ::exalloc_status.null_handler.
    Если исходный указатель ненулевой, то после
    присваивания становится возможным обращение по
    индексу 0, а для остальных индексов вызывается
    функция, присвоенная ::exalloc_status.range_handler.
    Логическое значение указателя совпадает с
    логическим значением присвоенного указателя.
*/
    exptr (const T* p)
            { e = (T*)p; len = (e != NULL); k = 0; x = NULL;    }
/*
    Конструктор для присваивания обычного массива.
    Если исходный указатель нулевой, то при попытке
    обращения по ограниченному указателю вызывается
    функция, присвоенная ::exalloc_status.null_handler.
    Если исходный указатель ненулевой, то после
    присваивания становится возможным обращение к
    элементам массива с контролем индексов.
    Логическое значение указателя совпадает с
    логическим значением присвоенного указателя.
*/
    exptr (const T* m, size_t n, size_t i = 0)
            { if ((e = (T*)m) == NULL) n = 0; len = n; k = i; x = NULL; }
/*
    Конструкторы для присваивания динамического массива
    и указателя. Размер массива не должен меняться.
    При обращении за пределами массива, вызывается
    функция, присвоенная ::exalloc_status.range_handler.
*/
    exptr (const exblock<T>&m, size_t i = 0)
            { e = (T*)m.base(); len = m.size(); k = i; x = NULL;    }
    exptr (const expoint<T>&m, size_t i = 0)
            { e = (T*)m.base(); len = m.size(); k = i; x = NULL;    }
/*
    Конструктор для выделения подмассива с проверкой индекса.
*/
    exptr (const exptr<T>&m, size_t n, size_t i = 0)
            { e = (T*)m.check(n); len = n; k = i; x = m.x;          }
/*
    Конструктор копирования и деструктор задаются по умолчанию.
    Операция присваивания задается явным образом, чтобы
    разрешить присваивание константным указателям.
*/
      exptr<T>& operator = (T* p)
            { e = p; len = (e != NULL); k = 0; x = NULL;
              return *this;                                 }

const exptr<T>& operator = (const T* p)             const
            { return *(exptr<T>*)this = (T*)p;              }

      exptr<T>& operator = (exblock<T>& m)
            { e = m.base(); len = m.size(); k = 0; x = NULL;
              return *this;                                 }

const exptr<T>& operator = (exblock<T>& m)          const
            { return *(exptr<T>*)this = *(exblock<T>*)&m;   }

const exptr<T>& operator = (const exblock<T>& m)    const
            { return *(exptr<T>*)this = *(exblock<T>*)&m;   }

      exptr<T>& operator = (expoint<T>& m)
            { e = m.base(); len = m.size(); k = 0; x = NULL;
              return *this;                                 }

const exptr<T>& operator = (expoint<T>& m)          const
            { return *(exptr<T>*)this = *(expoint<T>*)&m;   }

const exptr<T>& operator = (const expoint<T>& m)    const
            { return *(exptr<T>*)this = *(expoint<T>*)&m;   }

#ifdef  __TURBOC__

      exptr<T>& operator = (exptr<T>& m)
            { e = m.e; len = m.len; k = m.k; x = m.x;
              return *this;                                 }

const exptr<T>& operator = (exptr<T>& m)            const
            { return *(exptr<T>*)this = *(exptr<T>*)&m;     }

const exptr<T>& operator = (const exptr<T>& m)      const
            { return *(exptr<T>*)this = *(exptr<T>*)&m;     }

#else /*__TURBOC__*/

      exptr<T>& operator = (const exptr<T>& m)      const
            { (*(exptr<T>*)this).e   = m.e;
              (*(exptr<T>*)this).len = m.len;
              (*(exptr<T>*)this).k   = m.k;
              (*(exptr<T>*)this).x   = m.x;
              return *(exptr<T>*)this;                      }

#endif/*__TURBOC__*/
/*
    Методы распределения и освобождения памяти
    с вызовом конструкторов и деструкторов.
*/
    exptr<T>& allocate (size_t n)
        { e = EXNEW (T, n); len = n; return *this; }

    void deallocate ()
        { EXDELETE (e); e = NULL; len = 0; }
/*
    Методы распределения и освобождения памяти
    без вызова конструкторов и деструкторов.
*/
    exptr<T>& alloc (size_t n)
        { e = EXALLOC (T, n); len = n; return *this; }

    void free ()
        { EXFREE (e); e = NULL; len = 0; }

    exptr<T>& aligned_alloc (size_t n, size_t blockalign)
        { e = EXALIGNED_ALLOC (T, n, blockalign); len = n; return *this; }

    void aligned_free ()
        { EXALIGNED_FREE (e); e = NULL; len = 0; }
/*
    Доступ к массиву без проверки индекса.
*/
      T&  item   (size_t i)             { return *(e+k+i);      }
const T&  item   (size_t i)     const   { return *(e+k+i);      }
      T*  base   ()                     { return  (e+k);        }
const T*  base   ()             const   { return  (e+k);        }
/*
    Доступ к массиву с проверкой индекса.
    При выходе индекса за границы массива вызывается
    функция, присвоенная ::exalloc_status.range_handler.
*/
private:

#ifdef  ALLOW_EXARRAY_SCHEDULING

INDECL       T&  at_  (size_t i)      {       register T* p = e;
                if (len <= (size_t)i) p = range(i); return p[i]; }
INDECL const T&  at_  (size_t i) const{ const register T* p = e;
                if (len <= (size_t)i) p = range(i); return p[i]; }

#else /*ALLOW_EXARRAY_SCHEDULING*/

INDECL       T&  at_  (size_t i)
                { if (len <= (size_t)i) range (i); return e[i]; }
INDECL const T&  at_  (size_t i) const
                { if (len <= (size_t)i) range (i); return e[i]; }

#endif/*ALLOW_EXARRAY_SCHEDULING*/

public:

INDECL       T&  at   (size_t i)        { return at_(k+i);  }
INDECL const T&  at   (size_t i) const  { return at_(k+i);  }

#ifdef  ALLOW_EXARRAY_ITERATORS
/*
    Определения для библиотеки STL.
    Ограниченный указатель может выступать как
    контейнер с ограниченным набором функций и как
    полноценный итератор с произвольным доступом.
*/
typedef std::random_access_iterator_tag iterator_category;
                                        /* Категория итератора */
typedef       T         value_type;     /* Тип элемента */
typedef    size_t       size_type;      /* size() */
typedef ptrdiff_t       difference_type;/* operator - */
#ifdef  NCHECKPTR
typedef       T*        iterator;       /* begin(), end() */
typedef const T*        const_iterator; /* begin(), end() */
#else /*NCHECKPTR*/
typedef       exptr<T>  iterator;       /* begin(), end() */
typedef const exptr<T>  const_iterator; /* begin(), end() */
#endif/*NCHECKPTR*/
typedef       T*        pointer;        /* operator -> */
typedef const T*        const_pointer;  /* operator -> */
typedef       T&        reference;      /* operators *, [] */
typedef const T&        const_reference;/* operators *, [] */
/*
    Итераторы для библиотеки STL.
      begin ()    Указывает на текущий элемент.
      end   ()    Указывает на элемент, следующий
                  за последним.
    Если ограниченный указатель находится за
    пределами массива, то begin() == end().
    Обращаться к элементу *end() нельзя.
*/
#ifdef  NCHECKPTR

iterator        begin ()        { return base();        }
const_iterator  begin ()  const { return base();        }

iterator        end   ()        { return base()+size(); }
const_iterator  end   ()  const { return base()+size(); }

#else /*NCHECKPTR*/

iterator        begin ()        { return *this;         }
const_iterator  begin ()  const { return *this;         }

iterator        end   ()        { return exptr<T>(e,k,(len<(size_t)k?k:len));}
const_iterator  end   ()  const { return exptr<T>(e,k,(len<(size_t)k?k:len));}

#endif/*NCHECKPTR*/

#endif/*ALLOW_EXARRAY_ITERATORS*/
/*
    Автоматическое преобразование в const T* применяется
    при передаче базового массива в функцию через параметр
    const T* и при логических операциях. Если указателю
    присвоен массив или ненулевое числовое значение, то
    логическое значение указателя - истина, иначе - ложь.
*/
    operator const T*()     const { return base();      }
/*
    Доступ к массиву с проверкой индекса.
*/
#ifdef  ALLOW_EXARRAY_SIZETYPE
INDECL       T& operator [] (size_t i)               { return at (i);}
INDECL const T& operator [] (size_t i)         const { return at (i);}
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
INDECL       T& operator [] (ptrdiff_t i)            { return at ((size_t)i);}
INDECL const T& operator [] (ptrdiff_t i)      const { return at ((size_t)i);}
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
INDECL       T& operator [] (unsigned long i)        { return at ((size_t)i);}
INDECL const T& operator [] (unsigned long i)  const { return at ((size_t)i);}
INDECL       T& operator [] (long i)                 { return at ((size_t)i);}
INDECL const T& operator [] (long i)           const { return at ((size_t)i);}
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
INDECL       T& operator [] (unsigned int i)         { return at (i);}
INDECL const T& operator [] (unsigned int i)   const { return at (i);}
INDECL       T& operator [] (int i)                  { return at (i);}
INDECL const T& operator [] (int i)            const { return at (i);}
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
INDECL       T& operator [] (unsigned short i)       { return at (i);}
INDECL const T& operator [] (unsigned short i) const { return at (i);}
INDECL       T& operator [] (short i)                { return at (i);}
INDECL const T& operator [] (short i)          const { return at (i);}
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/
INDECL       T& operator *  ()                       { return at (0);}
INDECL const T& operator *  ()                 const { return at (0);}
#ifdef  ALLOW_EXARRAY_SELECTION
INDECL       T* operator -> ()                { return (T*)&(at (0));}
INDECL const T* operator -> ()         const  { return (T*)&(at (0));}
#endif/*ALLOW_EXARRAY_SELECTION*/
/*
    Арифметические операции.
*/
private:

exptr<T>   _add (size_t i) const    { return exptr<T>(e,len,k+i,x);     }
exptr<T>   _sub (size_t i) const    { return exptr<T>(e,len,k-i,x);     }

exptr<T>&  _ad  (size_t i) const
                { ((exptr<T>*)this)->k+=i; return (*(exptr<T>*)this);   }
exptr<T>&  _sb  (size_t i) const
                { ((exptr<T>*)this)->k-=i; return (*(exptr<T>*)this);   }
public:

#ifdef  ALLOW_EXARRAY_SIZETYPE
      exptr<T>    operator +  (size_t i)                { return (_add (i)); }
const exptr<T>    operator +  (size_t i)         const  { return (_add (i)); }
      exptr<T>    operator -  (size_t i)                { return (_sub (i)); }
const exptr<T>    operator -  (size_t i)         const  { return (_sub (i)); }
      exptr<T>&   operator += (size_t i)                { return (_ad  (i)); }
const exptr<T>&   operator += (size_t i)         const  { return (_ad  (i)); }
      exptr<T>&   operator -= (size_t i)                { return (_sb  (i)); }
const exptr<T>&   operator -= (size_t i)         const  { return (_sb  (i)); }
#endif/*ALLOW_EXARRAY_SIZETYPE*/
#ifdef  ALLOW_EXARRAY_PTRDIFFTYPE
      exptr<T>    operator +  (ptrdiff_t i)             { return (_add ((size_t)i)); }
const exptr<T>    operator +  (ptrdiff_t i)      const  { return (_add ((size_t)i)); }
      exptr<T>    operator -  (ptrdiff_t i)             { return (_sub ((size_t)i)); }
const exptr<T>    operator -  (ptrdiff_t i)      const  { return (_sub ((size_t)i)); }
      exptr<T>&   operator += (ptrdiff_t i)             { return (_ad  ((size_t)i)); }
const exptr<T>&   operator += (ptrdiff_t i)      const  { return (_ad  ((size_t)i)); }
      exptr<T>&   operator -= (ptrdiff_t i)             { return (_sb  ((size_t)i)); }
const exptr<T>&   operator -= (ptrdiff_t i)      const  { return (_sb  ((size_t)i)); }
#endif/*ALLOW_EXARRAY_PTRDIFFTYPE*/
#ifndef DISALLOW_EXARRAY_LONGTYPE
      exptr<T>    operator +  (unsigned long i)         { return (_add ((size_t)i)); }
const exptr<T>    operator +  (unsigned long i)  const  { return (_add ((size_t)i)); }
      exptr<T>    operator +  (long i)                  { return (_add ((size_t)i)); }
const exptr<T>    operator +  (long i)           const  { return (_add ((size_t)i)); }
      exptr<T>    operator -  (unsigned long i)         { return (_sub ((size_t)i)); }
const exptr<T>    operator -  (unsigned long i)  const  { return (_sub ((size_t)i)); }
      exptr<T>    operator -  (long i)                  { return (_sub ((size_t)i)); }
const exptr<T>    operator -  (long i)           const  { return (_sub ((size_t)i)); }
      exptr<T>&   operator += (unsigned long i)         { return (_ad  ((size_t)i)); }
const exptr<T>&   operator += (unsigned long i)  const  { return (_ad  ((size_t)i)); }
      exptr<T>&   operator += (long i)                  { return (_ad  ((size_t)i)); }
const exptr<T>&   operator += (long i)           const  { return (_ad  ((size_t)i)); }
      exptr<T>&   operator -= (unsigned long i)         { return (_sb  ((size_t)i)); }
const exptr<T>&   operator -= (unsigned long i)  const  { return (_sb  ((size_t)i)); }
      exptr<T>&   operator -= (long i)                  { return (_sb  ((size_t)i)); }
const exptr<T>&   operator -= (long i)           const  { return (_sb  ((size_t)i)); }
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifndef DISALLOW_EXARRAY_INTTYPE
      exptr<T>    operator +  (unsigned int i)          { return (_add (i)); }
const exptr<T>    operator +  (unsigned int i)   const  { return (_add (i)); }
      exptr<T>    operator +  (int i)                   { return (_add (i)); }
const exptr<T>    operator +  (int i)            const  { return (_add (i)); }
      exptr<T>    operator -  (unsigned int i)          { return (_sub (i)); }
const exptr<T>    operator -  (unsigned int i)   const  { return (_sub (i)); }
      exptr<T>    operator -  (int i)                   { return (_sub (i)); }
const exptr<T>    operator -  (int i)            const  { return (_sub (i)); }
      exptr<T>&   operator += (unsigned int i)          { return (_ad  (i)); }
const exptr<T>&   operator += (unsigned int i)   const  { return (_ad  (i)); }
      exptr<T>&   operator += (int i)                   { return (_ad  (i)); }
const exptr<T>&   operator += (int i)            const  { return (_ad  (i)); }
      exptr<T>&   operator -= (unsigned int i)          { return (_sb  (i)); }
const exptr<T>&   operator -= (unsigned int i)   const  { return (_sb  (i)); }
      exptr<T>&   operator -= (int i)                   { return (_sb  (i)); }
const exptr<T>&   operator -= (int i)            const  { return (_sb  (i)); }
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifndef DISALLOW_EXARRAY_SHORTTYPE
      exptr<T>    operator +  (unsigned short i)        { return (_add (i)); }
const exptr<T>    operator +  (unsigned short i) const  { return (_add (i)); }
      exptr<T>    operator +  (short i)                 { return (_add (i)); }
const exptr<T>    operator +  (short i)          const  { return (_add (i)); }
      exptr<T>    operator -  (unsigned short i)        { return (_sub (i)); }
const exptr<T>    operator -  (unsigned short i) const  { return (_sub (i)); }
      exptr<T>    operator -  (short i)                 { return (_sub (i)); }
const exptr<T>    operator -  (short i)          const  { return (_sub (i)); }
      exptr<T>&   operator += (unsigned short i)        { return (_ad  (i)); }
const exptr<T>&   operator += (unsigned short i) const  { return (_ad  (i)); }
      exptr<T>&   operator += (short i)                 { return (_ad  (i)); }
const exptr<T>&   operator += (short i)          const  { return (_ad  (i)); }
      exptr<T>&   operator -= (unsigned short i)        { return (_sb  (i)); }
const exptr<T>&   operator -= (unsigned short i) const  { return (_sb  (i)); }
      exptr<T>&   operator -= (short i)                 { return (_sb  (i)); }
const exptr<T>&   operator -= (short i)          const  { return (_sb  (i)); }
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

      exptr<T>&   operator ++ ()          { k++; return (*this);             }
const exptr<T>&   operator ++ ()    const { return ++(*((exptr<T>*)this));   }

      exptr<T>&   operator -- ()          { k--; return (*this);             }
const exptr<T>&   operator -- ()    const { return --(*((exptr<T>*)this));   }

      exptr<T>    operator ++ (int)       { exptr<T>t=*this; k++; return t;  }
      exptr<T>    operator ++ (int) const { return (*((exptr<T>*)this))++;   }

      exptr<T>    operator -- (int)       { exptr<T>t=*this; k--; return t;  }
      exptr<T>    operator -- (int) const { return (*((exptr<T>*)this))--;   }

private:
/*
    Блокировка операций для выдачи более понятной диагностики.
*/
#ifdef  DISALLOW_EXARRAY_LONGTYPE
void    operator [] (unsigned long i)           ;
void    operator [] (unsigned long i)   const   ;
void    operator [] (long i)                    ;
void    operator [] (long i)            const   ;
void    operator +  (unsigned long i)           ;
void    operator +  (unsigned long i)   const   ;
void    operator +  (long i)                    ;
void    operator +  (long i)            const   ;
void    operator -  (unsigned long i)           ;
void    operator -  (unsigned long i)   const   ;
void    operator -  (long i)                    ;
void    operator -  (long i)            const   ;
void    operator += (unsigned long i)           ;
void    operator += (unsigned long i)   const   ;
void    operator += (long i)                    ;
void    operator += (long i)            const   ;
void    operator -= (unsigned long i)           ;
void    operator -= (unsigned long i)   const   ;
void    operator -= (long i)                    ;
void    operator -= (long i)            const   ;
#endif/*DISALLOW_EXARRAY_LONGTYPE*/
#ifdef  DISALLOW_EXARRAY_INTTYPE
void    operator [] (unsigned int i)            ;
void    operator [] (unsigned int i)    const   ;
void    operator [] (int i)                     ;
void    operator [] (int i)             const   ;
void    operator +  (unsigned int i)            ;
void    operator +  (unsigned int i)    const   ;
void    operator +  (int i)                     ;
void    operator +  (int i)             const   ;
void    operator -  (unsigned int i)            ;
void    operator -  (unsigned int i)    const   ;
void    operator -  (int i)                     ;
void    operator -  (int i)             const   ;
void    operator += (unsigned int i)            ;
void    operator += (unsigned int i)    const   ;
void    operator += (int i)                     ;
void    operator += (int i)             const   ;
void    operator -= (unsigned int i)            ;
void    operator -= (unsigned int i)    const   ;
void    operator -= (int i)                     ;
void    operator -= (int i)             const   ;
#endif/*DISALLOW_EXARRAY_INTTYPE*/
#ifdef  DISALLOW_EXARRAY_SHORTTYPE
void    operator [] (unsigned short i)          ;
void    operator [] (unsigned short i)  const   ;
void    operator [] (short i)                   ;
void    operator [] (short i)           const   ;
void    operator +  (unsigned short i)          ;
void    operator +  (unsigned short i)  const   ;
void    operator +  (short i)                   ;
void    operator +  (short i)           const   ;
void    operator -  (unsigned short i)          ;
void    operator -  (unsigned short i)  const   ;
void    operator -  (short i)                   ;
void    operator -  (short i)           const   ;
void    operator += (unsigned short i)          ;
void    operator += (unsigned short i)  const   ;
void    operator += (short i)                   ;
void    operator += (short i)           const   ;
void    operator -= (unsigned short i)          ;
void    operator -= (unsigned short i)  const   ;
void    operator -= (short i)                   ;
void    operator -= (short i)           const   ;
#endif/*DISALLOW_EXARRAY_SHORTTYPE*/

void    operator [] (unsigned char i)           ;
void    operator [] (unsigned char i)   const   ;
void    operator [] (signed char i)             ;
void    operator [] (signed char i)     const   ;
void    operator +  (unsigned char i)           ;
void    operator +  (unsigned char i)   const   ;
void    operator +  (signed char i)             ;
void    operator +  (signed char i)     const   ;
void    operator -  (unsigned char i)           ;
void    operator -  (unsigned char i)   const   ;
void    operator -  (signed char i)             ;
void    operator -  (signed char i)     const   ;
void    operator += (unsigned char i)           ;
void    operator += (unsigned char i)   const   ;
void    operator += (signed char i)             ;
void    operator += (signed char i)     const   ;
void    operator -= (unsigned char i)           ;
void    operator -= (unsigned char i)   const   ;
void    operator -= (signed char i)             ;
void    operator -= (signed char i)     const   ;
#ifdef  ALLOW_EXARRAY_CHARTYPE
void    operator [] (char i)                    ;
void    operator [] (char i)            const   ;
void    operator +  (char i)                    ;
void    operator +  (char i)            const   ;
void    operator -  (char i)                    ;
void    operator -  (char i)            const   ;
void    operator += (char i)                    ;
void    operator += (char i)            const   ;
void    operator -= (char i)                    ;
void    operator -= (char i)            const   ;
#endif/*ALLOW_EXARRAY_CHARTYPE*/
};
/*
    Защищенный метод range() вызывается для прерывания
    работы программы или выдачи "безопасного" указателя
    на статическую область памяти.
*/
template <class T> T* exptr<T>::range() const
{
    exrange (e);
    static char _stub [sizeof(T)];
    return (T*)(_stub);
}
/*
    Проверка размера и выдача обычного указателя.
*/
template <class T> T* exptr<T>::check_ (size_t n) const
{
    if ((len <= (size_t)k || len - (size_t)k < n) && n > 0) range();
    return (e + k);
}
/*
    Проверка доступности индексов 0..i и выдача обычного указателя.
*/
template <class T> T* exptr<T>::index_ (size_t i) const
{
    if (len <= (size_t)k || len - (size_t)k <= i) range();
    return (e + k);
}

/* --------------------------------------------------------------
    Реализация операций сравнения.
-------------------------------------------------------------- */

template <class T> inline int   operator == (const exarray<T>&  t, const exarray<T>&  m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exarray<T>&  t, const exarray<T>&  m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exarray<T>&  t, const exarray<T>&  m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exarray<T>&  t, const exarray<T>&  m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exarray<T>&  t, const exarray<T>&  m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exarray<T>&  t, const exarray<T>&  m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exarray<T>&  t, const expoint<T>&  m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exarray<T>&  t, const expoint<T>&  m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exarray<T>&  t, const expoint<T>&  m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exarray<T>&  t, const expoint<T>&  m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exarray<T>&  t, const expoint<T>&  m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exarray<T>&  t, const expoint<T>&  m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exarray<T>&  t, const exptr<T>&    m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exarray<T>&  t, const exptr<T>&    m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exarray<T>&  t, const exptr<T>&    m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exarray<T>&  t, const exptr<T>&    m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exarray<T>&  t, const exptr<T>&    m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exarray<T>&  t, const exptr<T>&    m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exarray<T>&  t, const exvector<T>& m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exarray<T>&  t, const exvector<T>& m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exarray<T>&  t, const exvector<T>& m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exarray<T>&  t, const exvector<T>& m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exarray<T>&  t, const exvector<T>& m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exarray<T>&  t, const exvector<T>& m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const expoint<T>&  t, const exarray<T>&  m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const expoint<T>&  t, const exarray<T>&  m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const expoint<T>&  t, const exarray<T>&  m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const expoint<T>&  t, const exarray<T>&  m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const expoint<T>&  t, const exarray<T>&  m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const expoint<T>&  t, const exarray<T>&  m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const expoint<T>&  t, const expoint<T>&  m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const expoint<T>&  t, const expoint<T>&  m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const expoint<T>&  t, const expoint<T>&  m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const expoint<T>&  t, const expoint<T>&  m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const expoint<T>&  t, const expoint<T>&  m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const expoint<T>&  t, const expoint<T>&  m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const expoint<T>&  t, const exptr<T>&    m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const expoint<T>&  t, const exptr<T>&    m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const expoint<T>&  t, const exptr<T>&    m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const expoint<T>&  t, const exptr<T>&    m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const expoint<T>&  t, const exptr<T>&    m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const expoint<T>&  t, const exptr<T>&    m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const expoint<T>&  t, const exvector<T>& m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const expoint<T>&  t, const exvector<T>& m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const expoint<T>&  t, const exvector<T>& m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const expoint<T>&  t, const exvector<T>& m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const expoint<T>&  t, const exvector<T>& m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const expoint<T>&  t, const exvector<T>& m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exptr<T>&    t, const exarray<T>&  m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exptr<T>&    t, const exarray<T>&  m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exptr<T>&    t, const exarray<T>&  m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exptr<T>&    t, const exarray<T>&  m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exptr<T>&    t, const exarray<T>&  m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exptr<T>&    t, const exarray<T>&  m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exptr<T>&    t, const expoint<T>&  m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exptr<T>&    t, const expoint<T>&  m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exptr<T>&    t, const expoint<T>&  m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exptr<T>&    t, const expoint<T>&  m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exptr<T>&    t, const expoint<T>&  m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exptr<T>&    t, const expoint<T>&  m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exptr<T>&    t, const exptr<T>&    m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exptr<T>&    t, const exptr<T>&    m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exptr<T>&    t, const exptr<T>&    m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exptr<T>&    t, const exptr<T>&    m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exptr<T>&    t, const exptr<T>&    m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exptr<T>&    t, const exptr<T>&    m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exptr<T>&    t, const exvector<T>& m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exptr<T>&    t, const exvector<T>& m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exptr<T>&    t, const exvector<T>& m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exptr<T>&    t, const exvector<T>& m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exptr<T>&    t, const exvector<T>& m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exptr<T>&    t, const exvector<T>& m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exvector<T>& t, const exarray<T>&  m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exvector<T>& t, const exarray<T>&  m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exvector<T>& t, const exarray<T>&  m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exvector<T>& t, const exarray<T>&  m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exvector<T>& t, const exarray<T>&  m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exvector<T>& t, const exarray<T>&  m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exvector<T>& t, const expoint<T>&  m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exvector<T>& t, const expoint<T>&  m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exvector<T>& t, const expoint<T>&  m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exvector<T>& t, const expoint<T>&  m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exvector<T>& t, const expoint<T>&  m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exvector<T>& t, const expoint<T>&  m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exvector<T>& t, const exptr<T>&    m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exvector<T>& t, const exptr<T>&    m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exvector<T>& t, const exptr<T>&    m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exvector<T>& t, const exptr<T>&    m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exvector<T>& t, const exptr<T>&    m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exvector<T>& t, const exptr<T>&    m) { return (t.base() >= m.base()); }

template <class T> inline int   operator == (const exvector<T>& t, const exvector<T>& m) { return (t.base() == m.base()); }
template <class T> inline int   operator != (const exvector<T>& t, const exvector<T>& m) { return (t.base() != m.base()); }
template <class T> inline int   operator <  (const exvector<T>& t, const exvector<T>& m) { return (t.base() <  m.base()); }
template <class T> inline int   operator >  (const exvector<T>& t, const exvector<T>& m) { return (t.base() >  m.base()); }
template <class T> inline int   operator <= (const exvector<T>& t, const exvector<T>& m) { return (t.base() <= m.base()); }
template <class T> inline int   operator >= (const exvector<T>& t, const exvector<T>& m) { return (t.base() >= m.base()); }

/* --------------------------------------------------------------
    Реализация операций вычитания.
-------------------------------------------------------------- */

template <class T> inline ptrdiff_t operator - (const exarray<T>&  t, const exarray<T>&  m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exarray<T>&  t, const expoint<T>&  m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exarray<T>&  t, const exptr<T>&    m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exarray<T>&  t, const exvector<T>& m) { return (t.base() - m.base()); }

template <class T> inline ptrdiff_t operator - (const expoint<T>&  t, const exarray<T>&  m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const expoint<T>&  t, const expoint<T>&  m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const expoint<T>&  t, const exptr<T>&    m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const expoint<T>&  t, const exvector<T>& m) { return (t.base() - m.base()); }

template <class T> inline ptrdiff_t operator - (const exptr<T>&    t, const exarray<T>&  m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exptr<T>&    t, const expoint<T>&  m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exptr<T>&    t, const exptr<T>&    m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exptr<T>&    t, const exvector<T>& m) { return (t.base() - m.base()); }

template <class T> inline ptrdiff_t operator - (const exvector<T>& t, const exarray<T>&  m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exvector<T>& t, const expoint<T>&  m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exvector<T>& t, const exptr<T>&    m) { return (t.base() - m.base()); }
template <class T> inline ptrdiff_t operator - (const exvector<T>& t, const exvector<T>& m) { return (t.base() - m.base()); }

/* --------------------------------------------------------------
    Шаблон стека массивов с переменными размерами.

    Шаблон описывает стек, откуда можно взять
    (get) массив exblock<T> и куда можно вернуть
    (put) массив exblock<T>, если он больше не
    нужен. Методы get и put могут применяться
    в конструкторах и деструкторах объектов,
    производных от шаблона exblock и работают
    быстрее штатные методы распределения памяти.
-------------------------------------------------------------- */

template <class T> class exblockstack
{
    exarray<exblock_t> stack;   /* Стек массивов exblock_t<T> */
    size_t n;                   /* Число массивов */
/*
    Для блокировки присваивания стеков объявляются приватные
    конструктор копирования и операция присваивания.
*/
    exblockstack                (const exblockstack<T>&);
    exblockstack<T>& operator = (const exblockstack<T>&);

public:
/*
    Конструктор создает пустой стек.
*/
    exblockstack()       { n = 0;    }  /* Конструктор */

    void get (exblock<T>&);             /* Извлечение массива */
    void put (exblock<T>&);             /* Занесение массива */
    size_t size() const { return n; }   /* Число массивов */

   ~exblockstack();                     /* Деструктор */
};
/*
    Метод get извлекает массив exblock<T> из стека,
    а если стек пуст, то создает пустой массив.
*/
template <class T> void exblockstack<T>::get (exblock<T>& array)
{
    if (n)                              /* Стек непуст: */
    {                                   /* извлечение массива */
        exblock<T>* p = (exblock<T>*) & (stack.item (--n));
        array.setbase_c (p->base()); p->setbase_c (NULL);
        array.setsize_c (p->size()); p->setsize_c (0);
    }
    else                                /* Стек пуст: */
    {                                   /* создание массива */
        array.setbase_c (NULL);
        array.setsize_c (0);
    }
}
/*
    Метод put заносит массив в стек.
*/
template <class T> void exblockstack<T>::put (exblock<T>& array)
{
    stack.expand (n);
    exblock<T>* p = (exblock<T>*) & (stack.item (n++));
    p->setbase_c (array.base()); array.setbase_c (NULL);
    p->setsize_c (array.size()); array.setsize_c (0);
}
/*
    Деструктор удаляет все находящиеся в стеке массивы.
*/
template <class T> exblockstack<T>::~exblockstack()
{
    while (n) ((exblock<T>*) & (stack.item (--n)))->clear();
}

#endif/*__cplusplus*/

#endif/*EXARRAY_H*/
