/* --------------------------------------------------------------
    ������������ ������� � ��������� ������� (������ 1.2c).

    http://www.imach.uran.ru/exarray

    Copyright 1998-2010, �.�.�������, ���� ��� PAH.
    All Rights Reserved.

    ��� ��������������� �� �������� �������� "public domain",
    �.�. ����������� ��������� ����������, ����������� �
    ��������������� ���� ��� ������� ���������� ���������
    � ���������� �������� � ���� ��������� ����������.

    ������ ����������� ����������� ��������������� "��� ����"
    ��� �����-���� ��������, ����� ��� ���������������. ��
    ����������� ��� �� ���� ����������� ����� � ����. �����
    �� ����� ����� ��������������� �� ������ ������, �����,
    ������ ��� ����� ������ ���� ������ ��� ����� ��������
    ������������� ������� ������������ �����������.
-------------------------------------------------------------- */
#ifndef EXARRAY_H
#define EXARRAY_H

#include <stddef.h>
#include <limits.h>

/* --------------------------------------------------------------
    ������� ������������� ��� ����������� �������� � ���������
    �������� � �������������� ����������� ��������.

    ������������ ������ �������� ��������� ���������������
    ���������� ������� ��� ������ ������� �� ������� �������.

    ������������ ��������� ������������ ��� ��������� ���������
    ������������� ������� � �������������� ����������� �������.

    ������������ ������ ������������ ������ � ���������
    ��������� � ��������� �� ��������� �������.

    ������������ ��������� ������������ ��� ��������� ��������
    (�������, ������������ � ������������) � ��������� ��������
    ��� ������� �������� �� ��������.

    ����������:
    exarray<���>  ���;      ������������ ������.
    expoint<���>  ���;      ������������ ���������.
    exvector<���> ���;      ������������ ������.
    exptr<���>    ���;      ������������ ���������.

    ����������� ������� �������� � ��� ��������� � �����������.
-------------------------------------------------------------- */

#ifdef  __cplusplus
template <class T> class exarray;
template <class T> class expoint;
template <class T> class exvector;
template <class T> class exptr;
#endif/*__cplusplus*/

/* ==============================================================
    ����� �������������.

1.  ��������������� ����� �� ��������� ���������, �.�. ���
    ����� ������� �������� ��� ������������, �� ���������������
    ��������� �++.

    ALLOW_EXARRAY_ADDSUB        - ��������� �������� + � -
            ��� ������������ �������� � ������������� ��������,
            � ���������� ���� �������� ������������ ���������.
            (����� + � - ���������� ������ ��� ����������).
            * �������� � ���������� ���������� ����������
              � Borland C++ 3.1.

    ALLOW_EXARRAY_SELECTION     - ��������� �������� ->.
    ALLOW_EXARRAY_ITERATORS     - ��������� �������� ->,
            � ����� ������ ��� ���������� �������� � ��������
            ���������� � ����������� ���������� STL.
            * �� �������������� � Borland C++ 3.1 � 4.5.
            * ����������� ����� ������������ ������� STL.

    ALLOW_EXARRAY_EXCEPTIONS    - ��������� ����� ����������
            ������� ��������� ���������� (���������, ����
            �������� ������ ���������� � ������������
            ��������� �������).
            * �� �������������� � Borland C++ 3.1.
            * � Visual C++ ��������� ���������� � ������������
              ��������� �������� � ���������� ��������
              ����������� ����������� (�� ������������
              ������� ��������� ���������).

2.  ����� �������������. �������� ��� �����, ���� ��� ����������
    �������� � ���������.

    ALLOW_EXARRAY_NEW_DELETE    - �������������� new[] � delete[]
            ��� �������� EXNEW � EXDELETE.
            * ������ ��������, ���� ���������� ��������� ���������
              �� ���������� C++ (__cplusplus >= 199711L), ����� ���
              Visual C++ 5.0 � ���� � ��� Borland C++ 4.5 � ����.

    ALLOW_EXARRAY_PLACEMENT     - ��������� placement delete.
            * ������ ��������, ���� ���������� ��������� ���������
              �� ���������� C++ (__cplusplus >= 199711L), ����� ���
              Visual C++ 6.0 � ����.

    ALLOW_EXARRAY_CHARTYPE      - ��� char �������
            �� signed/unsigned char.
            * ������ ��������, ���� ���������� ��������� ���������
              �� ���������� C++ (__cplusplus >= 199711L), ����� ���
              Visual C++ 6.0 � ����, Borland C++ 4.5 � ����.

    ALLOW_EXARRAY_SIZETYPE      - ��� size_t ������ ����
            ����������� ������������� �����, ������� long.
            * ������ �������� ��� ������������, ������������
              SSIZE_MAX ��� _WIN64 � _I64_MAX ������ ULONG_MAX,
              ��� Visual C++ � 64-������ ������.

    ALLOW_EXARRAY_PTRDIFFTYPE   - ��� ptrdiff_t ������ ����
            ����������� ������������� �����, ������� long,
            �� �� ������ size_t.
            * ������ �������� ��� ������������, ������������
              SSIZE_MAX ��� _WIN64 � _I64_MAX ������ ULONG_MAX,
              ��� Visual C++ � 64-������ ������.

    DISALLOW_EXARRAY_LONGTYPE   - ��������� ������ � ���������
            ������� �� �������� ���� long � unsigned long.

    DISALLOW_EXARRAY_INTTYPE   - ��������� ������ � ���������
            ������� �� �������� ���� int � unsigned int.

    DISALLOW_EXARRAY_SHORTTYPE   - ��������� ������ � ���������
            ������� �� �������� ���� short � unsigned short.

    ALLOW_EXARRAY_USING         - ������������ ��������� using
            ��� ������ ������������� ������-�������.
            * ������ ��������, ���� ���������� ��������� ���������
              �� ���������� C++ (__cplusplus >= 199711L), ����� ���
              Visual C++ 2005 � ����, GNU g++ 3.4 � ����.
            * �� �������������� � Borland C++ 3.1 � 4.5.

3.  ����� �����������.

    ALLOW_EXARRAY_SCHEDULING    - ����������� ���� ���
            �������������� ���������������� Pentium �
            Pentium MMX (�� �������������).
            * ���������������� ��� Borland C++ 3.1 � 4.5,
              � ���� ������� - ��� Borland C++ Builder.
            * ������ ��� Visual C++ ������� �� ����������,
              ��� Visual C++ 6.0 ������ ����������� �����
              �������� � ������ ������������� ������.

    NCHECKPTR                   - ��������� ���������� ��������.
            * ������� ��� ���������� � ������� NDEBUG � �.
            * Visual C++ ������ NDEBUG � ������ Release.
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
    �������������� ���������� �����.
-------------------------------------------------------------- */

