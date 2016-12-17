/* --------------------------------------------------------------
    Целые числа с неограниченной разрядной сеткой (версия 2.0).

    http://www.imach.uran.ru/cbignum

    Copyright 1999-2010, Р.Н.Шакиров, ИМаш УрО PAH.
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
//      Служебные объекты в локальной памяти потока.
//================================================

//      Служебные объекты являются глобальными для того, чтобы исключить
//      накладные расходы на создание временных объектов во время выполнения
//      методов, критических с точки зрения производительности.
//      Служебные объекты являются локальными по отношению к каждому
//      потоку программы, как это предусматривается ключевым словом
//      thread_local будущего стандарта C++ 0x.
//      Память, занятая под служебные объекты, не освобождается.
//
//      Для служебных объектов применяется специальные агрегированные
//      типы без конструкторов, чтобы выполнить ограничения трансляторов,
//      а также создаются фиктивные (не используемые) объекты для гарантии
//      подстановки шаблонов.
//
//      Отметим, что вариант со статическими переменными классов вместо
//      глобальных вызывает крах в Borland C++ 4.5 и Builder 1.0,

#ifdef  EXTHREAD_LOCAL
exblockstack_t EXTHREAD_LOCAL cBigTemp_stack;   // Стек неограниченных чисел.
exblockstack<long>            cBigTemp_sfict;   // Фиктивный массив.
#else //EXTHREAD_LOCAL
exblockstack_t                cBigTemp_stack;   // Стек (не используется).
#endif//EXTHREAD_LOCAL

//================================================
//      Подлежащие исключению переменные - не для
//      реентерабельного или многопоточного кода.
//================================================

#ifndef _CBIGNUM_MT
cBigNumber cBigNumberLastDivMod;                // Остаток от последнего деления.
cBigNumber cBigNumberLastRootRm;                // Остаток от последнего корня.
#endif//_CBIGNUM_MT

//================================================
//      Служебные функции.
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
//      Реализация информационных методов.
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
//      Реализация технологических преобразований.
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
//      Реализация унарных операций накопления.
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
//      Реализация операций присваивания.
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
//      Реализация сдвигов.
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
//      Реализация возведения в степень.
//================================================

cBigNumber& cBigNumber::pow (const cBigNumber& b)
{
  cBigNumber a; swap (a);
  if (b.hiword() >= 0)                          // Положительная степень.
  {
    setbits (a) *= b;                           // Оценка размера результата.
    if (log2_BITS >= 0)
      *this >>= log2_BITS;                      // Оптимизированное деление.
    else
      *this = cBigDivMod (*this, BITS);         // Общее деление.
  }
  else *this = 0;                               // Отрицательная степень.
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
  if (b >= 0)                                   // Положительная степень.
  {
    setbits (a) *= b;                           // Оценка размера результата.
    if (log2_BITS >= 0)
      *this >>= log2_BITS;                      // Оптимизированное деление.
    else
      *this = cBigDivMod (*this, BITS);         // Общее деление.
  }
  else *this = 0;                               // Отрицательная степень.
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
//      Реализация возведения в степень по модулю.
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
//      Реализация квадратного корня.
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
//      Реализация генератора случайных чисел.
//================================================

cBigNumber& cBigNumber::setrandom (unsigned long (*p)(), unsigned CBNL lBits)
{
  checkexpand ((size_t)(lBits / BITS + 1));
  cBigNumberRandom (p, lBits, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

//================================================
//      Реализация комбинированных операций.
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
//      Реализация операций с таблицей сдвигов.
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
//      Элементарные тесты на простоту числа.
//================================================

//      Strong Probable Primarily test for base b
//      by Olivier Langlois <olanglois@sympatico.ca>
//      Выдает:
//      0 - составное
//      2 - возможно, простое
//      http://primes.utm.edu/glossary/page.php?sort=StrongPRP

int     b_SPRP  (
        const cBigNumber& n,            // Тестируемое число.
        const cBigNumber& b             // База.
)
{
  if (n <= 3) return (n < 2? 0: 2);     // Проверка на величину.
  if ((n.loword() & 1) == 0) return 0;  // Проверка на нечетность.

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
//      Сообщения.
//================================================

const char* cBigNumber::pszConverting   = "Converting";
const char* cBigNumber::pszFormatting   = "Formatting";

//================================================
//      Таблицы для перекодировки.
//================================================

//      Таблица перекодировки цифр в символы.

const char  cBigNumber::numtochar [2] [MAX_RADIX + 1] =
                       {{'0','1','2','3','4','5','6','7','8','9',
                         'a','b','c','d','e','f','x'},
                        {'0','1','2','3','4','5','6','7','8','9',
                         'A','B','C','D','E','F','X'}};

//      Таблица перекодировки символов в цифры, заполняемая SqrTab().

      char  cBigNumber::chartonum [UCHAR_MAX + 1];

//      В таблице коэффициентов для каждого основания записывается
//      максимальная целочисленная степень типа long и число цифр
//      в строковом представлении этой степени.

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

//      Для оптимизации ввода/вывода неограниченных чисел для каждого
//      основания создается массив с результатами последовательного
//      возведения исходного коэффициента в квадрат.
//
//      Максимальное число квадратов MAX_NP_IN/MAX_NP_OUT определяются
//      по критерию минимизации времени ввода/вывода чисел размерностью
//      до 1,000,000 бит на процессоре с быстрым кэшем размера >= 256К.
//
//      Большое значение MAX_NP_IN определяется применением для
//      старших квадратов эффективного метода умножения Карацубы,
//      а иначе оно быть таким же, как MAX_NP_OUT.

#ifdef  _CBIGNUM_KARATSUBA_MUL
#define MAX_NP_IN   ((sizeof (size_t))==2? 2: 18)
#else //_CBIGNUM_KARATSUBA_MUL
#define MAX_NP_IN   ((sizeof (size_t))==2? 2: 9)
#endif//_CBIGNUM_KARATSUBA_MUL
#define MAX_NP_OUT  ((sizeof (size_t))==2? 2: 9)
#define MAX_NP      (MAX_NP_IN > MAX_NP_OUT? MAX_NP_IN: MAX_NP_OUT)

//      Квадраты, для которых выводится прогресс-индикатор.
//      SHOW_NP_IN больше SHOW_NP_OUT, потому что ввод быстрее
//      вывода, в особенности с аппаратным ускорением.

#define SHOW_NP_IN  (MAX_NP_IN < 12? MAX_NP_IN: 12)
#define SHOW_NP_OUT (MAX_NP_OUT)

//      Для небольших квадратов строится и применяется таблица сдвигов.

#ifdef  _CBIGNUM_HARDWARE_MUL
#define MAX_SMP_IN  (MAX_NP_IN < 7? MAX_NP_IN: 7)
#else //_CBIGNUM_HARDWARE_MUL
#define MAX_SMP_IN  (MAX_NP_IN < 8? MAX_NP_IN: 8)
#endif//_CBIGNUM_HARDWARE_MUL
#define MAX_TAB_OUT (MAX_NP_OUT)
#define MAX_TAB     (MAX_SMP_IN > MAX_TAB_OUT? MAX_SMP_IN: MAX_TAB_OUT)

//     Мы сохраняем ссылки на динамически размещенные объекты,
//     а не на сами объекты, потому что в многопоточном режиме
//     эти статические массивы могут заполняться в конкурентном режиме.

static cBigNumber*  powradix    [MAX_RADIX + 1] [MAX_NP + 1];
static cBigNumber*  powradixtab [MAX_RADIX + 1] [MAX_TAB + 1];
static size_t       powradixexw [MAX_RADIX + 1] [MAX_TAB + 1];

//      Заполнение таблицы перекодировки и массива коэффициентов и квадратов
//      В таблице обязательно должен быть младший коэффициент и его квадрат.
//      Код противогоночный, т.к. функция может вызываться одновременно из
//      нескольких потоков.

size_t  cBigNumber::SqrTab (
                unsigned radix,         // Основание (2..16).
                size_t   len,           // Число слов в числе.
                size_t   max_np         // Максимальное число квадратов + 1.
)                                       // Выдает требуемое число
{                                       // квадратов + 1 (0..max_np).
  assert (radix >= 2);
  assert (radix <= MAX_RADIX);
  assert (max_np <= MAX_NP);
  static size_t npow [MAX_RADIX + 1];   // Число квадратов + 1.

  if (npow [radix] == 0)
  {
//      Заполнение таблицы перекодировки (противогоночный код).

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

//      Размещаем массив для коэффициентов. Массив не будет удаляться,
//      потому что мы на него запоминаются ссылки. Если в параллельных
//      потоках одновременно разместится несколько массивов, то
//      используется один из них, а другие уйдут в утечку памяти.

    EXPTR(cBigNumber) pow = EXPTRNEW(cBigNumber, 3);

//      Младший коэффициент.

    pow [2] = maxradix [radix] [0];
    pow [2].tab();                      // Таблица сдвигов.
    powradix    [radix] [0] =
    powradixtab [radix] [0] = &pow [2]; // Противогоночный код.
    powradixexw [radix] [0] = 0;

//      Квадрат младшего коэффициента.

    pow [0].setmul (maxradix [radix] [0], maxradix [radix] [0]);
    powradix    [radix] [1] = &pow [0]; // Противогоночный код.

    pow [1].setr (pow [0], powradixexw [radix] [1] = pow [0].exwords());
    pow [1].tab();                      // Таблица сдвигов.
    powradixtab [radix] [1] = &pow [1]; // Противогоночный код.

    npow [radix] = 2;                   // Число квадратов + 1.
  }

//      Старшие последовательные квадраты, если требуются.

  size_t np = 0;                        // Индекс текущего квадрата.
  while (np < max_np && powradix [radix] [np]->length() * 4 < len)
  {
    if (++np >= npow [radix])
    {

//      Размещаем массив для коэффициентов. Массив не будет удаляться,
//      потому что мы на него запоминаются ссылки. Если в параллельных
//      потоках одновременно разместится несколько массивов, то
//      используется один из них, а другие уйдут в утечку памяти.

      EXPTR(cBigNumber) pow = EXPTRNEW(cBigNumber, 2);

      pow [0] = 0;                      // Квадрат по коду далее.
      if (np <= MAX_SMP_IN)
        pow [0].addmulsmp
          (*powradixtab [radix] [np - 1], *powradixtab [radix] [np - 1],
            powradixexw [radix] [np - 1] * 2);
      else
        pow [0].addmul
          (*powradix [radix] [np - 1], *powradix [radix] [np - 1]);

      powradix [radix] [np] = &pow [0]; // Противогоночный код.

      if (np <= MAX_TAB)
      {
        pow [1].setr (pow [0], powradixexw [radix] [np] = pow [0].exwords());
        pow [1].tab();                  // Таблица сдвигов.
        powradixtab [radix] [np] = &pow [1];
      }                                 // Противогоночный код.
      npow [radix] = np + 1;            // Число квадратов + 1.
    }
  }
  assert (np <= max_np);
  return np;
}

//================================================
//      Реализация перевода строки в число.
//================================================

//      Метод перевода символьного массива в беззнаковое
//      неограниченное число работает для оснований от 2 до 16.

cBigNumber& cBigNumber::setunsign (     // Преобразование из массива без знака.
                const EXPTR(char) ps,   // Массив с цифрами.
                unsigned radix,         // Основание (2..16).
                size_t   width          // Число символов в массиве.
            )
{
  assert (ps);
  assert (radix >= 2);
  assert (radix <= MAX_RADIX);

//      Ввод числа.
//
//      Исходный (неоптимизированный) алгоритм выглядит так:
//      *this = 0;
//      for (; width > 0; --width)
//      {
//        *this *= radix;
//        *this += chartonum [*ps++];
//      }
//
//      В оптимизированном алгоритме вместо умножения применяется
//      методы умножения с накоплением addmul и addmulsmp.
//      Для сокращения числа умножений применяется
//      умножение на коэффициенты powradix, полученные путем
//      последовательного возведения основания radix в квадраты.
//      Результаты промежуточных умножений с накоплением
//      запоминаются в числе num_ и в стеке множителей mul.
//      Первоначально стек пуст.
//
//      АЛГОРИТМ
//      Множители нулевого порядка представляются одним словом типа
//      long и получаются путем преобразования коротких подцепочек.
//      Множитель i-го порядка получается путем объединения по
//      addmul или addmulb двух последовательных множителей
//      порядка i-1, первый из которых запоминается в mul [i].
//      Результирующее число получается путем последовательного
//      объединения по addmul или addmulsmp старшего множителя
//      из num_.

  *this = 0;
  if (width != 0)
  {
    long lradix =       maxradix [radix] [0];
    int  nradix = (int) maxradix [radix] [1];
    size_t nr = (width - 1) / nradix;   // Число полных слов (не более).
    size_t n = width - nr++ * nradix;   // Число символов в неполном слове.

//      Заполнение стека коэффициентов powradix [radix].
//      Выдается требуемое число коэффициентов.

    size_t np = SqrTab (radix, nr, MAX_NP_IN);
    EXPTR(cBigNumber*) powr    = EXPTRTO(cBigNumber*,
                                         powradix    [radix], MAX_NP_IN + 1);
    EXPTR(cBigNumber*) powrtab = EXPTRTO(cBigNumber*,
                                         powradixtab [radix], MAX_SMP_IN + 1);
    EXPTR(size_t)      powrexw = EXPTRTO(size_t,
                                         powradixexw [radix], MAX_SMP_IN + 1);

//      Обнуление стека множителей.

    _CBIGS cBigNumber num_;                     // Старший множитель.
    _CBIGS exvector<cBigNumber> stk_ (MAX_NP_IN);
    EXPTR(cBigNumber) mul = EXPTRTYPE(stk_);    // Стек множителей.
    { for (size_t i = 1; i < np; i++) mul [i] = 0; }

//      Прогресс - индикация.

    cTTY cProgress (cBigNumberProgress);
    size_t numlength = nr * sizeof (CBNL);
    if (numlength < cTTY_numscaleK) numlength = 0;
    if (numlength != 0 && np >= SHOW_NP_IN)
      cProgress.showScaled (cBigNumber::pszConverting, 0, (long)numlength);

//      Цикл по парным группам из nradix цифр.

    long lhigh = 0;                     // Старшее слово.
    if ((nr & 1) != 0) goto odd;        // Неполное слово - младшее.
    do
    {
//      Преобразование старшей группы из nradix цифр.

      {
        long lb = 0;                    // Слово.
        for(;;)
        {
          lb += chartonum [*ps++];
          assert (lb >= 0);
          if (--n == 0) break;
          lb *= radix;
        }
        n = nradix;
        lhigh = lb;                     // Старшее слово.
      }
      --nr;                             // Обработали группу.

//      Преобразование младшей группы из nradix цифр.

odd:  {
        long lb = 0;                    // Слово.
        for(;;)
        {
          lb += chartonum [*ps++];
          assert (lb >= 0);
          if (--n == 0) break;
          lb *= radix;
        }
        n = nradix;
        num_ = lb;                      // Младшее слово.
      }
      --nr;                             // Обработали группу.

//      Вычисление множителя первого порядка.

      num_.addmul (lhigh, lradix);      // Объединение слов.
      assert (num_ >= 0);

//      Вычисление множителей второго и последующих порядков
//      и накопление числа.

      size_t i = 1;
      for (;; i++)
      {
        if (i < np)                     // Вычисление множителей.
        {
          if ((nr & (2<<i)-1) != 0)     // Нечетный множитель
          {                             // запоминается
            num_.swap (mul [i]);        // в стеке.
            break;
          }                             // Иначе вычисляется множитель
                                        // более высокого порядка.
          if (i <= MAX_SMP_IN)
            num_.addmulsmp (*powrtab [i], mul [i], powrexw [i]);
          else
            num_.addmul (*powr [i], mul [i]);
          assert (num_ >= 0);
        }
        else                            // Накопление числа.
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

//      Число преобразовано.

  return *this;
}

//      Метод перевода строки в знаковое неограниченное число работает
//      для оснований от 2 до 16. Если основание не указано (0), то
//      основание 10 или 8 по префиксу 0 или 16 по префиксу 0x/0X.

cBigNumber& cBigNumber::set (           // Преобразование из строки.
                const char* psz,        // Строка.
                unsigned radix          // Основание (2..16) или 0.
            )                           // 0 - по соглашениям языка С.
{
  assert (psz);
  assert (radix != 1);
  assert (radix <= MAX_RADIX);
  int sign = 0;                         // Знак числа.
  unsigned c = *psz++;                  // Текущий символ.

//      Распознавание знака.

  switch (c)
  {
    case '-': sign = 1;
    case '+': c = *psz++;
  }

//      Распознавание основания.

  if (radix == 0)
  {
    radix = 10;
    if (c == '0')
    {
      c = *psz++; radix = 8;
      if (c == 'X' || c == 'x') { c = *psz++; radix = 16; }
    }
  }

//      Поиск конца числа.

  SqrTab (radix);                       // Заполнение chartonum.
  int width = 0;
  for (;;)
  {
    if ((size_t)chartonum [c] >= radix) break;
    c = psz [width++];                  // Следующий символ.
  }
  --psz;

//      Декодирование числа.

  setunsign (EXPTRTO(char, psz, width), radix, width);

//      Присвоение знака.

  if (sign) this->neg();

  return *this;
}

//================================================
//      Реализация перевода числа в строку.
//================================================

//      Метод перевода неограниченного числа для оснований от 2 до 16,
//      знак и основание слева, число справа.
//
//      Флаги форматирования:
//      cBigNumber_unsign     беззнаковый вывод
//      cBigNumber_showbase   добавить признак основания (0 или 0x)
//      cBigNumber_showpos    выводить знак для положительных чисел
//      cBigNumber_uppercase  прописные буквы для чисел с основанием > 10.

char*   cBigNumber::toa (               // Перевод в строку.
                cBigString& buf_,       // Расширяемый буфер.
                unsigned radix,         // Основание (2..16) или 0 (=10).
                size_t   width,         // Минимальное число символов.
                long     fill           // Символ-заполнитель и флаги.
        ) const                         // Выдает указатель на строку.
{
  assert (radix != 1);
  assert (radix <= MAX_RADIX);
  if (radix == 0) radix = 10;
  size_t i = 0;                                 // Индекс в буфере.
  _CBIGS cBigNumber num_;                       // Старшее частное.
  num_ = *this; num_.fit();                     // Нормализация.
  if (fill & cBigNumber_unsign) num_.unsign();  // Число без знака.

//      Выбор таблицы перекодировки цифр.

  register const EXPTR(char) ps = EXPTRTO (char,
                    numtochar [(fill & cBigNumber_uppercase) != 0],
                    sizeof (numtochar [0]));

//      Вывод знака и префиксов.

  {
    CBNL sign = num_.comp0();

    if (sign < 0)
    {
      buf_ [i++] = '-'; num_.neg();             // Вывод знака '-'.
    }
    else if (sign > 0 && (fill & cBigNumber_showpos))
    {
      buf_ [i++] = '+';                         // Вывод знака '+'.
    }

    if (radix % 8 == 0 && (fill & cBigNumber_showbase))
    {
      buf_ [i++] = '0';                         // Вывод основания.
      if (radix == 16) buf_ [i++] = ps [16];
    }
  }

//      Формирование числа в обратном порядке.
//
//      Исходный (неоптимизированный) алгоритм выглядит так:
//      do
//      {
//        buf_ [i++] = ps [num % radix];
//        num_ /= radix;
//      }
//      while (num_ != 0);
//
//      В оптимизированном алгоритме вместо деления применяется
//      метод setdivmodtab, выдающий частное и остаток за раз.
//      Для сокращения числа делений применяется
//      деление на коэффициенты powradix, полученные путем
//      последовательного возведения основания radix в квадрат.
//
//      В результате деления на старший коэффициент с индексом npmax
//      образуются частное и остаток, которые делятся на коэффициент
//      с индексом npmax-1, и т.п., пока в результате деления на
//      младший коэффициент с индексом 0 не образуются числа,
//      укладывающиеся в разрядную сетку типа long, которые преобразуются
//      в строку путем последовательного деления на radix.
//
//      Старшее частное хранится в num_, а младшие частные
//      в стеке частных div и помечаются в стеке признаков kdiv.
//      Признак равен 1, если в соответствующем элементе div
//      находится младшее частное. Первоначально массив частных пуст.
//      Для старшего частного в соответствующем по номеру элементе
//      массива kdiv всегда находится 0.
//
//      АЛГОРИТМ
//      Старшее частное делится с записью нового частного в num_ и
//      остатка в div [npmax - 1]; div [npmax - 1] делится c записью
//      частного в div [npmax - 1] и остатка в div [npmax - 2] и т.п.
//      Короткие остаток и частное от деления div [0] преобразуется в
//      строку; в ходе преобразования остатка частное хранится в div [0].
//      Частное div [1] переносится в div [0] и процесс повторяется
//      начиная c div [0], потом div [2] переносится в div [1] и
//      процесс повторяется c div [1], и т.п. вплоть до старшего частного,
//      когда производится уменьшение npmax и алгоритм повторяется заново.

  size_t j = i;                                 // Позиция первой цифры.
  {
//      Заполнение стека коэффициентов powradix [radix].
//      Выдается требуемое число коэффициентов.

    size_t np = SqrTab (radix, num_.length(), MAX_TAB_OUT);
    EXPTR(cBigNumber*) powrtab = EXPTRTO(cBigNumber*,
                                         powradixtab [radix], MAX_TAB_OUT + 1);
    EXPTR(size_t)      powrexw = EXPTRTO(size_t,
                                         powradixexw [radix], MAX_TAB_OUT + 1);

//      Рабочие стеки.

    _CBIGS cBigNumber mod_;                     // Остаток деления.
    _CBIGS exvector<cBigNumber> stk_ (MAX_TAB_OUT);
    _CBIGS exvector<int>       kstk_ (MAX_TAB_OUT);
    EXPTR(cBigNumber) div = EXPTRTYPE(stk_);    // Стек частных.
    EXPTR(int)       kdiv = EXPTRTYPE(kstk_);   // Стек признаков.

//      Прогресс - индикация.

    cTTY cProgress (cBigNumberProgress);
    size_t numlength = num_.length() * sizeof (CBNL);
    if (numlength < cTTY_numscaleK) numlength = 0;

//      Цикл по частным.
//      Преобразуются все кусочки числа, кроме самого последнего остатка.

    size_t npmax = np;                          // Индекс старшего частного.
    for (;;)                                    // Цикл по частным.
    {
      if (np == npmax)                          // Старшее частное.
      {
        if (numlength != 0 && np >= SHOW_NP_OUT)
          cProgress.showScaled (cBigNumber::pszFormatting,
            (long)(numlength - num_.length() * sizeof (long)),
            (long)numlength);

//        Старшее частное делится на соответствующий коэффициент.
//        Если результат деления нулевой и индекс старшего
//        делителя больше 0, то индекс старшего частного уменьшается
//        и цикл обработки старшего частного повторяется.

        num_.swap (mod_);                       // Запись частного в num_
        num_.setdivmodtab (mod_, *powrtab [np], // и остатка в mod_.
                                  powrexw [np]);

        if (num_.comp0() == 0)                  // Старшее частное закончилось,
        {                                       // остался старший остаток.
          if (np == 0) break;                   // Если это последний остаток,
                                                // то цикл заканчивается,
          num_.swap (mod_);                     // иначе остаток становится
          npmax = --np;                         // старшим частным,
          numlength = 0;                        // блокируется индикация
          assert (kdiv [np] == 0);              // и после контроля
          continue;                             // деление повторяется.
        }
      }
      else                                      // Поиск частного.
      {
        assert (np > 0);                        // Контроль.
        assert (kdiv [np - 1] == 0);            // Контроль.
        if (kdiv [np] == 0)                     // Если частного нет, то
        {                                       // переходим к следующему
          ++np; continue;                       // по старшинству.
        }                                       // Если частное найдено,
        mod_.swap (div [np]);                   // то загружаем div [np] и
        kdiv [np] = 0;                          // помечаем его свободным.
      }

//      Цикл деления остатка - пока индекс остатка больше 0,
//      он делится на соответствующий коэффициент.

      for (; np != 0; np--)
      {
        assert (kdiv [np - 1] == 0);            // Контроль.
        kdiv [np - 1] = 1;                      // Установка признака.
        div  [np - 1].setdivmodtab (mod_, *powrtab [np - 1],
                                           powrexw [np - 1]);
                                                // Запись частного в div
      }                                         // и остатка в mod_.

//      Цикл обработки младшего остатка и частного,
//      которые укладываются в пределы разрядной сетки long.
//      Младший остаток берется из mod_, а младшее частное - из div [0],
//      если оно там есть (это так всегда, кроме случая npmax == 0).

      {
        register long lnum;                     // Короткое целое.
        lnum = (long) mod_.loword();            // Вывод остатка.
        for (;;)                                // Цикл выполняется
        {                                       // до 2 раз.
          register int knum = (int) maxradix [radix] [1] - 1;
          do                                    // Вывод knum цифр.
          {
            buf_ [i++] = ps [ (int) (lnum % radix) ];
            lnum /= radix;                      // Вывод цифры.
          }
          while (--knum != 0);

          assert (lnum >= 0);                   // Контроль.
          assert (lnum < (long) radix);         // Контроль.
          buf_ [i++] = ps [(int)(lnum)];        // Последняя цифра.

          if (kdiv [0] == 0) break;             // Есть частное?
          lnum = (long) div [0].loword();       // Вывод частного.
          kdiv [0] = 0;                         // div [0] свободен.
        }
      }

//      Повторяем цикл, начиная с самого младшего частного.

      np = (npmax != 0);
    }                                           // Цикл по частным.

//      Вывод последнего остатка.

    {
      long lnum = (long) mod_.loword();
      if (lnum != 0)                            // Короткий остаток.
      do
      {
        buf_ [i++] = ps [(int) (lnum % radix)]; // Вывод цифры.
      }
      while ((lnum /= radix) != 0);
    }

//      Вывод нуля для нулевого числа.

    if (i == j) buf_ [i++] = ps [0];            // Число равно 0,
  }

//      Вывод символа-заполнителя и завершение строки.

  {
    if ((char)fill == '\0') fill = ps [0];      // Символ-заполнитель.
    while (i < width) buf_ [i++] = (char)fill;  // Вывод заполнителя.
    buf_ [i] = 0;                               // Строка готова.
    EXDEBUG (buf_.checkindex (i));
    i--;
  }

//      Перестановка символов.

  {
    register EXPTR(char) ps = EXPTRTYPE(buf_);
    while (i > j)
    {
      char c = ps [j]; ps [j] = ps [i]; j++; ps [i] = c; i--;
    }
  }

//      Число сформировано.

  return ((char*)(const char *)buf_);           // Выдаем строку.
}

//      Подлежащий исключению метод выдает указатель на статическую строку,
//      которая переписывается при каждом повторном вызове метода.

#ifdef  _CBIGNUM_TOATMP
char*   cBigNumber::toatmp (            // Перевод в строку.
                unsigned radix,         // Основание (2..16) или 0 (=10).
                size_t   width,         // Минимальное число символов.
                long     fill           // Символ-заполнитель и флаги.
        ) const                         // Выдает указатель на строку во
{                                       // внутреннем статическом буфере.
  static cBigString buf_;
  return toa (buf_, radix, width, fill);
}
#endif//_CBIGNUM_TOATMP

//================================================
//      Реализация вывода на консоль.
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
//      Шестнадцатеричный дамп CBNL без знака.
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
