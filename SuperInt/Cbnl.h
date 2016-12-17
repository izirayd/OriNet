/* --------------------------------------------------------------
    ����� ����� � �������������� ��������� ������ (������ 2.0).
    ������� ��� ����������� �����������.

    http://www.imach.uran.ru/cbignum

    Copyright 1999-2010, �.�.�������, ���� ��� PAH.
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
#ifndef _CBNL_H
#define _CBNL_H

/* --------------------------------------------------------------
    ��� CBNL.
-------------------------------------------------------------- */

/*
    Visual C++ (64-bit).
*/
#ifndef CBNL
#ifdef  _MSC_VER
#ifdef  _WIN64
#define CBNL __int64
#define CBNL_MIN    _I64_MIN
#define CBNL_MAX    _I64_MAX
#define UCBNL_MAX   _UI64_MAX
#include "Cbnl64.inl"
#endif/*_WIN64*/
#endif/*_MSC_VER*/
#endif/*CBNL*/

/*
    Other.
*/
#ifndef CBNL
#define CBNL long
#define CBNL_MIN    LONG_MIN
#define CBNL_MAX    LONG_MAX
#define UCBNL_MAX   ULONG_MAX
#include "Cbnl.inl"
#endif/*CBNL*/

/* --------------------------------------------------------------
    ���������� � ������� ��� ������� �������.
-------------------------------------------------------------- */

/*
    Visual C++ (32-bit).
*/
#ifndef _CBNL_C
#ifdef  _MSC_VER
#if     _MSC_VER >= 1200
#ifdef  _M_IX86
#if 0
#define _CBNL_C     __fastcall
#endif
#endif/*_M_IX86*/
#endif/*_MSC_VER*/
#endif/*_MSC_VER*/
#endif/*_CBNL_C*/

/*
    Other.
*/
#ifndef _CBNL_C
#define _CBNL_C
#endif/*_CBNL_C*/

#endif/*_CBNL_H*/