#ifdef  ALLOW_EXARRAY_ITERATORS
#include <iterator>
#define ALLOW_EXARRAY_SELECTION
#endif/*ALLOW_EXARRAY_ITERATORS*/

/* --------------------------------------------------------------
    ������������� �����������.

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
    ������� ��� ���������� ���������� � ������.
    STDECL  - ����������� ���������� �
    EXDECL  - ������� ������� � ������������� ������ ����������
    FCDECL  - ������� ������� � ���������� �������
    INDECL  - force-inline ������� C++
    VDECL   - ������� ����������
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
    ������������� ������������ ������ ��� C.
============================================================== */

/* --------------------------------------------------------------
    ������ ���������� ����� ���� ��������������
    � ������ ���������� ������.
-------------------------------------------------------------- */

#ifndef EXALLOC_H_MAIN
#define EXALLOC_H_MAIN

/* --------------------------------------------------------------
    ������� ������ ���������, ��� "C", �����
    ������������� �������������� ����.
    ��� ��������� ���������� ��������� ������
    ����� ���������� �������� ������������.
-------------------------------------------------------------- */

#ifdef  __cplusplus
extern "C" {
#endif/*__cplusplus*/

/* --------------------------------------------------------------
    ���������, ����������� ������������� �������
    �� ������ ���������.
-------------------------------------------------------------- */

struct  exblock_t { void* e; size_t len; };

/* --------------------------------------------------------------
    ���������, ����������� ����� ������������ ��������
    �� ������ ���������.
-------------------------------------------------------------- */

struct  exblockstack_t { exblock_t stack; size_t n; };

/* --------------------------------------------------------------
    ������������ NULL-������.
-------------------------------------------------------------- */

extern struct  exblock_t VDECL exnull_c;

/* --------------------------------------------------------------
    ������� set_exalloc_handler() �������������
    ���������� ������ ������������� ������,
    ����������� set_new_handler().
-------------------------------------------------------------- */

extern  void (* set_exalloc_handler (void (*pvf) (void))) (void);

/* --------------------------------------------------------------
    ����������� ������� ������ � ��������
    o ������������� ������.

    exalloc_status.size_handler
        ��������� ��������������� �������, ����������
        ��� �������� ������. ������� ������ ���������
        ���������� ��� ���������� ������ ���������.

    exalloc_status.grow_handler
        ��������� ��������������� �������, ����������
        ��� �������������� ���������� ������� �������
        ��� NULL, ���� ����� ��������������� �������
        �� ���������. ����� ����������� ��� ����������
        ��������������� ���������� �������.

    exalloc_status.null_handler
        ��������� �������, ���������� ��� ���������
        �� NULL-���������. ������� ������ ���������
        ���������� ��� ���������� ������ ���������.

    exalloc_status.range_handler
        ��������� �������, ���������� ��� ������
        ������� �� ������� ������� � ��� ������, ����
        �� ������������� �������������� ����������
        ������� �������. ������� ������ ���������
        ���������� ��� ���������� ������ ���������.
-------------------------------------------------------------- */

struct  exalloc_status_t
{
    void (* VDECL size_handler)(void);
    void (* VDECL grow_handler)(void);
    void (* VDECL null_handler)(void);
    void (* VDECL range_handler)(void);
    char*  pMinAlloc;               /* ����������� ����� */
    char*  pMaxAlloc;               /* ������������ ����� */
    int    nBlocksAllocated;        /* ����� �������������� ������ */
    int    nBlocksFailed;           /* ����� ������ ������������� */
};
extern struct exalloc_status_t VDECL exalloc_status;

/* --------------------------------------------------------------
    ������� exmalloc() �������� ���� ������, ����������� ������.

    ������� exmrealloc() ��������, ��������, ����������� ���
    ����������� ���� ������, � ����� �������� �������� �������.

    ������ ����� �������� � ������. ��� ������������ ���������
    ����� ��� �������� ������ ���������� ���������� ������,
    ������������� �� exalloc_set_handler, ����� �����������
    exalloc_status.size_handler.

    ������� exfree() ����������� ���� ������.

    ������� exaligned_ ��������� ������� ������������.
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
    ������� exrange() ��������
    exalloc_status.null_handler  ��� p == NULL,
    exalloc_status.range_handler ��� p != NULL.
-------------------------------------------------------------- */

void    FCDECL  exrange (void* p);

/* --------------------------------------------------------------
    ��������� ������������� ������������ ������.

    EXSIZE_T_MAX        - ������������ ��������,
            ���������� � �������� ����������
            ������������� ���������� � ���������
            ������������ ��������� �����.
    EXARRAY_ALIGN       - ������������ ����� ������,
            ��������������� ��� ������������� �������.
            ���������� �������� - 1, 2, 4, 8, 16.
    EXCALCBLOCKSIZE_SYS - ������ ����������
            ��������� ����� ������, ���������
            �� ������������ ����������� (����
            ������ ����������, �� ����� �������
            ������� ������, �� �� ����� 16).
    EXCALCBLOCKSIZE_MOD - ��������� ������
            ����� ������ � ������ ����������
            ��������� ��� ������� �� ���������
            ������� excalcblocksize(), �������
            pow(64,n)+/-16 ����.
    EXCALCBLOCKSIZE1..5 - ��������� ������
            ����� ������ ��� ����� ����������
            ���������, ������������ �� ���������
            ������� excalcblocksize() � ���������
            ����������� ��������, ����������
            ��� �� ��������.
    EXCALCBLOCKSIZE_MAX - ������������ ������
            ����� ������ ��� ����� ����������
            ���������, ������� �����������
            ��������� ������������� ������
            (������������ ������� � ����������
            ����� ����������� ��������������
            �����������).
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
    ������� excalcblocksize() ������������
    ����������� ������ ����� ������ � ������,
    ������� ������ ���� �� ������ ����������.
    ���� ����������� ������ ������� �����,
    �� �������� EXSIZE_T_MAX.
-------------------------------------------------------------- */

size_t  FCDECL  excalcblocksize (size_t blocksize);

/* --------------------------------------------------------------
    ������������� ���������� � ���������
    ������������ ��������� �����.
      exmuladd (s,n,k)   = s * n + k.
      exmul    (s,n)     = s * n.
      exadd    (n,k)     = n + k.
    ��� ������������ �������� EXSIZE_T_MAX.
      exchkadd (n,k,max) = n + k, ���� <= max, ����� ������.
    ��� ��������� ������������ ���� ������� ������������
    ���������� �++ � ����������� ��������� �� ����� s � k.
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
    ���������� ������� ������ ����� ��������������
    � ������ �� ���������.
-------------------------------------------------------------- */

#ifdef  __cplusplus
}
#endif/*__cplusplus*/

