/* --------------------------------------------------------------
    Целые числа с неограниченной разрядной сеткой (версия 2.0).
    Потоковый ввод-вывод.

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
//      Оператор потокового ввода.
//================================================

//      Форматирование ввода проводится на основе следующих флагов:
//      ios::skipws     пропустить начальные пробелы
//      ios::dec        число по основанию 10
//      ios::oct        число по основанию 8
//      ios::hex        число по основанию 16

_CIOSTD istream& operator >> (_CIOSTD istream& is, cBigNumber& b)
{
  assert (&is);
  assert (&b);
  long flags = is.flags();              // Флаги.
  unsigned radix = (                    // Основание.
         (flags & _CIOSTD ios::dec)? 10:
         (flags & _CIOSTD ios::oct)? 8:
         (flags & _CIOSTD ios::hex)? 16: 0);

//      Пропуск пробельных символов, если это предусмотрено
//      флагом ios::skipws и ввод первого символа.

  if (flags & _CIOSTD ios::skipws) is >> _CIOSTD ws;
  int c = is.get();                     // Текущий символ.

//      Распознавание знака.

  int sign = 0;                         // Знак числа.
  switch (c)
  {
    case '-': sign = 1;
    case '+': c = is.get();
  }
  if (radix == 0)                       // Распознавание основания.
  {
    radix = 10;
    if (c == '0')
    {
      c = is.get(); radix = 8;
      if (c == 'X' || c == 'x') { c = is.get(); radix = 16; }
    }
  }

//      Ввод цифр.

  cBigNumber::SqrTab (radix);           // Заполнение cBigNumber::chartonum.
  _CBIGS exarray<char> str_;            // Массив с цифрами.
  size_t width = 0;                     // Число цифр в массиве.
  for (;;)
  {
    if ((size_t)cBigNumber::chartonum [(unsigned char)c] >= radix)
      break;
    str_ [width++] = (char)c;
    c = is.get();                       // Следующий символ.
  }
  if (c != EOF) is.putback ((char)c);   // Возвращаем нераспознанный символ.

  str_ [width] = 0;                     // Конец строки.
  EXDEBUG (str_.checkindex (width));

//      Декодирование числа.

  b.setunsign (EXPTRTYPE(str_), radix, width);

//      Присвоение знака.

  if (sign) b.neg();

  return is;
}

//================================================
//      Оператор потокового вывода.
//================================================

//      Форматирование вывода проводится на основе следующих флагов:
//      ios::width()    ширина поля.
//      ios::fill()     символ-заполнитель.
//      ios::left       выравнивание влево.
//      ios::right      выравнивание вправо.
//      ios::internal   знак и основание слева, число справа.
//      ios::dec        знаковое число по основанию 10 (умолчание).
//      ios::oct        знаковое число по основанию 8.
//      ios::hex        знаковое число по основанию 16.
//      ios::showbase   добавить признак основания (0 или 0x).
//      ios::showpos    выводить знак для положительных чисел.
//      ios::uppercase  прописные буквы для чисел с основанием > 10.

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
