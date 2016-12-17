/* --------------------------------------------------------------
    ����� ����� � �������������� ��������� ������ (������ 2.0).
    ��������� ����-�����.

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
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DISALLOW_EXARRAY_SHORTTYPE
#include "Cbignum.h"
#include "Cbignums.h"
#define BITS    (CHAR_BIT * sizeof (CBNL))

#ifdef  _MSC_VER
#pragma auto_inline (off)
#endif/*_MSC_VER*/

#ifndef _CBIGNUM_MT
#define _CBIGS  static
#else //_CBIGNUM_MT
#define _CBIGS
#endif//_CBIGNUM_MT

//================================================
//      �������� ���������� �����.
//================================================

//      �������������� ����� ���������� �� ������ ��������� ������:
//      ios::skipws     ���������� ��������� �������
//      ios::dec        ����� �� ��������� 10
//      ios::oct        ����� �� ��������� 8
//      ios::hex        ����� �� ��������� 16

_CIOSTD istream& operator >> (_CIOSTD istream& is, cBigNumber& b)
{
  assert (&is);
  assert (&b);
  long flags = is.flags();              // �����.
  unsigned radix = (                    // ���������.
         (flags & _CIOSTD ios::dec)? 10:
         (flags & _CIOSTD ios::oct)? 8:
         (flags & _CIOSTD ios::hex)? 16: 0);

//      ������� ���������� ��������, ���� ��� �������������
//      ������ ios::skipws � ���� ������� �������.

  if (flags & _CIOSTD ios::skipws) is >> _CIOSTD ws;
  int c = is.get();                     // ������� ������.

//      ������������� �����.

  int sign = 0;                         // ���� �����.
  switch (c)
  {
    case '-': sign = 1;
    case '+': c = is.get();
  }
  if (radix == 0)                       // ������������� ���������.
  {
    radix = 10;
    if (c == '0')
    {
      c = is.get(); radix = 8;
      if (c == 'X' || c == 'x') { c = is.get(); radix = 16; }
    }
  }

//      ���� ����.

  cBigNumber::SqrTab (radix);           // ���������� cBigNumber::chartonum.
  _CBIGS exarray<char> str_;            // ������ � �������.
  size_t width = 0;                     // ����� ���� � �������.
  for (;;)
  {
    if ((size_t)cBigNumber::chartonum [(unsigned char)c] >= radix)
      break;
    str_ [width++] = (char)c;
    c = is.get();                       // ��������� ������.
  }
  if (c != EOF) is.putback ((char)c);   // ���������� �������������� ������.

  str_ [width] = 0;                     // ����� ������.
  EXDEBUG (str_.checkindex (width));

//      ������������� �����.

  b.setunsign (EXPTRTYPE(str_), radix, width);

//      ���������� �����.

  if (sign) b.neg();

  return is;
}

//================================================
//      �������� ���������� ������.
//================================================

//      �������������� ������ ���������� �� ������ ��������� ������:
//      ios::width()    ������ ����.
//      ios::fill()     ������-�����������.
//      ios::left       ������������ �����.
//      ios::right      ������������ ������.
//      ios::internal   ���� � ��������� �����, ����� ������.
//      ios::dec        �������� ����� �� ��������� 10 (���������).
//      ios::oct        �������� ����� �� ��������� 8.
//      ios::hex        �������� ����� �� ��������� 16.
//      ios::showbase   �������� ������� ��������� (0 ��� 0x).
//      ios::showpos    �������� ���� ��� ������������� �����.
//      ios::uppercase  ��������� ����� ��� ����� � ���������� > 10.

_CIOSTD ostream& operator << (_CIOSTD ostream& os, const cBigNumber& b)
{
  assert (&os);
  assert (&b);
  long fill  = 0;
  long flags = (&os? os.flags(): 0);
  unsigned radix = ((flags & _CIOSTD ios::hex)? 16:
                    (flags & _CIOSTD ios::oct)? 8: 10);
  unsigned width = 0;

  if (flags & _CIOSTD ios::internal)
  {
    width = (unsigned) os.width();
    fill  = (long)     os.fill();
  }
#ifdef  _CBIGNUM_UNSIGN_OCT_HEX
  if (radix != 10) fill |= cBigNumber_unsign;
#endif//_CBIGNUM_UNSIGN_OCT_HEX
  if (flags & _CIOSTD ios::uppercase) fill |= cBigNumber_uppercase;
  if (flags & _CIOSTD ios::showbase)  fill |= cBigNumber_showbase;
  if (flags & _CIOSTD ios::showpos)   fill |= cBigNumber_showpos;

  _CBIGS cBigString str;
  os << b.toa (str, radix, width, fill);
  return os;
}