/* --------------------------------------------------------------
    ����� EXALLOC_EXTRA_ITEMS(T) ������ ����� ��������������
    ������������ ��������� � ������� �������� ���� T.
    ����� ����� ��������� �������� �� 0 � ����.
    ������� �������� - 1.
-------------------------------------------------------------- */

#ifndef EXALLOC_EXTRA_ITEMS
#define EXALLOC_EXTRA_ITEMS(T)  (1)
#endif/*EXALLOC_EXTRA_ITEMS*/

/* --------------------------------------------------------------
    ����� EXALLOC() �������� ����������� ������
    ���� ������ ��� ������ ���������.

    ����� EXREALLOC() ��������, ��������,
    ����������� ��� ����������� ���� ������,
    � ����� �������� �������� �������.

    ������ ��������, ��� ����� ��������� n ���� �.
    ������������� � ������������ ����� ���������
    ���������� ������ ��� EXALLOC_EXTRA_ITEMS(T)
    ���������� ��������� �� ��������� ���������
    �������. ������������ �� ����������.

    ��� ������������ ��������� ����� ��� ��������
    ������ ���������� ���������� ������, �������������
    �� exalloc_set_handler, ����� �����������
    exalloc_status.size_handler.

    ����� EXFREE() ����������� ���� ������
    ����������� �� ����������.

    ����� EXALIGNED_ ��������� ������ ������������.
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
    ���������� ���������� �� ����� �������������
    � ������ ������.
-------------------------------------------------------------- */

#endif/*EXALLOC_H_MAIN*/

/* ==============================================================
    ������������� ������������ ������ ��� C � C++.
============================================================== */

/* --------------------------------------------------------------
    ����� EXNEW() �������� ���������� ���� ������
    � �������� ������������ �++.

    ������ ��������, ��� ����� ��������� n ���� �.
    ������������� � ������������ ����� ���������
    ���������� ������ ��� EXALLOC_EXTRA_ITEMS(T)
    ��������� �� ��������� ��������� �������.

    ��� ������������ ��������� ����� ���
    �������� ������ ���������� �������,
    ������������� �� set_exalloc_handler(),
    ���� �� ������� ���, �� ���������� abort().

    ����� EXDELETE() �������� ����������� C++
    � ����������� ���� ������.

    � ������ C ��� ����� ���������� EXALLOC � EXFREE.
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
    ������� �������� � ���������� ��� ��� �++.

    ������������ ������ �������� ����������� ������� ������,
    ��������� �� ���� ��������:
    1) �������� �������. ��� ������� �������� ����������
       ��������� ����������� �������  ������, ����� ����
       ���������� �����������. � ��������� �����������
       ��������� �� ������ � ������.
    2) ���������� �������, ����������� ��� ����������
       ���������� ��������� �������� ���� (�����������
       ����� ��������� �������� � EXALLOC_EXTRA_ITEMS).
       ���� ���������� ������� �������� ����������
       �������� �������� ����, �� ��� ����� ������ ���
       ��������� � ������� ��� �������� �������.
    3) ����������������� ������, ������ �������
       ������������ �������������.

    ������� ����������� ������� ����������� ��� ���������
    � ������� ��� �������� �������, �.�. ��������� �������
    �������������� ����������� ���������������� ������ -
    - ��������� �� �������, ������� ����� ���������.

    ��� ����� �������� ������ "������" ������������ ������,
    �.�. ������ ��� ���������, ����� ��������� ����������
    ������� ������ � ������� "�������" ��������� ���� �� ����.
============================================================== */

#ifdef  __cplusplus

/* --------------------------------------------------------------
    ��������������� ������ ��� ������ ������������� �
    ������������ ��� ������������� ������.
-------------------------------------------------------------- */

template <class T> class exreloc_c
{
    T value;
    exreloc_c                (const exreloc_c<T>&);
    exreloc_c<T>& operator = (const exreloc_c<T>&);
public:
    exreloc_c                () {}
/*
    Placement-��������� new � delete ��� ������ �������������
    � ������������ ��� ������������� ������.
*/
    void* operator new    (size_t, void* p)
                                    { return p;    }
#ifdef  ALLOW_EXARRAY_PLACEMENT
    void  operator delete (void*, void*)  { ; }
#endif/*ALLOW_EXARRAY_PLACEMENT*/
    void  operator delete (void*)         { ; }
};

/* --------------------------------------------------------------
    ������ ������� � ����������� ���������.

    ������ ����������� ��� ���������� ����������� �������.
    ����������� ����������������� �������� �������� exblock
    �� �������������.
-------------------------------------------------------------- */

