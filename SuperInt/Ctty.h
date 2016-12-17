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
#ifndef _CTTY_H
#define _CTTY_H

#ifndef _CBNL_H
#include "Cbnl.h"
#endif//_CBNL_H

/* --------------------------------------------------------------
    Опции.
-------------------------------------------------------------- */

#define cTTY_reserved1          0x80000000L
#define cTTY_reserved2          0x40000000L
#define cTTY_strwidth(n)        (((long)(n) & 0x3F) << 24)
#define cTTY_numspace           0x00800000L
#define cTTY_numscale           0x00400000L
#define cTTY_numpercent         0x00200000L
#define cTTY_numwidth(n)        (((long)(n) & 0x1F) << 16)

#define cTTY_getstrwidth(n)     ((size_t)((long)(n) >> 24) & 0x3F)
#define cTTY_getnumspace(n)     ((size_t)((long)(n) >> 23) & 1)
#define cTTY_getnumscale(n)     ((size_t)((long)(n) >> 22) & 1)
#define cTTY_getnumpercent(n)   ((size_t)((long)(n) >> 21) & 1)
#define cTTY_getnumwidth(n)     ((size_t)((long)(n) >> 16) & 0x1F)
#define cTTY_numflags           0x00FF0000L
#define cTTY_charmask           0x0000FFFFL

#define cTTY_commentwidth       cTTY_strwidth (12)
#define cTTY_numscalewidth      cTTY_numwidth (5)
#define cTTY_numpercentwidth    cTTY_numwidth (4)

#define cTTY_numscaleK          0x2600L
#define cTTY_numscaleM          (cTTY_numscaleK << 10)
#define cTTY_numscaleG          (cTTY_numscaleK << 20)
#define cTTY_numscaleT          (cTTY_numscaleK << 30)
#define cTTY_numscaleP          (cTTY_numscaleK << 40)

/* --------------------------------------------------------------
    Внешние ссылки объявлены, как "C", чтобы
    заблокировать преобразование имен.
    Это позволяет переносить объектные модули
    между различными версиями компиляторов.
-------------------------------------------------------------- */

#ifdef  __cplusplus
extern "C" {
#endif/*__cplusplus */

/* --------------------------------------------------------------
    Вспомогательные функции.
-------------------------------------------------------------- */

extern  void    cTTY_SetMaxOutput (const char* psz1, const char* psz2);
extern  char*   cTTY_ltoa10       (CBNL num, char* buf);

/* --------------------------------------------------------------
    Функции - обработчики.
-------------------------------------------------------------- */

extern  int     cTTY_StandardOutput (void*, const char*, CBNL, CBNL, long);
extern  int     cTTY_ProgressOutput (void*, const char*, CBNL, CBNL, long);
typedef int     (*cTTY_h)           (void*, const char*, CBNL, CBNL, long);

#ifdef  __cplusplus
}
#endif/*__cplusplus */

/* --------------------------------------------------------------
    Класс C++ для потокового вывода.
-------------------------------------------------------------- */

#ifdef  __cplusplus

class cTTY
{
  cTTY_h ph;

public:

  cTTY& put (const char* psz, CBNL num, CBNL max = 0, long cop = cTTY_numspace)
  {
    if (ph) (*ph) (this, psz, num, max, cop);
    return *this;
  }

  cTTY& putSpaced (const char* psz, CBNL num, CBNL max = 0)
  {
    return put (psz, num, max, cTTY_commentwidth | cTTY_numspace);
  }

  cTTY& putScaled (const char* psz, CBNL num, CBNL max = 0)
  {
    return put (psz, num, max, cTTY_commentwidth | cTTY_numscalewidth
                                                 | cTTY_numscale);
  }

  cTTY& putPercent (const char* psz, CBNL num, CBNL max = 100)
  {
    return put (psz, num, max, cTTY_commentwidth | cTTY_numpercentwidth
                                                 | cTTY_numpercent);
  }

  void show (const char* psz, CBNL num, CBNL max = 0, long cop = cTTY_numspace)
  {
    if (ph) (*ph) (this, psz, num, max, cop | '\r');
  }

  void showSpaced (const char* psz, CBNL num, CBNL max = 0)
  {
    show (psz, num, max, cTTY_commentwidth | cTTY_numspace);
  }

  void showScaled (const char* psz, CBNL num, CBNL max = 0)
  {
    show (psz, num, max, cTTY_commentwidth | cTTY_numscalewidth
                                           | cTTY_numscale);
  }

  void showPercent (const char* psz, CBNL num, CBNL max = 100)
  {
    show (psz, num, max, cTTY_commentwidth | cTTY_numpercentwidth
                                           | cTTY_numpercent);
  }

public:
  cTTY()                                { ph = 0;                       }
  cTTY (const cTTY& os)                 { ph = os.ph;                   }
  cTTY (const cTTY_h p)                 { ph = p;                       }
  cTTY& operator = (const cTTY& os)     { ph = os.ph; return *this;     }
  cTTY& operator = (const cTTY_h p)     { ph = p;     return *this;     }
  operator cTTY_h     () const          { return ph;                    }
  int      operator ! () const          { return (ph == 0);             }
  ~cTTY()                               { put (0, 0, 0, '\r');          }
};

inline cTTY& operator << (cTTY& os, const char* psz)
{
  return os.put (psz, 0, 0, 0);
}

#if CBNL_MAX != LONG_MAX
inline cTTY& operator << (cTTY& os, CBNL num)
{
  return os.put (0, num, 0, cTTY_numwidth (1));
}
#endif

inline cTTY& operator << (cTTY& os, long num)
{
  return os.put (0, num, 0, cTTY_numwidth (1));
}

inline cTTY& operator << (cTTY& os, int num)
{
  return os.put (0, num, 0, cTTY_numwidth (1));
}

inline cTTY& operator << (cTTY& os, short num)
{
  return os.put (0, num, 0, cTTY_numwidth (1));
}

inline cTTY& operator << (cTTY& os, char c)
{
  return os.put (0, 0, 0, c);
}

inline void operator <<= (cTTY& os, const char* psz)
{
  os.put (psz, 0, 0, '\r');
}

#if CBNL_MAX != LONG_MAX
inline void operator <<= (cTTY& os, CBNL num)
{
  os.put (0, num, 0, cTTY_numwidth (1) | '\r');
}
#endif

inline void operator <<= (cTTY& os, long num)
{
  os.put (0, num, 0, cTTY_numwidth (1) | '\r');
}

inline void operator <<= (cTTY& os, int num)
{
  os.put (0, num, 0, cTTY_numwidth (1) | '\r');
}

inline void operator <<= (cTTY& os, short num)
{
  os.put (0, num, 0, cTTY_numwidth (1) | '\r');
}

inline void operator <<= (cTTY& os, char c)
{
  char s [2]; s [0] = c; s [1] = 0;
  os.put (s, 0, 0, '\r');
}

#endif/*__cplusplus */

#endif/*_CTTY_H */
