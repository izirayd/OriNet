/* --------------------------------------------------------------
    Независимый от компилятора потоковый вывод (версия 2.0).

    http://www.imach.uran.ru/cbignum

    Copyright 2005-2010, Р.Н.Шакиров, ИМаш УрО PAH.
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
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "Cios.h"
#include "Ctty.h"
#define BITS    (CHAR_BIT * sizeof (CBNL))

#ifdef  _MSC_VER
#pragma auto_inline (off)
#endif/*_MSC_VER*/

/* --------------------------------------------------------------
    Опции препроцессора.
    _CTTY_R     выводить '\r' вместо '\b' для возврата курсора в
                cTTY_ProgressOutput
-------------------------------------------------------------- */

#if 0
#define _CTTY_R
#endif

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
    Вспомогательные функции.
-------------------------------------------------------------- */

static  const char*   pszOf1 = " of";           /* Префикс для max */
static  const char*   pszOf2 = "";              /* Суффикс для max */
static  size_t        nOf    = 3;               /* Длина префикса и суффикса */

void    cTTY_SetMaxOutput (const char* psz1, const char* psz2)
{
  nOf = strlen (pszOf1 = psz1) + strlen (pszOf2 = psz2);
}

char* cTTY_ltoa10 (CBNL num, char* buf)         /* Преобразование в строку */
{                                               /* Нужен достаточный буфер */
  char *b = buf;
  if (num < 0) { *b++ = '-'; num = -num; }
  {
    char *p = b;

    do *p++ = (char) ((int) (num % 10) + '0');  /* Пишем цифру */
    while ((num /= 10) > 0);                    /* Запоминаем цифру */

    *p-- = '\0';                                /* Завершаем строку */

    do { char c = *p; *p-- = *b; *b++ = c; }    /* Переставляем цифры */
    while (b < p);                              /* до половины */
  }
  return buf;
}

/* --------------------------------------------------------------
    Поддержка выводя для C++
-------------------------------------------------------------- */

size_t  cTTY_PutStream (_CIOSTD ostream& os,    /* Вывод в поток */
                        const   char*   psz,    /* Строка или NULL */
                                CBNL    num,    /* Число */
                                CBNL    max,    /* Максимум или 0 */
                                long    cop)    /* Опции или символ, не \r */
{                                               /* Выдает размер строки */
  size_t n = 0;                                 /* после последнего \n */
  unsigned width = (unsigned) os.width (0);     /* Обнуление ширины */
/*
        Вывод строки.
*/
  if (psz != NULL) { n = strlen (psz); os << psz; }
/*
        Вывод числа.
*/
  char c = 0;                                   /* Суффикс */
  if ((num | max | (cop & cTTY_numflags)) != 0L)
  {
    if ((cop & cTTY_numpercent) != 0L)
    {                                           /* Процентное отношение */
      if (max != 0 && max != 100) num = (num * 100 + max/2) / max;
      max = 0; c = '%';
    }
    else
    if ((cop & cTTY_numscale) != 0L)            /* Масштабирование */
    {
      CBNL abs = num;
      if (abs < 0) abs = -abs;
      if (abs < max) abs = max;
      if (abs >= 0x2600) { num >>=  9; max >>=  9; c = 'K'; abs >>=  9;
      if (abs >= 0x4C00) { num >>= 10; max >>= 10; c = 'M'; abs >>= 10;
      if (abs >= 0x4C00) { num >>= 10; max >>= 10; c = 'G'; abs >>= 10;
      if (abs >= 0x4C00) { num >>= 10; max >>= 10; c = 'T'; abs >>= 10;
      if (abs >= 0x4C00) { num >>= 10; max >>= 10; c = 'P'; }}}}
                           ++num /= 2; ++max /= 2; }
    }                                           /* 0x2600 = 10240-512 */

    long unit = os.setf (_CIOSTD ios::unitbuf) & _CIOSTD ios::unitbuf;
    char fill = os.fill (' ');                  /* Флаг и заполнитель */
    char buf [BITS + 8];                        /* Буфер для преобразования */

    {                                           /* Вывод num */
      size_t nn = strlen (cTTY_ltoa10 (num, buf));
      nn += cTTY_getnumspace (cop);
      size_t mm = cTTY_getnumwidth (cop);
      if (psz != NULL)
      {
        size_t m = cTTY_getstrwidth (cop) + mm;
        if (n + nn < m) nn = m - n;
      }
      if (nn < mm) nn = mm;
      n += nn;
      os << _CIOSTD setw ((unsigned)nn) << buf;
    }

    if (max != 0L)                              /* Вывод ненулевого max */
    {
      size_t nn = strlen (cTTY_ltoa10 (max, buf));
      nn += cTTY_getnumspace (cop);
      size_t mm = cTTY_getnumwidth (cop);
      if (nn < mm) nn = mm;
      n += nn + nOf;
      os << pszOf1 << _CIOSTD setw ((unsigned)nn) << buf << pszOf2;
    }

    if (unit == 0) os.unsetf (_CIOSTD ios::unitbuf);
    os.fill (fill);                             /* Сброс флага и заполнителя */

    if (c != 0) { ++n; os << c; }               /* Вывод суффикса */
  }
/*
        Вывод символа.
*/
  c = (char) cop;
  if (c != 0 && c != '\r') { ++n; os << c; }    /* Вывод символа */

  os.width (width);                             /* Восстановление ширины */
  return n;
}