template <class T> class exblock
{
protected:
    T*          e;      /* ������� ������ */
    size_t      len;    /* ����� ��������� */
/*
    ����������� ���������� ������ ��������
    sizeof(T) * EXALLOC_EXTRA_ITEMS(T) ����.
*/
    static T* stub();
/*
    ������������� ������ ��� ��������� �����
    ��������� � ��������� ������� ����� ������.
*/
    void  reallocate (size_t n, size_t blocksize);
/*
    ���������� ������ ���������, ���� ������
    ��������� �� ����������� ������� ������
    ��� ��������� �� ������� i.
*/
        T*  range ()            const;
        T*  range (size_t i)            { return range()-i; }
const   T*  range (size_t i)    const   { return range()-i; }
/*
    ������������� ������ ��� �������� �
    �������� �������� i >= len. ������ e.
*/
        T*  grow  (size_t i)    const;
/*
    ����������� � ���������� ����������, �.�.
    ����������� ����������������� ��������
    �������� exblock �� �������������.
    � ������������ ������������ ������ ������� ������
    e = stub() � len = 0, ����� ����, ��� �������������,
    ������� ���������� ����� ������������� ������.
    � ����������� ������� ������� ����� clear().
*/
    exblock ()  { /* e = stub(); len = 0; */    }
    ~exblock()  { /* clear(); */                }

private:
/*
    ��� ���������� ������������ ��������
    � �� �������� � ������� �� ��������
    ����������� ��������� �����������
    ����������� � �������� ������������.
    ������� exblock ����� ���������� � �������
    �� ������, ���� ����� ������������ ���������.
*/
    exblock                (const exblock<T>&);
    exblock<T>& operator = (const exblock<T>&);

public:
/*
    ������ ������������ ������� ����� ������
    �� ���������� ����� ��������� � �������.
    ��� ������������ ������ EXSIZE_T_MAX.
*/
    size_t min_blocksize (size_t n) const
    {
        return (exmuladd (
            sizeof(T), n,
            sizeof(T) * EXALLOC_EXTRA_ITEMS(T)));
    }
/*
    ������������� ������ ����� ��� ���������
    ����� ���������, ��� ��������������.
*/
    void  reallocate (size_t n)
    {
        reallocate (n, min_blocksize (n));
    }
/*
    ������ ����������� ���������� �����
    ��������� � ������� �� ������� �����
    ������, �� �������� min_blocksize (0).
*/
    size_t max_size (size_t blocksize) const
    {
        return (blocksize / sizeof(T) -
                EXALLOC_EXTRA_ITEMS(T));
    }
/*
    STL-������ ���������� ��������:
      size     () ����� ���������.
      capacity () ����� ���������, ��� ������� ������������ ������.
      max_size () ����������� ��������� ����� ���������.
      empty    () �������� size() == 0.
      resize  (n) ��������� ����� ���������.
*/
    size_t  size     () const   { return len;                           }
    size_t  capacity () const;
    size_t  max_size () const   { return max_size (EXCALCBLOCKSIZE_MAX -
                                                   EXARRAY_ALIGN);      }
    int     empty    () const   { return (size() == 0);                 }
    void    resize   (size_t n);
/*
    ��������� ������������� ������� ��������.
*/
    void    confine  (size_t i);
/*
    ������������� ������ ���������� ��������,
    ���������������� �� ������������������.

      adjust (i)  �������� ������ ������� � ���, ����� ��������
                  ������ ��� �������� � ��������� �������� ���
                  ������� �������� � ����� ������� ��������.

      expand (i)  ��������� ������ ������� � ���, ����� ��������
                  ������ ��� �������� � ��������� ��������.

      shrink (i)  ��������� ������ ������� � ���, ����� �������
                  �������� � �������� ����� ������ ����������.

    ������ expand() � shrink() �������� ����������� ����������
    �������������� ������ adjust(). ������ expand() � adjust()
    ����������� ������� � ������� �������� � ��������� ��������.
    ����� shrink() ��������� ������� � ��������� ��������, ����
    ������ ��� ���� ��� ��������, ����� ���� ������� ��������� ��
    �����������. ��� ������ ��������� ����������� �������������
    ������ ������ � ��� ������, ���� ��� ���������. �������� �����
    ��������� ������������ ������� �������������� ������ � �����
    ���� ������ ������������.
*/
protected:
    void  adjust_c (size_t i);               /* ������������������ adjust */
    void  adjust_c (size_t i, const T** p);  /* ������������������ adjust */
    void  shrink_c (size_t i);               /* ������������������ shrink */
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
    ���������� ������ ���������� ��������:

      checkadjust (i) ������ adjust()
      checkexpand (i) ������ expand()
      checkshrink (i) ������ shrink()
      checkindex  (i) �������� i < len

    ��� ������� ����� NCHECKPTR ������ ��� ������ ���������
    ����� ��������, � ����� checkindex() �� ������ ������.

    ��� ���������� ����� NCHECKPTR ���������� ������
    ������������ ������ ������� ����� ������ confine(),
    ���� ��� ��������� � ������ checkindex() ����� �����
    ��������� ������ ��������, �� ���������� �������,
    ����������� ::exalloc_status.range_handler.
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
    ������ � array ��� �������� index:
      item (i)      �������.
      base ()       ��������� �����o�� array.
    ������ �� ������� � ��������� �������������
    �� ��� ���, ���� ������ array �� ���������.
*/
      T&  item   (size_t i)             { return e[i];          }
const T&  item   (size_t i)     const   { return e[i];          }
      T*  base   ()                     { return (e);           }
const T*  base   ()             const   { return (e);           }
/*
    ������ � ������� � ��������� �������:
      at   (i)      �������.
    ��� ������ ������� �� ������� ������� ����������
    �������, ����������� ::exalloc_status.range_handler.
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
    ������ � ������� � ��������� �������
    � �������������� ����������� �������.
      access (i)    �������.
    ��� ������ ������� �� ������� �������
    ������ ������� ������������� �������������.
    ��� ���������� ������� ���������� ���������
    ������, ����� ���� ��� ���� ����� ���������
    ����������� ������������ �� ���������.

    ���� ���������� ������� ���������� ��-��
    ������������ ��������� ����� ��� ��������
    ������, �� ����������� ��������� ������ �
    ����������� ���������� ���������, ��� �
    ������� exmrealloc().
    ���������:
        ������������� ������ ����������������, ���
        ������� ������������� �����, ��� ��� ����
        ������� ����� T, ����� char � 16-������
        ������ �������� � ��������� ������.

    ��� �������� ������������� ������ ��������
    ������ �� ������� ������� ��� ���������� ���
    ������������ ��������. ������ �������������
    �� ��� ���, ���� ������ ������� �� ���������.
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
    ����������� ��� ���������� STL. ������ ����� ���������
    ��� ��������� � ������������ ������� �������.
*/
typedef       T         value_type;     /* ��� �������� */
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
    ��������� ��� ���������� STL.
      begin ()    ��������� �� ������ �������.
      end   ()    ��������� �� �������, ���������
                  �� ���������.
    ���� ��������� ���, �� begin() == end().
    ���������� � �������� end() ������.
    � ���� ��������� ��������� ������������ ���������,
    �������������� �������� ��������, � ���� ����������
    ����� NCHECKPTR - �� ������� ���������. ���������
    ������������� �� ��� ���, ���� ������ ������� ��
    ���������.
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
    STL-�������� ��� ��������:
      clear()   ������� �������.
      swap(m)   ������������ �������� ��� ����������� ���������.
*/
    void    clear     ()        { reallocate (0, 0);    }
    void    swap      (exblock<T>&);
/*
    ��������� ������:
      setbase_c (p) ������������ �������� �������.
      setsize_c (n) ������������ ����� ���������.
    �����������:
    1) ������������ �������� ��������� ��� �������
       ��������� ����� ��������� � 0.
    2) ������������ �������� �������, ������ ��� �������
       ���������� ��������� exaligned_malloc ���
       exaligned_mrealloc � ������������� EXARRAY_ALIGN,
       ��� ���� ������ ����� ������ � ������ blocksize �
       ����� ��������� n ������ �������� �������
       blocksize >= min_blocksize (n).
    ����������� ������, ���������� ���������� new �� �������,
    �.�. ��� �� ��������� ���������� �++ (��� ����� �
    �������������� ������������� new ���������� �������������
    ������ ����� ������).
*/
    void    setbase_c (T* p)        { e   = p;  }
    void    setsize_c (size_t n)    { len = n;  }
};
/*
    ���������� ����� stub() ������ ���������
    �� ����������� ���������� ������ ��������
    sizeof(T) * EXALLOC_EXTRA_ITEMS(T) ����.
*/
template <class T> T* exblock<T>::stub()
{
    static char _stub [sizeof(T) *
            (EXALLOC_EXTRA_ITEMS(T)? EXALLOC_EXTRA_ITEMS(T): 1)];
    return (T*)(_stub);
}
/*
    ���������� ����� reallocate (n, blocksize)
    ������������� ������ ����� ������ � ������
    blocksize � ����� ��������� n. ������
    ����� � ����� ��������� ������ ��������
    ������� blocksize >= min_blocksize (n),
    �� ����������� ������ �������� �������,
    ����� ��������� blocksize == n == 0.

    ��� ���������� ����� ��������� �����������
    ��������� ���������� ������ � ����������
    ������������ ���������. ��� ����������
    ����� ��������� ���������� �����������.
    ����� ������������ ���������� � ���� ���������:
    ������ ������������ ����������� �������
    ���������� � �����������, ������ - ���.
*/
template <class T> void exblock<T>::reallocate (size_t n, size_t blocksize)
{
/*
    if (blocksize < min_blocksize (n)) abort();
*/
    T* p = e;               /* ������� ����� e */

    if (p == 0)             /* ����������� ��������� �� */
    {                       /* ������������� 0-��������� */
        if (this == (exblock<T>*)&exnull_c)
            ::exrange (0);
    }
    else if (len == 0)
    {                       /* ���� ������ ������, */
        p = 0;              /* �� p ������� �������� */
    }
    else
    {
#ifdef ALLOW_EXARRAY_EXCEPTIONS
        for (;;)
        {
        try
        {
#endif
                            /* ����� ������������ ��� */
        while (len > n)     /* �������� ��� ���������� */
            { --len; delete (exreloc_c<T> *) (p + len); }
#ifdef ALLOW_EXARRAY_EXCEPTIONS
        break;
        }
        catch (...) { };
        }
#endif
    }
    ::exaligned_mrealloc (  /* ������������� ������ */
          (void**)&p,       /* ��������� ����� ������ */
          blocksize,        /* ����� ������ */
          EXARRAY_ALIGN,    /* ������������ */
          sizeof(T) * len,  /* ������� ��������� */
          sizeof(T) * (n + EXALLOC_EXTRA_ITEMS(T))
    );
    if ((e = p) == 0)       /* ����������� ������ e */
    {                       /* ��� ������� ������� e */
        e = stub();         /* �������� ��� stub */
    }
    else
    {                       /* ����� ������������� ��� */
        while (len < n)     /* �������� ��� ��������� */
            { new (p + len) exreloc_c<T>; ++len; }
    }
}
/*
    ���������� ����� range() ���������� ��� ����������
    ������ ��������� ��� ������ "�����������" ���������
    �� ����������� ������� ������.
*/
template <class T> T* exblock<T>::range() const
{
    exrange (e);
    return stub();
}
/*
    ���������� ����� grow (i) ����������
    ��� ��������� � �������� � i >= len
    ��� ��������������� ���������� �������.
    ����� �������������� ������ ���������� �������,
    ����������� ::exalloc_status.grow_handler.
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
    ����� capacity() ������ ����� ���������,
    ��� ������� ������������ ������.
    ��� ����� ������ �� ������ size().
*/
template <class T> size_t exblock<T>::capacity() const
{
    return (max_size (::excalcblocksize (min_blocksize (len))));
}
/*
    ����� resize (n) ������������� �������� �����
    ��������� � ������������ ������ ��� �����
    ���������, �� �������� ���������.
*/
template <class T> void exblock<T>::resize  (size_t n)
{
    reallocate (n, ::excalcblocksize (min_blocksize (n)));
}
/*
    ����� confine (i) ������������� ������������ ������
    ��������, ������ ��������� � ������������ ������ ���
    ����� ���������, �� �������� ���������.
*/
template <class T> void exblock<T>::confine (size_t i)
{
    reallocate (i + 1, ::excalcblocksize (
        exmuladd (sizeof(T), i,
                  sizeof(T) * (1 + EXALLOC_EXTRA_ITEMS(T)))));
}
/*
    ����� adjust_c (i) ������������� ������������ ������
    ��������, �� ������� ���������.
*/
template <class T> void exblock<T>::adjust_c (size_t i)
{
    size_t blocksize = ::excalcblocksize (
        exmuladd (sizeof(T), i,
                  sizeof(T) * (1 + EXALLOC_EXTRA_ITEMS(T))));
    reallocate (max_size (blocksize), blocksize);
}
/*
    ����������� ������ adjust_c (i), �����������
    ��������������� ��������� �� ������� �������
    ��� ����������� �������.
    ��������� �������������� ������ � ��� ������,
    ���� �� ��������� � �������� �������.
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
    ����� shrink_c (i) ��������� ������������ ������ ��������
    �� �������� �� ������ ���������.
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
    ������������ �������� ��� ����������� ���������.
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
    ������ ������� � ����������� ���������, � �������� �����
    ���������� ����� ������������ ��������� � ������������
    ��������������� ���������� �������.

    ������ ����������� ��� ���������� ����������� �������.
    ����������� ����������������� �������� �������� explace
    �� �������������.
-------------------------------------------------------------- */

