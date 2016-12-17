/* --------------------------------------------------------------
    ����� ����� � �������������� ��������� ������ (������ 2.0).

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
#define DISALLOW_EXARRAY_SHORTTYPE
#include "Cbignum.h"
#include "Cbignumf.h"
#define BITS    (CHAR_BIT * sizeof (CBNL))
static  int     log2_BITS = cLongExactLog2 (BITS);

#ifdef  _MSC_VER
#pragma auto_inline (off)
#endif/*_MSC_VER*/

#ifndef _CBIGNUM_MT
#define _CBIGS  static
#else //_CBIGNUM_MT
#define _CBIGS
#endif//_CBIGNUM_MT

//================================================
//      ��������� ������� � ��������� ������ ������.
//================================================

//      ��������� ������� �������� ����������� ��� ����, ����� ���������
//      ��������� ������� �� �������� ��������� �������� �� ����� ����������
//      �������, ����������� � ����� ������ ������������������.
//      ��������� ������� �������� ���������� �� ��������� � �������
//      ������ ���������, ��� ��� ����������������� �������� ������
//      thread_local �������� ��������� C++ 0x.
//      ������, ������� ��� ��������� �������, �� �������������.
//
//      ��� ��������� �������� ����������� ����������� ��������������
//      ���� ��� �������������, ����� ��������� ����������� ������������,
//      � ����� ��������� ��������� (�� ������������) ������� ��� ��������
//      ����������� ��������.
//
//      �������, ��� ������� �� ������������ ����������� ������� ������
//      ���������� �������� ���� � Borland C++ 4.5 � Builder 1.0,

#ifdef  EXTHREAD_LOCAL
exblockstack_t EXTHREAD_LOCAL cBigTemp_stack;   // ���� �������������� �����.
exblockstack<long>            cBigTemp_sfict;   // ��������� ������.
#else //EXTHREAD_LOCAL
exblockstack_t                cBigTemp_stack;   // ���� (�� ������������).
#endif//EXTHREAD_LOCAL

//================================================
//      ���������� ���������� ���������� - �� ���
//      ���������������� ��� �������������� ����.
//================================================

#ifndef _CBIGNUM_MT
cBigNumber cBigNumberLastDivMod;                // ������� �� ���������� �������.
cBigNumber cBigNumberLastRootRm;                // ������� �� ���������� �����.
#endif//_CBIGNUM_MT

//================================================
//      ��������� �������.
//================================================

inline size_t size_max (size_t n1, size_t n2)
{
  return ((n1 > n2)? n1 : n2);
}
inline size_t size_min (size_t n1, size_t n2)
{
  return ((n1 < n2)? n1 : n2);
}

//================================================
//      ���������� �������������� �������.
//================================================

cBigNumber& cBigNumber::setbits     (const cBigNumber& b)
{
  size_t n = cBigNumberWords (CBPTRTYPE(b));
  set (n > 0? cULongBits ((unsigned CBNL)
                (b.code()[--n] ^ (b.hiword()>>(BITS-1)))): 0);
  return addmul ((CBNL)n, BITS);
}

cBigNumber& cBigNumber::setbits     (CBNL b)
{
  return set (cLongBits (b));
}

cBigNumber& cBigNumber::setexbits   (const cBigNumber& b)
{
  size_t n = cBigNumberExWords (CBPTRTYPE(b));
  set (cLongExBits (b.code()[n - (b.length() == 0)]));
  return addmul ((CBNL)n, BITS);
}

cBigNumber& cBigNumber::setexbits   (CBNL b)
{
  return set (cLongExBits (b));
}

//================================================
//      ���������� ��������������� ��������������.
//================================================

cBigNumber& cBigNumber::fit()
{
  if (length() == 0) checkexpand (1);
  checkindex (cBigNumberFit (EXPTRTYPE(*this)));
  return *this;
}

cBigNumber& cBigNumber::tab()
{
  checkexpand (exmuladd (BITS, length() + (length() == 0), BITS * 2 + 1));
  cBigNumberTab (EXPTRTYPE(*this));
  return *this;
}

cBigNumber& cBigNumber::smp()
{
#ifndef _CBIGNUM_HARDWARE_MUL
  checkexpand (exmuladd (BITS, length() + (length() == 0), BITS * 2 + 1));
  cBigNumberTab (EXPTRTYPE(*this));
#endif//_CBIGNUM_HARDWARE_MUL
  return *this;
}

cBigNumber& cBigNumber::gc()
{
  if (len != 0)
  {
#ifndef _CBIGNUM_DEF_ALLOC
    if (_testnot0() == 0) clear(); else
#endif//_CBIGNUM_DEF_ALLOC
    checkshrink (length());
  }
  return *this;
}

cBigNumber& cBigNumber::pack()
{
  if (len != 0)
  {
#ifndef _CBIGNUM_DEF_ALLOC
    if (_testnot0() == 0) clear(); else
#endif//_CBIGNUM_DEF_ALLOC
    reallocate (length() + 1);
  }
  return *this;
}

#ifdef  _CBIGNUM_DEF_ALLOC
void cBigNumber::clear()
{
  if (len != 0) { checkshrink (1);      }
  if (len >= 1) { e [0] = 1; e [1] = 0; }
}
#endif//_CBIGNUM_DEF_ALLOC

//================================================
//      ���������� ������� �������� ����������.
//================================================

cBigNumber& cBigNumber::operator ++()
{
  checkexpand (length() + 1);
  checkindex (cBigNumberMInc (EXPTRTYPE(*this)));
  return *this;
}

cBigNumber& cBigNumber::operator --()
{
  checkexpand (length() + 1);
  checkindex (cBigNumberMDec (EXPTRTYPE(*this)));
  return *this;
}

cBigNumber& cBigNumber::mul2()
{
  checkexpand (length() + 1);
  checkindex (cBigNumberMMul2 (EXPTRTYPE(*this)));
  return *this;
}

cBigNumber& cBigNumber::div2()
{
  checkexpand (length());
  checkindex (cBigNumberMDiv2 (EXPTRTYPE(*this)));
  return *this;
}