void    cTTY_CleanStream (_CIOSTD ostream& os,  /* Вывод пробелов и забоев */
                                size_t  nSpace, /* Число пробелов */
                                size_t  nBS)    /* Число забоев */
{
  long unit = os.setf (_CIOSTD ios::unitbuf) & _CIOSTD ios::unitbuf;
  char fill = os.fill (' ');                    /* Флаг и заполнитель */
  unsigned width = (unsigned) os.width (0);     /* Обнуление ширины */

  for (; nSpace > 0; nSpace--) os << ' ';       /* Выводим отдельные символы */
#ifndef _CTTY_R
  for (; nBS    > 0; nBS--   ) os << '\b';      /* из-за странности gcc 2.96 */
#else //_CTTY_R
  if (nBS) { nBS = 0; os << '\r'; }
#endif//_CTTY_R
  os << _CIOSTD flush;

  if (unit == 0) os.unsetf (_CIOSTD ios::unitbuf);
  os.fill (fill);                               /* Сброс флага и заполнителя */
  os.width (width);                             /* Восстановление ширины */
}

void   cTTY_FlushStream (_CIOSTD ostream& os)   /* Сброс потока */
{
  os << _CIOSTD flush;
}

#define cTTY_out        _CIOSTD cout
#define cTTY_err        _CIOSTD cerr

/* --------------------------------------------------------------
    Реализация функций-обработчиков.
-------------------------------------------------------------- */

static  void*   pWidth  = NULL;                 /* Объект стандарт. вывода */
static  size_t  nWidth  = 0;                    /* Ширина стандарт. вывода */
static  void*   pChars  = NULL;                 /* Объект прогресс-вывода */
static  size_t  nChars  = 0;                    /* Ширина прогресс-вывода */
static  size_t  nClean  = 0;                    /* Очистка прогресс-вывода */

int     cTTY_StandardOutput    (void*   pt,     /* Объект или NULL */
                        const   char*   psz,    /* Строка или NULL */
                                CBNL    num,    /* Число */
                                CBNL    max,    /* Максимум или 0 */
                                long    cop)    /* Опции и символ */
{                                               /* Выдает 0 */
  while (nClean != 0) cTTY_ProgressOutput (NULL, NULL, 0, 0, '\r');
                                                /* Очистка прогресс-вывода */
  const char *p = NULL;
  if (psz)
  {
    if ((p = strrchr (psz, '\r')) != NULL)
    do
      cTTY_StandardOutput (pt, NULL, 0, 0, *psz);
    while (psz++ != p);

    if ((p = strrchr (psz, '\n')) != NULL)
    {
      nWidth = (size_t)(psz - p - 1);
    }
  }

  size_t n = cTTY_PutStream (cTTY_out, psz, num, max, cop);
  if (n != 0) { pWidth = pt; nWidth += n; }

  switch ((char)cop)
  {
    case '\r':  if (nWidth == 0 || pWidth != pt && pt != NULL) break;
                cTTY_PutStream (cTTY_out, NULL, 0, 0, '\n');
    case '\n':  n = nWidth = 0;
    default:    if (n == 0 || p != NULL) cTTY_FlushStream (cTTY_out);
  }

  return 0;
}

int     cTTY_ProgressOutput    (void*   pt,     /* Объект или NULL */
                        const   char*   psz,    /* Строка или NULL */
                                CBNL    num,    /* Число */
                                CBNL    max,    /* Максимум или 0 */
                                long    cop)    /* Опции и символ */
{                                               /* Выдает 0 */
  const char *p = NULL;
  if (psz)
  {
    if ((p = strrchr (psz, '\r')) != NULL)
    do
      cTTY_ProgressOutput (pt, NULL, 0, 0, *psz);
    while (psz++ != p);

    if ((p = strrchr (psz, '\n')) != NULL)
    {
      nWidth = 0; nChars = (size_t)(psz - p - 1); nClean = 0;
    }
    else
    {
      size_t n = strlen (psz);
      if (n + nChars > 79)
      {
        psz += n;
        if (nChars <= 77)
        {
          psz += nChars - 77;
          nChars += cTTY_PutStream (cTTY_err, "..", 0, 0, 0);
          pChars = pt;
        }
      }
    }
  }

  size_t n = cTTY_PutStream (cTTY_err, psz, num, max, cop);
  if (n != 0) { pChars = pt; nChars += n; }

  switch ((char)cop)
  {
    case '\r': if (nClean < nChars) nClean = nChars;
               if (nClean == 0 || pChars != pt && pt != NULL) break;
               cTTY_CleanStream (cTTY_err, nClean - nChars, nClean);
               nClean = nChars; nChars = 0;     /* Store length of line */
               if (nClean > 79) nClean = 0;
               break;
    case '\n': n = nWidth = nChars = nClean = 0;
    default:   if (n == 0 || p != NULL) cTTY_FlushStream (cTTY_err);
  }

  return 0;
}

#ifdef  __cplusplus
}
#endif/*__cplusplus */