template <class T> class explace: public exblock<T>
{
protected:
/*
    ����������� � ���������� ����������, �.�.
    ����������� ����������������� ��������
    �������� explace �� �������������.
    � ������������ ������������ ������ ������� ������
    e = stub() � len = 0, ����� ����, ��� �������������,
    ������� ���������� ����� ������������� ������.
    � ����������� ������� ������� ����� clear().
*/
    explace ()  { /* e = stub(); len = 0; */    }
    ~explace()  { /* clear(); */                }

private:
/*
    ��� ���������� ������������ ��������
    � �� �������� � ������� �� ��������
    ����������� ��������� �����������
    ����������� � �������� ������������.
    ������� explace ����� ���������� � �������
    �� ������, ���� ����� ������������
    ��� ������������ ���������.
*/
    explace                (const explace<T>&);
    explace<T>& operator = (const explace<T>&);
};

/* --------------------------------------------------------------
    ������ ������������� ������� � ��������� ��������.
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
    ��� ���������� ������������ ������������
    �������� � �� �������� � ������� ��
    �������� ����������� ��������� �����������
    ����������� � �������� ������������.
    ������������ ������� ����� ���������� �
    ������� �� ������, ����� ������������
    ���������, � ����� ����� ��������� const T*.
*/
    exvector                (const exvector<T>&);
    exvector<T>& operator = (const exvector<T>&);