cBigNumber& cBigNumber::pow2()
{
  checkexpand (length() * 2 + 2);
  cBigNumberMul (CBPTRTYPE(*this), CBPTRTYPE(*this), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::sqrt()
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastRootRm;
#endif//_CBIGNUM_MT
  swap (cBigNumberLastRootRm);
  return setsqrtrm (cBigNumberLastRootRm);
}

//================================================
//      ���������� �������� ������������.
//================================================

cBigNumber& cBigNumber::setxor (const cBigNumber& a, const cBigNumber& b)
{
  checkindex (a.length() >= b.length()?
    (checkexpand (a.length()),
     cBigNumberXor (CBPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this))):
    (checkexpand (b.length()),
     cBigNumberXor (CBPTRTYPE(b), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setxor (const cBigNumber& a, CBNL b)
{
  checkindex (a.length()?
    (checkexpand (a.length()),
     cBigNumberXor (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), EXPTRTYPE(*this))):
    (checkexpand (1),
     cBigNumberXor (CBPTRTYPE(_cBigLong(b)), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setxor (CBNL a, const cBigNumber& b)
{
  checkindex (b.length()?
    (checkexpand (b.length()),
     cBigNumberXor (CBPTRTYPE(b), CBPTRTYPE(_cBigLong(a)), EXPTRTYPE(*this))):
    (checkexpand (1),
     cBigNumberXor (CBPTRTYPE(_cBigLong(a)), CBPTRTYPE(b), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setxor (CBNL a, CBNL b)
{
  return *this = a ^ b;
}

cBigNumber& cBigNumber::setand (const cBigNumber& a, const cBigNumber& b)
{
  checkindex (a.length() >= b.length()?
    (checkexpand (a.length()),
     cBigNumberAnd (CBPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this))):
    (checkexpand (b.length()),
     cBigNumberAnd (CBPTRTYPE(b), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setand (const cBigNumber& a, CBNL b)
{
  checkindex (a.length()?
    (checkexpand (a.length()),
     cBigNumberAnd (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), EXPTRTYPE(*this))):
    (checkexpand (1),
     cBigNumberAnd (CBPTRTYPE(_cBigLong(b)), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setand (CBNL a, const cBigNumber& b)
{
  checkindex (b.length()?
    (checkexpand (b.length()),
     cBigNumberAnd (CBPTRTYPE(b), CBPTRTYPE(_cBigLong(a)), EXPTRTYPE(*this))):
    (checkexpand (1),
     cBigNumberAnd (CBPTRTYPE(_cBigLong(a)), CBPTRTYPE(b), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setand (CBNL a, CBNL b)
{
  return *this = a & b;
}

cBigNumber& cBigNumber::setor  (const cBigNumber& a, const cBigNumber& b)
{
  checkindex (a.length() >= b.length()?
    (checkexpand (a.length()),
     cBigNumberOr (CBPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this))):
    (checkexpand (b.length()),
     cBigNumberOr (CBPTRTYPE(b), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setor  (const cBigNumber& a, CBNL b)
{
  checkindex (a.length()?
    (checkexpand (a.length()),
     cBigNumberOr (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), EXPTRTYPE(*this))):
    (checkexpand (1),
     cBigNumberOr (CBPTRTYPE(_cBigLong(b)), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setor  (CBNL a, const cBigNumber& b)
{
  checkindex (b.length()?
    (checkexpand (b.length()),
     cBigNumberOr (CBPTRTYPE(b), CBPTRTYPE(_cBigLong(a)), EXPTRTYPE(*this))):
    (checkexpand (1),
     cBigNumberOr (CBPTRTYPE(_cBigLong(a)), CBPTRTYPE(b), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setor  (CBNL a, CBNL b)
{
  return *this = a | b;
}

cBigNumber& cBigNumber::setadd (const cBigNumber& a, const cBigNumber& b)
{
  checkindex (a.length() >= b.length()?
    (checkexpand (a.length() + 1),
     cBigNumberAdd (CBPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this))):
    (checkexpand (b.length() + 1),
     cBigNumberAdd (CBPTRTYPE(b), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setadd (const cBigNumber& a, CBNL b)
{
  checkindex (a.length()?
    (checkexpand (a.length() + 1),
     cBigNumberAdd (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), EXPTRTYPE(*this))):
    (checkexpand (2),
     cBigNumberAdd (CBPTRTYPE(_cBigLong(b)), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setadd (CBNL a, const cBigNumber& b)
{
  checkindex (b.length()?
    (checkexpand (b.length() + 1),
     cBigNumberAdd (CBPTRTYPE(b), CBPTRTYPE(_cBigLong(a)), EXPTRTYPE(*this))):
    (checkexpand (2),
     cBigNumberAdd (CBPTRTYPE(_cBigLong(a)), CBPTRTYPE(b), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setadd (CBNL a, CBNL b)
{
  checkexpand (2);
  checkindex (cBigNumberAdd (CBPTRTYPE(_cBigLong(a)), CBPTRTYPE(_cBigLong(b)),
                             EXPTRTYPE(*this)));
  return *this;
}

cBigNumber& cBigNumber::setsub (const cBigNumber& a, const cBigNumber& b)
{
  checkindex (a.length() >= b.length()?
    (checkexpand (a.length() + 1),
     cBigNumberSub (CBPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this))):
    (checkexpand (b.length() + 1),
     cBigNumberSubS(CBPTRTYPE(b), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setsub (const cBigNumber& a, CBNL b)
{
  checkindex (a.length()?
    (checkexpand (a.length() + 1),
     cBigNumberSub (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), EXPTRTYPE(*this))):
    (checkexpand (2),
     cBigNumberSubS(CBPTRTYPE(_cBigLong(b)), CBPTRTYPE(a), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setsub (CBNL a, const cBigNumber& b)
{
  checkindex (b.length()?
    (checkexpand (b.length() + 1),
     cBigNumberSubS(CBPTRTYPE(b), CBPTRTYPE(_cBigLong(a)), EXPTRTYPE(*this))):
    (checkexpand (2),
     cBigNumberSub (CBPTRTYPE(_cBigLong(a)), CBPTRTYPE(b), EXPTRTYPE(*this))));
  return *this;
}

cBigNumber& cBigNumber::setsub (CBNL a, CBNL b)
{
  checkexpand (2);
  checkindex (cBigNumberSub (CBPTRTYPE(_cBigLong(a)), CBPTRTYPE(_cBigLong(b)),
                             EXPTRTYPE(*this)));
  return *this;
}

cBigNumber& cBigNumber::setmul (const cBigNumber& a, const cBigNumber& b)
{
  size_t la = a.length(), lb = b.length();
  checkexpand (la + lb + 2);
  if (la >= lb)
        cBigNumberMul (CBPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this));
  else  cBigNumberMul (CBPTRTYPE(b), CBPTRTYPE(a), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setmul (const cBigNumber& a, CBNL b)
{
  checkexpand (a.length() + 3);
  cBigNumberMul (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setmul (CBNL a, const cBigNumber& b)
{
  checkexpand (b.length() + 3);
  cBigNumberMul (CBPTRTYPE(b), CBPTRTYPE(_cBigLong(a)), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setmul (CBNL a, CBNL b)
{
  checkexpand (2); e [0] = 2; cLongMul (a, b, e + 1);
  checkindex (cBigNumberFit (EXPTRTYPE(*this)));
  return *this;
}

cBigNumber& cBigNumber::setdiv (const cBigNumber& a, const cBigNumber& b)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod;
#endif//_CBIGNUM_MT
  return setdivmod (cBigNumberLastDivMod = a, b);
}

cBigNumber& cBigNumber::setdiv (const cBigNumber& a, CBNL b)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod;
#endif//_CBIGNUM_MT
  return setdivmod (cBigNumberLastDivMod = a, b);
}

cBigNumber& cBigNumber::setdiv (CBNL a, const cBigNumber& b)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod;
#endif//_CBIGNUM_MT
  return setdivmod (cBigNumberLastDivMod = a, b);
}

cBigNumber& cBigNumber::setdiv (CBNL a, CBNL b)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod;
#endif//_CBIGNUM_MT
  return setdivmod (cBigNumberLastDivMod = a, b);
}

cBigNumber& cBigNumber::setmod (const cBigNumber& a, const cBigNumber& b)
{
  return (*this = a) %= b;
}

cBigNumber& cBigNumber::setmod (const cBigNumber& a, CBNL b)
{
  return (*this = a) %= b;
}

cBigNumber& cBigNumber::setmod (CBNL a, const cBigNumber& b)
{
  return (*this = a) %= b;
}

cBigNumber& cBigNumber::setmod (CBNL a, CBNL b)
{
  return (*this = a) %= b;
}

//================================================
//      ���������� �������.
//================================================

cBigNumber& cBigNumber::operator <<= (const cBigNumber& b)
{
  if (b.length() > 1) erange();
  return (*this <<= b.hiword());
}

cBigNumber& cBigNumber::operator <<= (CBNL b)
{
  if (b < 0) return (*this >>= (-b));
  int n = (int)(b % BITS); b /= BITS;
  checkexpand ((unsigned CBNL)b < UINT_MAX - 2 - length()?
                      (size_t)b + length() + 2: UINT_MAX);
  cBigNumberFit (EXPTRTYPE(*this));
  for (; n > 0; --n) cBigNumberMMul2 (EXPTRTYPE(*this));
  cBigNumberCopyShl (CBPTRTYPE(*this), (size_t)b, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setshl (const cBigNumber& a, const cBigNumber& b)
{
  *this = a; return *this <<= b;
}

cBigNumber& cBigNumber::setshl (const cBigNumber& a, CBNL b)
{
  *this = a; return *this <<= b;
}

cBigNumber& cBigNumber::setshl (CBNL a, const cBigNumber& b)
{
  *this = a; return *this <<= b;
}

cBigNumber& cBigNumber::setshl (CBNL a, CBNL b)
{
  *this = a; return *this <<= b;
}

cBigNumber& cBigNumber::operator >>= (const cBigNumber& b)
{
  if (b.length() > 1) erange();
  return (*this >>= b.hiword());
}

cBigNumber& cBigNumber::operator >>= (CBNL b)
{
  if (b < 0) { if (-b > 0) return (*this <<= (-b));
               else { cBigNumberERange (CBPTRTYPE(_cBigLong (b))); b = 0; } }
  size_t n = (size_t) b % BITS; b /= BITS;
  if (length() == 0) checkexpand (1);
  cBigNumberFit (EXPTRTYPE(*this));
  for (; n > 0; --n) cBigNumberMDiv2 (EXPTRTYPE(*this));
  cBigNumberCopyShr (CBPTRTYPE(*this),
                     (unsigned CBNL)b <= UINT_MAX? (size_t)b: UINT_MAX,
                     EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setshr (const cBigNumber& a, const cBigNumber& b)
{
  *this = a; return *this >>= b;
}

cBigNumber& cBigNumber::setshr (const cBigNumber& a, CBNL b)
{
  *this = a; return *this >>= b;
}

cBigNumber& cBigNumber::setshr (CBNL a, const cBigNumber& b)
{
  *this = a; return *this >>= b;
}

cBigNumber& cBigNumber::setshr (CBNL a, CBNL b)
{
  *this = a; return *this >>= b;
}

//================================================
//      ���������� ���������� � �������.
//================================================

cBigNumber& cBigNumber::pow (const cBigNumber& b)
{
  cBigNumber a; swap (a);
  if (b.hiword() >= 0)                          // ������������� �������.
  {
    setbits (a) *= b;                           // ������ ������� ����������.
    if (log2_BITS >= 0)
      *this >>= log2_BITS;                      // ���������������� �������.
    else
      *this = cBigDivMod (*this, BITS);         // ����� �������.
  }
  else *this = 0;                               // ������������� �������.
  CBNL lw = loword();
  size_t n = ((length() <= 1 &&
              (unsigned CBNL)lw <= UINT_MAX - 4)?
                    ((size_t)lw + 4): UINT_MAX);
  checkexpand (n); a.checkexpand (n);
  cBigNumberPow (EXPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::pow (CBNL b)
{
  cBigNumber a; swap (a);
  if (b >= 0)                                   // ������������� �������.
  {
    setbits (a) *= b;                           // ������ ������� ����������.
    if (log2_BITS >= 0)
      *this >>= log2_BITS;                      // ���������������� �������.
    else
      *this = cBigDivMod (*this, BITS);         // ����� �������.
  }
  else *this = 0;                               // ������������� �������.
  CBNL lw = loword();
  size_t n = ((length() <= 1 &&
              (unsigned CBNL)lw <= UINT_MAX - 4)?
                    ((size_t)lw + 4): UINT_MAX);
  checkexpand (n); a.checkexpand (n);
  cBigNumberPow (EXPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setpow (const cBigNumber& a, const cBigNumber& b)
{
  *this = a; return pow (b);
}

cBigNumber& cBigNumber::setpow (const cBigNumber& a, CBNL b)
{
  *this = a; return pow (b);
}

cBigNumber& cBigNumber::setpow (CBNL a, const cBigNumber& b)
{
  *this = a; return pow (b);
}

cBigNumber& cBigNumber::setpow (CBNL a, CBNL b)
{
  *this = a; return pow (b);
}

//================================================
//      ���������� ���������� � ������� �� ������.
//================================================

cBigNumber& cBigNumber::powmod (const cBigNumber& b, const cBigNumber& mod)
{
  cBigTemp a; swap (a);
  size_t n = mod.length() * 2 + 2;
  checkexpand (n); a.checkexpand (size_max (a.length() + 1, n));
  cBigNumberPowMod (EXPTRTYPE(a), CBPTRTYPE(b),
                    CBPTRTYPE(mod), EXPTRTYPE(*this));
  shrink (length()); checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::powmod (const cBigNumber& b, CBNL mod)
{
  cBigTemp a; swap (a);
  checkexpand (4); a.checkexpand (size_max (a.length() + 1, 4));
  cBigNumberPowMod (EXPTRTYPE(a), CBPTRTYPE(b),
                    CBPTRTYPE(_cBigLong(mod)), EXPTRTYPE(*this));
  shrink (length()); checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::powmod (CBNL b, const cBigNumber& mod)
{
  cBigTemp a; swap (a);
  size_t n = mod.length() * 2 + 2;
  checkexpand (n); a.checkexpand (size_max (a.length() + 1, n));
  cBigNumberPowMod (EXPTRTYPE(a), CBPTRTYPE(_cBigLong(b)),
                    CBPTRTYPE(mod), EXPTRTYPE(*this));
  shrink (length()); checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::powmod (CBNL b, CBNL mod)
{
  cBigTemp a; swap (a);
  checkexpand (4); a.checkexpand (size_max (a.length() + 1, 4));
  cBigNumberPowMod (EXPTRTYPE(a), CBPTRTYPE(_cBigLong(b)),
                    CBPTRTYPE(_cBigLong(mod)), EXPTRTYPE(*this));
  shrink (length()); checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setpowmod (const cBigNumber& a, const cBigNumber& b,
                                   const cBigNumber& mod)
{
  *this = a; return powmod (b, mod);
}
cBigNumber& cBigNumber::setpowmod (const cBigNumber& a, const cBigNumber& b,
                                   CBNL mod)
{
  *this = a; return powmod (b, mod);
}

cBigNumber& cBigNumber::setpowmod (const cBigNumber& a, CBNL b,
                                   const cBigNumber& mod)
{
  *this = a; return powmod (b, mod);
}
cBigNumber& cBigNumber::setpowmod (const cBigNumber& a, CBNL b, CBNL mod)
{
  *this = a; return powmod (b, mod);
}

cBigNumber& cBigNumber::setpowmod (CBNL a, const cBigNumber& b,
                                   const cBigNumber& mod)
{
  *this = a; return powmod (b, mod);
}
cBigNumber& cBigNumber::setpowmod (CBNL a, const cBigNumber& b, CBNL mod)
{
  *this = a; return powmod (b, mod);
}

cBigNumber& cBigNumber::setpowmod (CBNL a, CBNL b, const cBigNumber& mod)
{
  *this = a; return powmod (b, mod);
}
cBigNumber& cBigNumber::setpowmod (CBNL a, CBNL b, CBNL mod)
{
  *this = a; return powmod (b, mod);
}

//================================================
//      ���������� ����������� �����.
//================================================

cBigNumber& cBigNumber::setsqrt (const cBigNumber& b)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastRootRm;
#endif//_CBIGNUM_MT
  return setsqrtrm (cBigNumberLastRootRm = b);
}

cBigNumber& cBigNumber::setsqrt (CBNL b)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastRootRm;
#endif//_CBIGNUM_MT
  return setsqrtrm (cBigNumberLastRootRm = b);
}

//================================================
//      ���������� ���������� ��������� �����.
//================================================

cBigNumber& cBigNumber::setrandom (unsigned long (*p)(), unsigned CBNL lBits)
{
  checkexpand ((size_t)(lBits / BITS + 1));
  cBigNumberRandom (p, lBits, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

//================================================
//      ���������� ��������������� ��������.
//================================================

cBigNumber& cBigNumber::add (const cBigNumber& b, size_t k)
{
  checkexpand (size_max (length(), b.length() + k) + 1);
  cBigNumberMAddShl (EXPTRTYPE(*this), CBPTRTYPE(b), k);
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::add (CBNL b, size_t k)
{
  checkexpand (size_max (length(), k + 1) + 1);
  cBigNumberMAddShl (EXPTRTYPE(*this), CBPTRTYPE(_cBigLong(b)), k);
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::sub (const cBigNumber& b, size_t k)
{
  checkexpand (size_max (length(), b.length() + k) + 1);
  cBigNumberMSubShl (EXPTRTYPE(*this), CBPTRTYPE(b), k);
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::sub (CBNL b, size_t k)
{
  checkexpand (size_max (length(), k + 1) + 1);
  cBigNumberMSubShl (EXPTRTYPE(*this), CBPTRTYPE(_cBigLong(b)), k);
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::addmul (const cBigNumber& a, const cBigNumber& b)
{
  size_t la = a.length(), lb = b.length();
  checkexpand (size_max (length(), la + lb + 1) + 1);
  if (la >= lb)
        cBigNumberMAddMul (CBPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this));
  else  cBigNumberMAddMul (CBPTRTYPE(b), CBPTRTYPE(a), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::addmul (const cBigNumber& a, CBNL b)
{
  checkexpand (size_max (length(), a.length() + 2) + 1);
  cBigNumberMAddMul (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)),
                     EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::addmul (CBNL a, const cBigNumber& b)
{
  checkexpand (size_max (length(), b.length() + 2) + 1);
  cBigNumberMAddMul (CBPTRTYPE(b), CBPTRTYPE(_cBigLong(a)),
                     EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::addmul (CBNL a, CBNL b)
{
  CBNL c [5]; c [0] = 1; c [1] = a;
  checkexpand (size_max (length(), 2) + 1);
  if (b >= 0)
     cBigNumberMAddMulShl (EXPTRTO(CBNL,c,sizeof(c)/sizeof(*c)-1),
                           CBPTRTYPE(_cBigLong(b)), 0, EXPTRTYPE(*this));
  else
     cBigNumberMSubMulShl (EXPTRTO(CBNL,c,sizeof(c)/sizeof(*c)-1),
                           CBPTRTYPE(_cBigLong(-b)), 0, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::submul (const cBigNumber& a, const cBigNumber& b)
{
  size_t la = a.length(), lb = b.length();
  checkexpand (size_max (length(), la + lb + 1) + 1);
  if (la >= lb)
        cBigNumberMSubMul (CBPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this));
  else  cBigNumberMSubMul (CBPTRTYPE(b), CBPTRTYPE(a), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::submul (const cBigNumber& a, CBNL b)
{
  checkexpand (size_max (length(), a.length() + 2) + 1);
  cBigNumberMSubMul (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)),
                     EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::submul (CBNL a, const cBigNumber& b)
{
  checkexpand (size_max (length(), b.length() + 2) + 1);
  cBigNumberMSubMul (CBPTRTYPE(b), CBPTRTYPE(_cBigLong(a)),
                     EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::submul (CBNL a, CBNL b)
{
  CBNL c [5]; c [0] = 1; c [1] = a;
  checkexpand (size_max (length(), 2) + 1);
  if (b >= 0)
     cBigNumberMSubMulShl (EXPTRTO(CBNL,c,sizeof(c)/sizeof(*c)-1),
                           CBPTRTYPE(_cBigLong(b)), 0, EXPTRTYPE(*this));
  else
     cBigNumberMAddMulShl (EXPTRTO(CBNL,c,sizeof(c)/sizeof(*c)-1),
                           CBPTRTYPE(_cBigLong(-b)), 0, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setdivmod (cBigNumber& a, const cBigNumber& b)
{
  checkexpand (a.fit().length() + 2);
  cBigNumberMModDiv (EXPTRTYPE(a), CBPTRTYPE(b), EXPTRTYPE(*this));
  a.checkindex (a.length());
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setdivmod (cBigNumber& a, CBNL b)
{
  checkexpand (a.fit().length() + 2);
  cBigNumberMModDiv (EXPTRTYPE(a), CBPTRTYPE(_cBigLong(b)),
                     EXPTRTYPE(*this));
  a.checkindex (a.length());
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setsqrtrm (cBigNumber& a)
{
  checkexpand (a.fit().length() / 2 + 1);
  if (a.hiword() >= 0)
    cBigNumberMRmSqrt (EXPTRTYPE(a), EXPTRTYPE(*this));
  else
    *this = 0;
  a.checkindex (a.length());
  checkindex (length());
  return *this;
}
//================================================
//      ���������� �������� � �������� �������.
//================================================

cBigNumber& cBigNumber::addmultab (const cBigNumber& a, const cBigNumber& b,
                                   size_t k)
{
  checkexpand (size_max (length(), a.length() + b.length() + k) + 1);
  assert (b.hiword() >= 0);
  cBigNumberMAddMulShlTab (CBPTRTYPE(a), CBPTRTYPE(b), k, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::addmultab (const cBigNumber& a, CBNL b, size_t k)
{
  checkexpand (size_max (length(), a.length() + k + 1) + 1);
  assert (b >= 0);
  cBigNumberMAddMulShlTab (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), k,
                           EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::submultab (const cBigNumber& a, const cBigNumber& b,
                                   size_t k)
{
  checkexpand (size_max (length(), a.length() + b.length() + k) + 1);
  assert (b.hiword() >= 0);
  cBigNumberMSubMulShlTab (CBPTRTYPE(a), CBPTRTYPE(b), k, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::submultab (const cBigNumber& a, CBNL b, size_t k)
{
  checkexpand (size_max (length(), a.length() + k + 1) + 1);
  assert (b >= 0);
  cBigNumberMSubMulShlTab (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), k,
                           EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::addmulsmp (const cBigNumber& a, const cBigNumber& b,
                                   size_t k)
{
  checkexpand (size_max (length(), a.length() + b.length() + k) + 1);
  assert (b.hiword() >= 0);
#ifdef  _CBIGNUM_HARDWARE_MUL
  cBigNumberMAddMulShl (EXPTRTYPE(*(cBigNumber*)&a), CBPTRTYPE(b),
                        k, EXPTRTYPE(*this));
#else //_CBIGNUM_HARDWARE_MUL
  cBigNumberMAddMulShlTab (CBPTRTYPE(a), CBPTRTYPE(b), k, EXPTRTYPE(*this));
#endif//_CBIGNUM_HARDWARE_MUL
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::addmulsmp (const cBigNumber& a, CBNL b, size_t k)
{
  checkexpand (size_max (length(), a.length() + k + 1) + 1);
  assert (b >= 0);
#ifdef  _CBIGNUM_HARDWARE_MUL
  cBigNumberMAddMulShl (EXPTRTYPE(*(cBigNumber*)&a), CBPTRTYPE(_cBigLong(b)),
                        k, EXPTRTYPE(*this));
#else //_CBIGNUM_HARDWARE_MUL
  cBigNumberMAddMulShlTab (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), k,
                           EXPTRTYPE(*this));
#endif//_CBIGNUM_HARDWARE_MUL
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::submulsmp (const cBigNumber& a, const cBigNumber& b,
                                   size_t k)
{
  checkexpand (size_max (length(), a.length() + b.length() + k) + 1);
  assert (b.hiword() >= 0);
#ifdef  _CBIGNUM_HARDWARE_MUL
  cBigNumberMSubMulShl (EXPTRTYPE(*(cBigNumber*)&a), CBPTRTYPE(b),
                        k, EXPTRTYPE(*this));
#else //_CBIGNUM_HARDWARE_MUL
  cBigNumberMSubMulShlTab (CBPTRTYPE(a), CBPTRTYPE(b), k, EXPTRTYPE(*this));
#endif//_CBIGNUM_HARDWARE_MUL
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::submulsmp (const cBigNumber& a, CBNL b, size_t k)
{
  checkexpand (size_max (length(), a.length() + k + 1) + 1);
  assert (b >= 0);
#ifdef  _CBIGNUM_HARDWARE_MUL
  cBigNumberMSubMulShl (EXPTRTYPE(*(cBigNumber*)&a), CBPTRTYPE(_cBigLong(b)),
                        k, EXPTRTYPE(*this));
#else //_CBIGNUM_HARDWARE_MUL
  cBigNumberMSubMulShlTab (CBPTRTYPE(a), CBPTRTYPE(_cBigLong(b)), k,
                           EXPTRTYPE(*this));
#endif//_CBIGNUM_HARDWARE_MUL
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setdivtab (const cBigNumber& a, const cBigNumber& b,
                                   size_t k)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod;
#endif//_CBIGNUM_MT
  cBigNumberLastDivMod = a;
  checkexpand (cBigNumberLastDivMod.fit().length() + 2);
  cBigNumberMModDivShlTab (EXPTRTYPE(cBigNumberLastDivMod), CBPTRTYPE(b), k,
                           EXPTRTYPE(*this));
  cBigNumberLastDivMod.checkindex (cBigNumberLastDivMod.length());
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setdivtab (CBNL a, const cBigNumber& b, size_t k)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod;
#endif//_CBIGNUM_MT
  cBigNumberLastDivMod = a;
  checkexpand (3);
  cBigNumberMModDivShlTab (EXPTRTYPE(cBigNumberLastDivMod), CBPTRTYPE(b), k,
                           EXPTRTYPE(*this));
  cBigNumberLastDivMod.checkindex (cBigNumberLastDivMod.length());
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setmodtab (const cBigNumber& a, const cBigNumber& b,
                                   size_t k)
{
  *this = a; fit();
  cBigNumberMModShlTab (EXPTRTYPE(*this), CBPTRTYPE(b), k);
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setmodtab (CBNL a, const cBigNumber& b, size_t k)
{
  *this = a;
  cBigNumberMModShlTab (EXPTRTYPE(*this), CBPTRTYPE(b), k);
  checkindex (length());
  return *this;
}

cBigNumber& cBigNumber::setdivmodtab (cBigNumber& a, const cBigNumber& b,
                                      size_t k)
{
  checkexpand (a.fit().length() + 2);
  cBigNumberMModDivShlTab (EXPTRTYPE(a), CBPTRTYPE(b), k, EXPTRTYPE(*this));
  a.checkindex (a.length());
  checkindex (length());
  return *this;
}

//================================================
//      ������������ ����� �� �������� �����.
//================================================

//      Strong Probable Primarily test for base b
//      by Olivier Langlois <olanglois@sympatico.ca>
//      ������:
//      0 - ���������
//      2 - ��������, �������
//      http://primes.utm.edu/glossary/page.php?sort=StrongPRP

int     b_SPRP  (
        const cBigNumber& n,            // ����������� �����.
        const cBigNumber& b             // ����.
)
{
  if (n <= 3) return (n < 2? 0: 2);     // �������� �� ��������.
  if ((n.loword() & 1) == 0) return 0;  // �������� �� ����������.

//                                            a
//      Find t and a satisfying: n - 1 = t * 2 , t odd
//

  cBigNumber t = n;
  cBigNumber nMinus1 = --t;
  cBigNumber a; a.setexbits (nMinus1);
  t >>= a;

//      We check the congruence class of      i
//                                       t * 2
//                                      b      % n
//      for each i from 0 to a - 1.
//      If any one is correct, then n passes.
//      Otherwise, n fails.

  cBigNumber test; test.setpowmod (b, t, n);

  if (test == 1 || test == nMinus1) return 2;

  if (a)
  {
    while (--a)
    {
      test.pow2(); test %= n; if (test == nMinus1) return 2;
    }
  }

  return 0;
}

//================================================
//      ���������.
//================================================

const char* cBigNumber::pszConverting   = "Converting";
const char* cBigNumber::pszFormatting   = "Formatting";

//================================================
//      ������� ��� �������������.
//================================================

//      ������� ������������� ���� � �������.

const char  cBigNumber::numtochar [2] [MAX_RADIX + 1] =
                       {{'0','1','2','3','4','5','6','7','8','9',
                         'a','b','c','d','e','f','x'},
                        {'0','1','2','3','4','5','6','7','8','9',
                         'A','B','C','D','E','F','X'}};

//      ������� ������������� �������� � �����, ����������� SqrTab().

      char  cBigNumber::chartonum [UCHAR_MAX + 1];

//      � ������� ������������� ��� ������� ��������� ������������
//      ������������ ������������� ������� ���� long � ����� ����
//      � ��������� ������������� ���� �������.

static const long   maxradix[][2] = {{ 0L,  1},     //  0
                                     { 1L,  1},     //  1
                            { 1073741824L, 30},     //  2
                            { 1162261467L, 19},     //  3
                            { 1073741824L, 15},     //  4
                            { 1220703125L, 13},     //  5
                            {  362797056L, 11},     //  6
                            { 1977326743L, 11},     //  7
                            { 1073741824L, 10},     //  8
                            {  387420489L,  9},     //  9
                            { 1000000000L,  9},     // 10
                            {  214358881L,  8},     // 11
                            {  429981696L,  8},     // 12
                            {  815730721L,  8},     // 13
                            { 1475789056L,  8},     // 14
                            {  170859375L,  7},     // 15
                            {  268435456L,  7}};    // 16

//      ��� ����������� �����/������ �������������� ����� ��� �������
//      ��������� ��������� ������ � ������������ �����������������
//      ���������� ��������� ������������ � �������.
//
//      ������������ ����� ��������� MAX_NP_IN/MAX_NP_OUT ������������
//      �� �������� ����������� ������� �����/������ ����� ������������
//      �� 1,000,000 ��� �� ���������� � ������� ����� ������� >= 256�.
//
//      ������� �������� MAX_NP_IN ������������ ����������� ���
//      ������� ��������� ������������ ������ ��������� ��������,
//      � ����� ��� ���� ����� ��, ��� MAX_NP_OUT.

#ifdef  _CBIGNUM_KARATSUBA_MUL
#define MAX_NP_IN   ((sizeof (size_t))==2? 2: 18)
#else //_CBIGNUM_KARATSUBA_MUL
#define MAX_NP_IN   ((sizeof (size_t))==2? 2: 9)
#endif//_CBIGNUM_KARATSUBA_MUL
#define MAX_NP_OUT  ((sizeof (size_t))==2? 2: 9)
#define MAX_NP      (MAX_NP_IN > MAX_NP_OUT? MAX_NP_IN: MAX_NP_OUT)

//      ��������, ��� ������� ��������� ��������-���������.
//      SHOW_NP_IN ������ SHOW_NP_OUT, ������ ��� ���� �������
//      ������, � ����������� � ���������� ����������.

#define SHOW_NP_IN  (MAX_NP_IN < 12? MAX_NP_IN: 12)
#define SHOW_NP_OUT (MAX_NP_OUT)

//      ��� ��������� ��������� �������� � ����������� ������� �������.

#ifdef  _CBIGNUM_HARDWARE_MUL
#define MAX_SMP_IN  (MAX_NP_IN < 7? MAX_NP_IN: 7)
#else //_CBIGNUM_HARDWARE_MUL
#define MAX_SMP_IN  (MAX_NP_IN < 8? MAX_NP_IN: 8)
#endif//_CBIGNUM_HARDWARE_MUL
#define MAX_TAB_OUT (MAX_NP_OUT)
#define MAX_TAB     (MAX_SMP_IN > MAX_TAB_OUT? MAX_SMP_IN: MAX_TAB_OUT)

//     �� ��������� ������ �� ����������� ����������� �������,
//     � �� �� ���� �������, ������ ��� � ������������� ������
//     ��� ����������� ������� ����� ����������� � ������������ ������.

static cBigNumber*  powradix    [MAX_RADIX + 1] [MAX_NP + 1];
static cBigNumber*  powradixtab [MAX_RADIX + 1] [MAX_TAB + 1];
static size_t       powradixexw [MAX_RADIX + 1] [MAX_TAB + 1];

//      ���������� ������� ������������� � ������� ������������� � ���������
//      � ������� ����������� ������ ���� ������� ����������� � ��� �������.
//      ��� ���������������, �.�. ������� ����� ���������� ������������ ��
//      ���������� �������.

size_t  cBigNumber::SqrTab (
                unsigned radix,         // ��������� (2..16).
                size_t   len,           // ����� ���� � �����.
                size_t   max_np         // ������������ ����� ��������� + 1.
)                                       // ������ ��������� �����
{                                       // ��������� + 1 (0..max_np).
  assert (radix >= 2);
  assert (radix <= MAX_RADIX);
  assert (max_np <= MAX_NP);
  static size_t npow [MAX_RADIX + 1];   // ����� ��������� + 1.

  if (npow [radix] == 0)
  {
//      ���������� ������� ������������� (��������������� ���).

    int i;
    for (i = 0; i < MAX_RADIX; i++)
    {
      chartonum [numtochar [0] [i]] = (char)i;
      chartonum [numtochar [1] [i]] = (char)i;
    }
    for (i = 0; i <= UCHAR_MAX; i++)
    {
      if (chartonum [i] == 0 && i != '0') chartonum [i] = MAX_RADIX;
    }

//      ��������� ������ ��� �������������. ������ �� ����� ���������,
//      ������ ��� �� �� ���� ������������ ������. ���� � ������������
//      ������� ������������ ����������� ��������� ��������, ��
//      ������������ ���� �� ���, � ������ ����� � ������ ������.

    EXPTR(cBigNumber) pow = EXPTRNEW(cBigNumber, 3);

//      ������� �����������.

    pow [2] = maxradix [radix] [0];
    pow [2].tab();                      // ������� �������.
    powradix    [radix] [0] =
    powradixtab [radix] [0] = &pow [2]; // ��������������� ���.
    powradixexw [radix] [0] = 0;

//      ������� �������� ������������.

    pow [0].setmul (maxradix [radix] [0], maxradix [radix] [0]);
    powradix    [radix] [1] = &pow [0]; // ��������������� ���.

    pow [1].setr (pow [0], powradixexw [radix] [1] = pow [0].exwords());
    pow [1].tab();                      // ������� �������.
    powradixtab [radix] [1] = &pow [1]; // ��������������� ���.

    npow [radix] = 2;                   // ����� ��������� + 1.
  }

//      ������� ���������������� ��������, ���� ���������.

  size_t np = 0;                        // ������ �������� ��������.
  while (np < max_np && powradix [radix] [np]->length() * 4 < len)
  {
    if (++np >= npow [radix])
    {

//      ��������� ������ ��� �������������. ������ �� ����� ���������,
//      ������ ��� �� �� ���� ������������ ������. ���� � ������������
//      ������� ������������ ����������� ��������� ��������, ��
//      ������������ ���� �� ���, � ������ ����� � ������ ������.

      EXPTR(cBigNumber) pow = EXPTRNEW(cBigNumber, 2);

      pow [0] = 0;                      // ������� �� ���� �����.
      if (np <= MAX_SMP_IN)
        pow [0].addmulsmp
          (*powradixtab [radix] [np - 1], *powradixtab [radix] [np - 1],
            powradixexw [radix] [np - 1] * 2);
      else
        pow [0].addmul
          (*powradix [radix] [np - 1], *powradix [radix] [np - 1]);

      powradix [radix] [np] = &pow [0]; // ��������������� ���.

      if (np <= MAX_TAB)
      {
        pow [1].setr (pow [0], powradixexw [radix] [np] = pow [0].exwords());
        pow [1].tab();                  // ������� �������.
        powradixtab [radix] [np] = &pow [1];
      }                                 // ��������������� ���.
      npow [radix] = np + 1;            // ����� ��������� + 1.
    }
  }
  assert (np <= max_np);
  return np;
}

//================================================
//      ���������� �������� ������ � �����.
//================================================

//      ����� �������� ����������� ������� � �����������
//      �������������� ����� �������� ��� ��������� �� 2 �� 16.

cBigNumber& cBigNumber::setunsign (     // �������������� �� ������� ��� �����.
                const EXPTR(char) ps,   // ������ � �������.
                unsigned radix,         // ��������� (2..16).
                size_t   width          // ����� �������� � �������.
            )
{
  assert (ps);
  assert (radix >= 2);
  assert (radix <= MAX_RADIX);

//      ���� �����.
//
//      �������� (������������������) �������� �������� ���:
//      *this = 0;
//      for (; width > 0; --width)
//      {
//        *this *= radix;
//        *this += chartonum [*ps++];
//      }
//
//      � ���������������� ��������� ������ ��������� �����������
//      ������ ��������� � ����������� addmul � addmulsmp.
//      ��� ���������� ����� ��������� �����������
//      ��������� �� ������������ powradix, ���������� �����
//      ����������������� ���������� ��������� radix � ��������.
//      ���������� ������������� ��������� � �����������
//      ������������ � ����� num_ � � ����� ���������� mul.
//      ������������� ���� ����.
//
//      ��������
//      ��������� �������� ������� �������������� ����� ������ ����
//      long � ���������� ����� �������������� �������� ����������.
//      ��������� i-�� ������� ���������� ����� ����������� ��
//      addmul ��� addmulb ���� ���������������� ����������
//      ������� i-1, ������ �� ������� ������������ � mul [i].
//      �������������� ����� ���������� ����� �����������������
//      ����������� �� addmul ��� addmulsmp �������� ���������
//      �� num_.

  *this = 0;
  if (width != 0)
  {
    long lradix =       maxradix [radix] [0];
    int  nradix = (int) maxradix [radix] [1];
    size_t nr = (width - 1) / nradix;   // ����� ������ ���� (�� �����).
    size_t n = width - nr++ * nradix;   // ����� �������� � �������� �����.

//      ���������� ����� ������������� powradix [radix].
//      �������� ��������� ����� �������������.

    size_t np = SqrTab (radix, nr, MAX_NP_IN);
    EXPTR(cBigNumber*) powr    = EXPTRTO(cBigNumber*,
                                         powradix    [radix], MAX_NP_IN + 1);
    EXPTR(cBigNumber*) powrtab = EXPTRTO(cBigNumber*,
                                         powradixtab [radix], MAX_SMP_IN + 1);
    EXPTR(size_t)      powrexw = EXPTRTO(size_t,
                                         powradixexw [radix], MAX_SMP_IN + 1);

//      ��������� ����� ����������.

    _CBIGS cBigNumber num_;                     // ������� ���������.
    _CBIGS exvector<cBigNumber> stk_ (MAX_NP_IN);
    EXPTR(cBigNumber) mul = EXPTRTYPE(stk_);    // ���� ����������.
    { for (size_t i = 1; i < np; i++) mul [i] = 0; }

//      �������� - ���������.

    cTTY cProgress (cBigNumberProgress);
    size_t numlength = nr * sizeof (CBNL);
    if (numlength < cTTY_numscaleK) numlength = 0;
    if (numlength != 0 && np >= SHOW_NP_IN)
      cProgress.showScaled (cBigNumber::pszConverting, 0, (long)numlength);

//      ���� �� ������ ������� �� nradix ����.

    long lhigh = 0;                     // ������� �����.
    if ((nr & 1) != 0) goto odd;        // �������� ����� - �������.
    do
    {
//      �������������� ������� ������ �� nradix ����.

      {
        long lb = 0;                    // �����.
        for(;;)
        {
          lb += chartonum [*ps++];
          assert (lb >= 0);
          if (--n == 0) break;
          lb *= radix;
        }
        n = nradix;
        lhigh = lb;                     // ������� �����.
      }
      --nr;                             // ���������� ������.

//      �������������� ������� ������ �� nradix ����.

odd:  {
        long lb = 0;                    // �����.
        for(;;)
        {
          lb += chartonum [*ps++];
          assert (lb >= 0);
          if (--n == 0) break;
          lb *= radix;
        }
        n = nradix;
        num_ = lb;                      // ������� �����.
      }
      --nr;                             // ���������� ������.

//      ���������� ��������� ������� �������.

      num_.addmul (lhigh, lradix);      // ����������� ����.
      assert (num_ >= 0);

//      ���������� ���������� ������� � ����������� ��������
//      � ���������� �����.

      size_t i = 1;
      for (;; i++)
      {
        if (i < np)                     // ���������� ����������.
        {
          if ((nr & (2<<i)-1) != 0)     // �������� ���������
          {                             // ������������
            num_.swap (mul [i]);        // � �����.
            break;
          }                             // ����� ����������� ���������
                                        // ����� �������� �������.
          if (i <= MAX_SMP_IN)
            num_.addmulsmp (*powrtab [i], mul [i], powrexw [i]);
          else
            num_.addmul (*powr [i], mul [i]);
          assert (num_ >= 0);
        }
        else                            // ���������� �����.
        {
          if (i <= MAX_SMP_IN)
            num_.addmulsmp (*powrtab [i], *this, powrexw [i]);
          else
            num_.addmul (*powr [i], *this);
          assert (num_ >= 0);
          num_.swap (*this);
          break;
        }
      }

      if (numlength != 0 && i == SHOW_NP_IN)
        cProgress.showScaled (cBigNumber::pszConverting,
          (long)(numlength - nr * sizeof (long)), (long)numlength);
    }
    while (nr != 0);
  }

//      ����� �������������.

  return *this;
}

//      ����� �������� ������ � �������� �������������� ����� ��������
//      ��� ��������� �� 2 �� 16. ���� ��������� �� ������� (0), ��
//      ��������� 10 ��� 8 �� �������� 0 ��� 16 �� �������� 0x/0X.

cBigNumber& cBigNumber::set (           // �������������� �� ������.
                const char* psz,        // ������.
                unsigned radix          // ��������� (2..16) ��� 0.
            )                           // 0 - �� ����������� ����� �.
{
  assert (psz);
  assert (radix != 1);
  assert (radix <= MAX_RADIX);
  int sign = 0;                         // ���� �����.
  unsigned c = *psz++;                  // ������� ������.

//      ������������� �����.

  switch (c)
  {
    case '-': sign = 1;
    case '+': c = *psz++;
  }

//      ������������� ���������.

  if (radix == 0)
  {
    radix = 10;
    if (c == '0')
    {
      c = *psz++; radix = 8;
      if (c == 'X' || c == 'x') { c = *psz++; radix = 16; }
    }
  }

//      ����� ����� �����.

  SqrTab (radix);                       // ���������� chartonum.
  int width = 0;
  for (;;)
  {
    if ((size_t)chartonum [c] >= radix) break;
    c = psz [width++];                  // ��������� ������.
  }
  --psz;

//      ������������� �����.

  setunsign (EXPTRTO(char, psz, width), radix, width);

//      ���������� �����.

  if (sign) this->neg();

  return *this;
}

//================================================
//      ���������� �������� ����� � ������.
//================================================

//      ����� �������� ��������������� ����� ��� ��������� �� 2 �� 16,
//      ���� � ��������� �����, ����� ������.
//
//      ����� ��������������:
//      cBigNumber_unsign     ����������� �����
//      cBigNumber_showbase   �������� ������� ��������� (0 ��� 0x)
//      cBigNumber_showpos    �������� ���� ��� ������������� �����
//      cBigNumber_uppercase  ��������� ����� ��� ����� � ���������� > 10.

char*   cBigNumber::toa (               // ������� � ������.
                cBigString& buf_,       // ����������� �����.
                unsigned radix,         // ��������� (2..16) ��� 0 (=10).
                size_t   width,         // ����������� ����� ��������.
                long     fill           // ������-����������� � �����.
        ) const                         // ������ ��������� �� ������.
{
  assert (radix != 1);
  assert (radix <= MAX_RADIX);
  if (radix == 0) radix = 10;
  size_t i = 0;                                 // ������ � ������.
  _CBIGS cBigNumber num_;                       // ������� �������.
  num_ = *this; num_.fit();                     // ������������.
  if (fill & cBigNumber_unsign) num_.unsign();  // ����� ��� �����.

//      ����� ������� ������������� ����.

  register const EXPTR(char) ps = EXPTRTO (char,
                    numtochar [(fill & cBigNumber_uppercase) != 0],
                    sizeof (numtochar [0]));

//      ����� ����� � ���������.

  {
    CBNL sign = num_.comp0();

    if (sign < 0)
    {
      buf_ [i++] = '-'; num_.neg();             // ����� ����� '-'.
    }
    else if (sign > 0 && (fill & cBigNumber_showpos))
    {
      buf_ [i++] = '+';                         // ����� ����� '+'.
    }

    if (radix % 8 == 0 && (fill & cBigNumber_showbase))
    {
      buf_ [i++] = '0';                         // ����� ���������.
      if (radix == 16) buf_ [i++] = ps [16];
    }
  }

//      ������������ ����� � �������� �������.
//
//      �������� (������������������) �������� �������� ���:
//      do
//      {
//        buf_ [i++] = ps [num % radix];
//        num_ /= radix;
//      }
//      while (num_ != 0);
//
//      � ���������������� ��������� ������ ������� �����������
//      ����� setdivmodtab, �������� ������� � ������� �� ���.
//      ��� ���������� ����� ������� �����������
//      ������� �� ������������ powradix, ���������� �����
//      ����������������� ���������� ��������� radix � �������.
//
//      � ���������� ������� �� ������� ����������� � �������� npmax
//      ���������� ������� � �������, ������� ������� �� �����������
//      � �������� npmax-1, � �.�., ���� � ���������� ������� ��
//      ������� ����������� � �������� 0 �� ���������� �����,
//      �������������� � ��������� ����� ���� long, ������� �������������
//      � ������ ����� ����������������� ������� �� radix.
//
//      ������� ������� �������� � num_, � ������� �������
//      � ����� ������� div � ���������� � ����� ��������� kdiv.
//      ������� ����� 1, ���� � ��������������� �������� div
//      ��������� ������� �������. ������������� ������ ������� ����.
//      ��� �������� �������� � ��������������� �� ������ ��������
//      ������� kdiv ������ ��������� 0.
//
//      ��������
//      ������� ������� ������� � ������� ������ �������� � num_ �
//      ������� � div [npmax - 1]; div [npmax - 1] ������� c �������
//      �������� � div [npmax - 1] � ������� � div [npmax - 2] � �.�.
//      �������� ������� � ������� �� ������� div [0] ������������� �
//      ������; � ���� �������������� ������� ������� �������� � div [0].
//      ������� div [1] ����������� � div [0] � ������� �����������
//      ������� c div [0], ����� div [2] ����������� � div [1] �
//      ������� ����������� c div [1], � �.�. ������ �� �������� ��������,
//      ����� ������������ ���������� npmax � �������� ����������� ������.

  size_t j = i;                                 // ������� ������ �����.
  {
//      ���������� ����� ������������� powradix [radix].
//      �������� ��������� ����� �������������.

    size_t np = SqrTab (radix, num_.length(), MAX_TAB_OUT);
    EXPTR(cBigNumber*) powrtab = EXPTRTO(cBigNumber*,
                                         powradixtab [radix], MAX_TAB_OUT + 1);
    EXPTR(size_t)      powrexw = EXPTRTO(size_t,
                                         powradixexw [radix], MAX_TAB_OUT + 1);

//      ������� �����.

    _CBIGS cBigNumber mod_;                     // ������� �������.
    _CBIGS exvector<cBigNumber> stk_ (MAX_TAB_OUT);
    _CBIGS exvector<int>       kstk_ (MAX_TAB_OUT);
    EXPTR(cBigNumber) div = EXPTRTYPE(stk_);    // ���� �������.
    EXPTR(int)       kdiv = EXPTRTYPE(kstk_);   // ���� ���������.

//      �������� - ���������.

    cTTY cProgress (cBigNumberProgress);
    size_t numlength = num_.length() * sizeof (CBNL);
    if (numlength < cTTY_numscaleK) numlength = 0;

//      ���� �� �������.
//      ������������� ��� ������� �����, ����� ������ ���������� �������.

    size_t npmax = np;                          // ������ �������� ��������.
    for (;;)                                    // ���� �� �������.
    {
      if (np == npmax)                          // ������� �������.
      {
        if (numlength != 0 && np >= SHOW_NP_OUT)
          cProgress.showScaled (cBigNumber::pszFormatting,
            (long)(numlength - num_.length() * sizeof (long)),
            (long)numlength);

//        ������� ������� ������� �� ��������������� �����������.
//        ���� ��������� ������� ������� � ������ ��������
//        �������� ������ 0, �� ������ �������� �������� �����������
//        � ���� ��������� �������� �������� �����������.

        num_.swap (mod_);                       // ������ �������� � num_
        num_.setdivmodtab (mod_, *powrtab [np], // � ������� � mod_.
                                  powrexw [np]);

        if (num_.comp0() == 0)                  // ������� ������� �����������,
        {                                       // ������� ������� �������.
          if (np == 0) break;                   // ���� ��� ��������� �������,
                                                // �� ���� �������������,
          num_.swap (mod_);                     // ����� ������� ����������
          npmax = --np;                         // ������� �������,
          numlength = 0;                        // ����������� ���������
          assert (kdiv [np] == 0);              // � ����� ��������
          continue;                             // ������� �����������.
        }
      }
      else                                      // ����� ��������.
      {
        assert (np > 0);                        // ��������.
        assert (kdiv [np - 1] == 0);            // ��������.
        if (kdiv [np] == 0)                     // ���� �������� ���, ��
        {                                       // ��������� � ����������
          ++np; continue;                       // �� �����������.
        }                                       // ���� ������� �������,
        mod_.swap (div [np]);                   // �� ��������� div [np] �
        kdiv [np] = 0;                          // �������� ��� ���������.
      }

//      ���� ������� ������� - ���� ������ ������� ������ 0,
//      �� ������� �� ��������������� �����������.

      for (; np != 0; np--)
      {
        assert (kdiv [np - 1] == 0);            // ��������.
        kdiv [np - 1] = 1;                      // ��������� ��������.
        div  [np - 1].setdivmodtab (mod_, *powrtab [np - 1],
                                           powrexw [np - 1]);
                                                // ������ �������� � div
      }                                         // � ������� � mod_.

//      ���� ��������� �������� ������� � ��������,
//      ������� ������������ � ������� ��������� ����� long.
//      ������� ������� ������� �� mod_, � ������� ������� - �� div [0],
//      ���� ��� ��� ���� (��� ��� ������, ����� ������ npmax == 0).

      {
        register long lnum;                     // �������� �����.
        lnum = (long) mod_.loword();            // ����� �������.
        for (;;)                                // ���� �����������
        {                                       // �� 2 ���.
          register int knum = (int) maxradix [radix] [1] - 1;
          do                                    // ����� knum ����.
          {
            buf_ [i++] = ps [ (int) (lnum % radix) ];
            lnum /= radix;                      // ����� �����.
          }
          while (--knum != 0);

          assert (lnum >= 0);                   // ��������.
          assert (lnum < (long) radix);         // ��������.
          buf_ [i++] = ps [(int)(lnum)];        // ��������� �����.

          if (kdiv [0] == 0) break;             // ���� �������?
          lnum = (long) div [0].loword();       // ����� ��������.
          kdiv [0] = 0;                         // div [0] ��������.
        }
      }

//      ��������� ����, ������� � ������ �������� ��������.

      np = (npmax != 0);
    }                                           // ���� �� �������.

//      ����� ���������� �������.

    {
      long lnum = (long) mod_.loword();
      if (lnum != 0)                            // �������� �������.
      do
      {
        buf_ [i++] = ps [(int) (lnum % radix)]; // ����� �����.
      }
      while ((lnum /= radix) != 0);
    }

//      ����� ���� ��� �������� �����.

    if (i == j) buf_ [i++] = ps [0];            // ����� ����� 0,
  }

//      ����� �������-����������� � ���������� ������.

  {
    if ((char)fill == '\0') fill = ps [0];      // ������-�����������.
    while (i < width) buf_ [i++] = (char)fill;  // ����� �����������.
    buf_ [i] = 0;                               // ������ ������.
    EXDEBUG (buf_.checkindex (i));
    i--;
  }

//      ������������ ��������.

  {
    register EXPTR(char) ps = EXPTRTYPE(buf_);
    while (i > j)
    {
      char c = ps [j]; ps [j] = ps [i]; j++; ps [i] = c; i--;
    }
  }

//      ����� ������������.

  return ((char*)(const char *)buf_);           // ������ ������.
}

//      ���������� ���������� ����� ������ ��������� �� ����������� ������,
//      ������� �������������� ��� ������ ��������� ������ ������.

#ifdef  _CBIGNUM_TOATMP
char*   cBigNumber::toatmp (            // ������� � ������.
                unsigned radix,         // ��������� (2..16) ��� 0 (=10).
                size_t   width,         // ����������� ����� ��������.
                long     fill           // ������-����������� � �����.
        ) const                         // ������ ��������� �� ������ ��
{                                       // ���������� ����������� ������.
  static cBigString buf_;
  return toa (buf_, radix, width, fill);
}
#endif//_CBIGNUM_TOATMP

//================================================
//      ���������� ������ �� �������.
//================================================

cTTY& operator << (cTTY& os, const cBigNumber& b)
{
  cTTY_h ph = os;
  if (ph) { cBigString buf_; (*ph) (&os, b.toa (buf_), 0, 0, 0); }
  return os;
}

void operator <<= (cTTY& os, const cBigNumber& b)
{
  cTTY_h ph = os;
  if (ph) { cBigString buf_; (*ph) (&os, b.toa (buf_), 0, 0, '\r'); }
}

//================================================
//      ����������������� ���� CBNL ��� �����.
//================================================

cHexDump::cHexDump (unsigned CBNL num)
{
  buffer [BITS / 4] = 0;
  for (size_t i = BITS / 4; i > 0; i--)
  {
    buffer [i - 1] = cBigNumber::numtochar [0] [(size_t)num & 0xF];
    num >>= 4;
  }
}