public:
/*
    ����������� ������������� �������.
    ��������� e ������ ���� ������� �� 0,
    �� e ������������� ���������
    �� ����������� ������ ���� T.
*/
    exvector () { e = stub(); len = 0; }
/*
    ����������� ������������� �������
    � �������� ��������� ������ ���������.
*/
/*explicit*/ exvector (size_t n);
/*
    ���������� ��������� ������������ ������.
*/
    ~exvector() { clear(); }
/*
    �������������� �������������� � const T*
    ����������� ��� �������� �������� �������
    � ������� ����� �������� const T* � ���
    ���������� ���������. ���������� ��������
    ������������� ������� - ������.
*/
    operator const T*()   const { return base();        }
/*
    ������ � ������� � ��������� �������.
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
    �������������� ��������.
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
    ���������� �������������� ��������.
    ���� ��� �� �������, �� ����� �����������
    �������������� � const T* � �������� ���
    �����������.
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
    ���������� �������� ��� ������ ����� �������� �����������.
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
    ����������� ������������� �������
    � �������� ��������� ������ ���������.
    ���������� ��������� ����������� �������
    ���������� ��� ��������� ������ ���
    ��������������� �������� ������� - � ����
    ������ �������� ������ ������������ ������,
    ��� �������� �� ��������� ����������.
*/
template <class T> exvector<T>::exvector (size_t n)
{
/*
    �������� ������� ������� � ������� �������.
*/
    exvector<T> tmp;
    e = tmp.e; len = tmp.len;
/*
    ��������� ������ � ��������������� ��������
    ������� � ������� �������. ���� ����������
    ����������, �� ������� �������� �������
    ����� ������� ������������ �������� �������.
*/
    tmp.resize (n);
/*
    ������ ������� ������� � ������� � ��������������
    �������.
*/
    swap (tmp);
}

/* --------------------------------------------------------------
    ������ ������������� ������� �
    �������������� ������ ���������.
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
    ��� ���������� ������������ ������������
    �������� � �� �������� � ������� ��
    �������� ����������� ��������� �����������
    ����������� � �������� ������������.
    ������������ ������� ����� ���������� �
    ������� �� ������, ����� ������������
    ��� ������������ ���������, � ����� �����
    ��������� const T*.
*/
    exarray                (const exarray<T>&);
    exarray<T>& operator = (const exarray<T>&);

public:
/*
    ����������� ������������� �������.
    ��������� e ������ ���� ������� �� 0, �� e �������������
    ��������� �� ����������� ������ ���� T.
*/
    exarray ()  { e = stub(); len = 0; }
/*
    ����������� ������������� �������
    � �������� ��������� ������ ���������.
*/
/*explicit*/ exarray (size_t n);
/*
    ���������� ��������� ������������ ������.
*/
    ~exarray()  { clear(); }
/*
    �������������� �������������� � const T*
    ����������� ��� �������� �������� �������
    � ������� ����� �������� const T* � ���
    ���������� ���������. ���������� ��������
    ������������� ������� - ������.
*/
    operator const T*()   const { return base();        }
/*
    ������ � ������� � ��������� �������
    � �������������� ����������� �������.
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
    �������������� ��������.
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
    ���������� �������������� ��������.
    ���� ��� �� �������, �� ����� �����������
    �������������� � const T* � �������� ���
    �����������.
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
    ���������� �������� ��� ������ ����� �������� �����������.
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
    ����������� ������������� �������
    � �������� ��������� ������ ���������.
    ���������� ��������� ����������� �������
    ���������� ��� ��������� ������ ���
    ��������������� �������� ������� - � ����
    ������ �������� ������ ������������ ������,
    ��� �������� �� ��������� ����������.
*/
template <class T> exarray<T>::exarray (size_t n)
{
/*
    �������� ������� ������� � ������� �������.
*/
    exarray<T> tmp;
    e = tmp.e; len = tmp.len;
/*
    ��������� ������ � ��������������� ��������
    ������� � ������� �������. ���� ����������
    ����������, �� ������� �������� �������
    ����� ������� ������������ �������� �������.
*/
    tmp.resize (n);
/*
    ������ ������� ������� � ������� �
    �������������� �������.
*/
    swap (tmp);
}

/* --------------------------------------------------------------
    ������ ������������� ���������.

    ������������ ��������� �������� �������� ������������
    ������ ������������� �������, ������� ����� ��������
    �� ������� �������.  �������� ����� ���� �������������,
    � ���� ������ ��� ������������ �������� ��������������
    �������� � ��� �� �������� �����.
-------------------------------------------------------------- */

template <class T> class expoint
{
protected:
    explace<T>* a;  /* ������������ ������ */
    size_t      k;  /* �������� � ������� */
                    /* ��������� �������� */
    size_t  lf  (size_t n)    const
    {
        if (n < (size_t)k) n = k; return (n - k);
    }

public:
/*
    ����������� ������������� ��������� � ��������� ��������� 0.
    ���� ��������� �� �������� ������������ ������, ��
    ��� ������� ��������� �� ��������� ���������� �������,
    ����������� ::exalloc_status.null_handler.
*/
    expoint ()          { a = (explace<T>*)&exnull_c; k = 0;   }
/*
    ����������� ��� ������������ ��������� ��������.
    � ������� �� �������� ���������, ������������� ���������
    ����� ��� ������ �������������� ���� ��������� ��� �������,
    ��� � ��������� �������� ��������. ���������� �� ���������
    �������� ��� ������� ��������� �� ��������� ���������� �������,
    ����������� ::exalloc_status.null_handler.
*/
    expoint (size_t i)  { a = (explace<T>*)&exnull_c; k = i;   }
/*
    ����������� ��� ������������ ������������� �������.
    ����� ������������ ���������� ��������� ��������� �
    ��������� ������� � �������������� ����������� ��� �������.
*/
    expoint (const explace<T>&m, size_t i=0) { a = (explace<T>*)&m; k = i; }
/*
    ����������� ����������� � ���������� �������� �� ���������.
    �������� ������������ �������� ����� �������, �����
    ��������� ������������ ����������� ����������.
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
    ������ � ������������� �������.
*/
      explace<T>& place()               { return *a; }
const explace<T>& place()       const   { return *a; }
/*
    ������������� ������ ����� ��� ��������� ����� ���������.
*/
    void  reallocate (size_t n)         { a->reallocate(exadd(n,k));}
/*
    STL-������ ���������� ��������.
*/
    size_t  size     ()         const   { return lf(a->size());     }
    size_t  capacity ()         const   { return lf(a->capacity()); }
    size_t  max_size ()         const   { return lf(a->max_size()); }
    int     empty    ()         const   { return (size() == 0);     }
    void    resize   (size_t n)         { a->resize (exadd(n,k));   }
/*
    ��������� ������������� �������.
*/
    void    confine  (size_t i)         { a->confine (exadd(i,k));  }
/*
    ������������� ������ ���������� ��������.
*/
    void    adjust   (size_t i)         { a->adjust (exadd(i,k));   }
    void    adjust   (size_t i, const T** p)
                                        { a->adjust (exadd(i,k),p); }
    void    expand   (size_t i) const   { a->expand (exadd(i,k));   }
    void    expand   (size_t i, const T** p)
                                const   { a->expand (exadd(i,k),p); }
    void    shrink   (size_t i)         { a->shrink (exadd(i,k));   }
/*
    ������ � ������� ��� �������� �������.
    ������ �� ������� � ��������� �������������
    �� ��� ���, ���� ������ ������� �� ���������.
*/
      T&  item   (size_t i)             { return *(a->base()+k+i);  }
const T&  item   (size_t i)     const   { return *(a->base()+k+i);  }
      T*  base   ()                     { return  (a->base()+k);    }
const T*  base   ()             const   { return  (a->base()+k);    }
/*
    ������ � ������� � ��������� �������.
    ��� ������ ������� �� ������� ������� ����������
    �������, ����������� ::exalloc_status.range_handler.
*/
INDECL       T&  at     (size_t i)      { return (a->at (k+i));     }
INDECL const T&  at     (size_t i) const{ return (
                                 ((const explace<T>*)a)->at (k+i)); }
/*
    ������ � ������� � ��������� �������
    � �������������� ����������� �������.
    ��� ������ ������� �� ������� �������
    ������ ������� ������������� �������������.
    ��� ���������� ������� ���������� ���������
    ������, ����� ���� ��� ���� ����� ���������
    ����������� ������������ �� ���������.
    ��� �������� ������������� ������ ��������
    ������ �� ������� ������� ��� ���������� ���
    ������������ ��������. ������ �������������
    �� ��� ���, ���� ������ ������� �� ���������.
*/
INDECL       T&  access (size_t i)      { return (a->access (k+i));     }
INDECL const T&  access (size_t i) const{ return (
                                 ((const explace<T>*)a)->access (k+i)); }

#ifdef  ALLOW_EXARRAY_ITERATORS
/*
    ����������� ��� ���������� STL.
    ������������ ��������� ����� ��������� ���
    ��������� � ������������ ������� ������� � ���
    ����������� �������� � ������������ ��������.
*/
typedef std::random_access_iterator_tag iterator_category;
                                        /* ��������� ��������� */
typedef       T         value_type;     /* ��� �������� */
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
    ��������� ��� ���������� STL.
      begin ()    ��������� �� ������� �������.
      end   ()    ��������� �� �������, ���������
                  �� ���������.
    ���� ������������ ��������� ��������� ��
    ��������� �������, �� begin() == end().
    ���������� � �������� *end() ������.
    � ���� ��������� ��������� ������������ ���������,
    �������������� �������� ��������, � ���� ����������
    ����� NCHECKPTR - �� ������� ���������. ���������
    ������������� �� ��� ���, ���� ������ ������� ��
    ���������.
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
    �������������� �������������� � const T* �����������
    ��� �������� �������� ������� � ������� ����� ��������
    const T* � ��� ���������� ���������. ���� ���������
    �������� ������������ ������ ��� ��������� ��������
    ��������, �� ���������� �������� ��������� - ������,
    ����� - ����. ���� ������ ������� �� ������� �������,
    �� ���������� ������� �� ����������.
*/
    operator const T*()   const { return base();        }
/*
    ������ � ������� � ��������� �������
    � �������������� ����������� �������.
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
    �������������� ��������.
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
    ���������� �������� ��� ������ ����� �������� �����������.
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
    ������ ������������� ��������� � ��������� ��������.

    ������������ ��������� �������� ����������
    � ������ � ����� �������, � ����� ��������
    ������������ ������ �������, ������� �����
    �������� �� ������� �������. �������� �����
    ���� �������������, � ���� ������ ���
    ������������ �������� ��������������
    �������� � ��� �� �������� �����.
-------------------------------------------------------------- */

template <class T> class exptr
{
protected:
    T*      e;      /* ������� ������ */
    size_t  len;    /* ����� ��������� */
    size_t  k;      /* �������� � ������� */
    T*      x;      /* ��������� ��� ����������� ������� (����� NULL) */
    size_t  lf  (size_t n)      const
    {
        if (n < (size_t)k) n = k; return (n - k);
    }
/*
    ����������� 1:1.
*/
    exptr (const T* me, size_t mlen, size_t mk, const T* mx)
            { e = (T*)me; len = mlen; k = mk; x = (T*)mx;       }
/*
    ���������� ������ ���������, ���� ������
    ��������� �� ����������� ������� ������
    ��� ��������� �� ������� i >= len.
*/
      T*  range  ()             const;
      T*  range  (size_t i)             { return range()-i;     }
const T*  range  (size_t i)     const   { return range()-i;     }

public:
/*
    �������� ������� � ������ �������� ���������.
*/
protected:
      T*  check_ (size_t n)     const;
public:
      T*  check  (size_t n)             { return check_(n);     }
const T*  check  (size_t n)     const   { return check_(n);     }
/*
    �������� ����������� �������� 0..i � ������ �������� ���������.
*/
protected:
      T*  index_ (size_t i)     const;
public:
      T*  index  (size_t i)             { return index_(i);     }
const T*  index  (size_t i)     const   { return index_(i);     }
/*
    ����������� ������������� NULL-���������.
    ���� ��������� �� �������� ������, �� ��� ����������
    �������� - ����, � ��� ������� ��������� �� ���������
    ���������� �������, �����������
    ::exalloc_status.null_handler.
*/
    exptr ()          { e = NULL; len = 0; k = 0; x = NULL;     }
/*
    ����������� ��� ������������ �������� ���������.
    ���� �������� ��������� �������, �� ��� �������
    ��������� �� ������������� ��������� ����������
    �������, ����������� ::exalloc_status.null_handler.
    ���� �������� ��������� ���������, �� �����
    ������������ ���������� ��������� ��������� ��
    ������� 0, � ��� ��������� �������� ����������
    �������, ����������� ::exalloc_status.range_handler.
    ���������� �������� ��������� ��������� �
    ���������� ��������� ������������ ���������.
*/
    exptr (const T* p)
            { e = (T*)p; len = (e != NULL); k = 0; x = NULL;    }
/*
    ����������� ��� ������������ �������� �������.
    ���� �������� ��������� �������, �� ��� �������
    ��������� �� ������������� ��������� ����������
    �������, ����������� ::exalloc_status.null_handler.
    ���� �������� ��������� ���������, �� �����
    ������������ ���������� ��������� ��������� �
    ��������� ������� � ��������� ��������.
    ���������� �������� ��������� ��������� �
    ���������� ��������� ������������ ���������.
*/
    exptr (const T* m, size_t n, size_t i = 0)
            { if ((e = (T*)m) == NULL) n = 0; len = n; k = i; x = NULL; }
/*
    ������������ ��� ������������ ������������� �������
    � ���������. ������ ������� �� ������ ��������.
    ��� ��������� �� ��������� �������, ����������
    �������, ����������� ::exalloc_status.range_handler.
*/
    exptr (const exblock<T>&m, size_t i = 0)
            { e = (T*)m.base(); len = m.size(); k = i; x = NULL;    }
    exptr (const expoint<T>&m, size_t i = 0)
            { e = (T*)m.base(); len = m.size(); k = i; x = NULL;    }
/*
    ����������� ��� ��������� ���������� � ��������� �������.
*/
    exptr (const exptr<T>&m, size_t n, size_t i = 0)
            { e = (T*)m.check(n); len = n; k = i; x = m.x;          }
/*
    ����������� ����������� � ���������� �������� �� ���������.
    �������� ������������ �������� ����� �������, �����
    ��������� ������������ ����������� ����������.
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
    ������ ������������� � ������������ ������
    � ������� ������������� � ������������.
*/
    exptr<T>& allocate (size_t n)
        { e = EXNEW (T, n); len = n; return *this; }

    void deallocate ()
        { EXDELETE (e); e = NULL; len = 0; }
/*
    ������ ������������� � ������������ ������
    ��� ������ ������������� � ������������.
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
    ������ � ������� ��� �������� �������.
*/
      T&  item   (size_t i)             { return *(e+k+i);      }
const T&  item   (size_t i)     const   { return *(e+k+i);      }
      T*  base   ()                     { return  (e+k);        }
const T*  base   ()             const   { return  (e+k);        }
/*
    ������ � ������� � ��������� �������.
    ��� ������ ������� �� ������� ������� ����������
    �������, ����������� ::exalloc_status.range_handler.
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
    ����������� ��� ���������� STL.
    ������������ ��������� ����� ��������� ���
    ��������� � ������������ ������� ������� � ���
    ����������� �������� � ������������ ��������.
*/
typedef std::random_access_iterator_tag iterator_category;
                                        /* ��������� ��������� */
typedef       T         value_type;     /* ��� �������� */
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
    ��������� ��� ���������� STL.
      begin ()    ��������� �� ������� �������.
      end   ()    ��������� �� �������, ���������
                  �� ���������.
    ���� ������������ ��������� ��������� ��
    ��������� �������, �� begin() == end().
    ���������� � �������� *end() ������.
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
    �������������� �������������� � const T* �����������
    ��� �������� �������� ������� � ������� ����� ��������
    const T* � ��� ���������� ���������. ���� ���������
    �������� ������ ��� ��������� �������� ��������, ��
    ���������� �������� ��������� - ������, ����� - ����.
*/
    operator const T*()     const { return base();      }
/*
    ������ � ������� � ��������� �������.
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
    �������������� ��������.
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
    ���������� �������� ��� ������ ����� �������� �����������.
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
    ���������� ����� range() ���������� ��� ����������
    ������ ��������� ��� ������ "�����������" ���������
    �� ����������� ������� ������.
*/
template <class T> T* exptr<T>::range() const
{
    exrange (e);
    static char _stub [sizeof(T)];
    return (T*)(_stub);
}
/*
    �������� ������� � ������ �������� ���������.
*/
template <class T> T* exptr<T>::check_ (size_t n) const
{
    if ((len <= (size_t)k || len - (size_t)k < n) && n > 0) range();
    return (e + k);
}
/*
    �������� ����������� �������� 0..i � ������ �������� ���������.
*/
template <class T> T* exptr<T>::index_ (size_t i) const
{
    if (len <= (size_t)k || len - (size_t)k <= i) range();
    return (e + k);
}

/* --------------------------------------------------------------
    ���������� �������� ���������.
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
    ���������� �������� ���������.
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
    ������ ����� �������� � ����������� ���������.

    ������ ��������� ����, ������ ����� �����
    (get) ������ exblock<T> � ���� ����� �������
    (put) ������ exblock<T>, ���� �� ������ ��
    �����. ������ get � put ����� �����������
    � ������������� � ������������ ��������,
    ����������� �� ������� exblock � ��������
    ������� ������� ������ ������������� ������.
-------------------------------------------------------------- */

template <class T> class exblockstack
{
    exarray<exblock_t> stack;   /* ���� �������� exblock_t<T> */
    size_t n;                   /* ����� �������� */
/*
    ��� ���������� ������������ ������ ����������� ���������
    ����������� ����������� � �������� ������������.
*/
    exblockstack                (const exblockstack<T>&);
    exblockstack<T>& operator = (const exblockstack<T>&);

public:
/*
    ����������� ������� ������ ����.
*/
    exblockstack()       { n = 0;    }  /* ����������� */

    void get (exblock<T>&);             /* ���������� ������� */
    void put (exblock<T>&);             /* ��������� ������� */
    size_t size() const { return n; }   /* ����� �������� */

   ~exblockstack();                     /* ���������� */
};
/*
    ����� get ��������� ������ exblock<T> �� �����,
    � ���� ���� ����, �� ������� ������ ������.
*/
template <class T> void exblockstack<T>::get (exblock<T>& array)
{
    if (n)                              /* ���� ������: */
    {                                   /* ���������� ������� */
        exblock<T>* p = (exblock<T>*) & (stack.item (--n));
        array.setbase_c (p->base()); p->setbase_c (NULL);
        array.setsize_c (p->size()); p->setsize_c (0);
    }
    else                                /* ���� ����: */
    {                                   /* �������� ������� */
        array.setbase_c (NULL);
        array.setsize_c (0);
    }
}
/*
    ����� put ������� ������ � ����.
*/
template <class T> void exblockstack<T>::put (exblock<T>& array)
{
    stack.expand (n);
    exblock<T>* p = (exblock<T>*) & (stack.item (n++));
    p->setbase_c (array.base()); array.setbase_c (NULL);
    p->setsize_c (array.size()); array.setsize_c (0);
}
/*
    ���������� ������� ��� ����������� � ����� �������.
*/
template <class T> exblockstack<T>::~exblockstack()
{
    while (n) ((exblock<T>*) & (stack.item (--n)))->clear();
}

#endif/*__cplusplus*/

#endif/*EXARRAY_H*/
