/* --------------------------------------------------------------
    Строковые операции с проверкой индекса (версия 1.2c).

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
#ifndef EXSTRING_H
#define EXSTRING_H

#ifndef EXARRAY_H
#include "Exarray.h"
#endif/*EXARRAY_H*/

#include<string.h>

/* ==============================================================
    Реализация строковых функций string.h для динамических
    массивов exarray<char> и указателей expoint<char>.

    Поскольку динамические массивы и указатели автоматически
    преобразуются к константным указателям, то переопределять
    требуется только функции, копирующие данные в динамические
    массивы.

    Переопределение выражается в добавлении операторов,
    управляющих размером массивов.
============================================================== */

/*
    Обходим ошибку в Borland C++ (Release mode)
*/
#ifdef  __TURBOC__
#pragma warn -ill
#pragma intrinsic -memcpy
#pragma intrinsic -memmove
#pragma intrinsic -memset
#pragma intrinsic -strcpy
#pragma intrinsic -strcat
#pragma intrinsic -stpcpy
#pragma intrinsic -strncpy
#pragma intrinsic -strncat
#pragma intrinsic -strxfrm
#pragma intrinsic -memccpy
#pragma intrinsic -strset
#pragma intrinsic -strnset
#pragma warn .ill
#endif/*__TURBOC__ */

inline  expoint<char>
memcpy  (expoint<char> dest, const void* src, size_t n)
{
    dest.expand (n, (const char**)&src);
    memcpy (dest.base(), src, n);
    return (dest);
}
inline  exarray<char>&
memcpy  (exarray<char>&dest, const void* src, size_t n)
{
    dest.expand (n, (const char**)&src);
    memcpy (dest.base(), src, n);
    return (dest);
}

inline  expoint<char>
memmove (expoint<char> dest, const void* src, size_t n)
{
    dest.expand (n, (const char**)&src);
    memmove (dest.base(), src, n);
    return (dest);
}
inline  exarray<char>&
memmove (exarray<char>&dest, const void* src, size_t n)
{
    dest.expand (n, (const char**)&src);
    memmove (dest.base(), src, n);
    return (dest);
}

inline  expoint<char>
memset  (expoint<char> s, int c, size_t n)
{
    s.expand (n);
    memset (s.base(), c, n);
    return (s);
}
inline  exarray<char>&
memset  (exarray<char>&s, int c, size_t n)
{
    s.expand (n);
    memset (s.base(), c, n);
    return (s);
}

inline  expoint<char>
strcpy  (expoint<char> dest, const char* src)
{
    return (memcpy (dest, src, strlen (src) + 1));
}
inline  exarray<char>&
strcpy  (exarray<char>&dest, const char* src)
{
    return (memcpy (dest, src, strlen (src) + 1));
}

inline  expoint<char>
strcat  (expoint<char> dest, const char* src)
{
    expoint<char> pdest (dest);
    strcpy (pdest += strlen (dest), src);
    return (dest);
}
inline  exarray<char>&
strcat  (exarray<char>&dest, const char* src)
{
    expoint<char> pdest (dest);
    strcpy (pdest += strlen (dest), src);
    return (dest);
}

inline  expoint<char>
stpcpy  (expoint<char> dest, const char* src)
{
    return (strcpy(dest, src) + strlen (src));
}
inline  expoint<char>
stpcpy  (exarray<char>&dest, const char* src)
{
    return (expoint<char>(strcpy(dest, src), strlen (src)));
}

inline  expoint<char>
strncpy (expoint<char> dest, const char* src, size_t maxlen)
{
    dest.expand (maxlen, &src);
    strncpy (dest.base(), src, maxlen);
    return (dest);
}
inline  exarray<char>&
strncpy (exarray<char>&dest, const char* src, size_t maxlen)
{
    dest.expand (maxlen, &src);
    strncpy (dest.base(), src, maxlen);
    return (dest);
}

inline  expoint<char>
strncat (expoint<char> dest, const char* src, size_t maxlen)
{
    dest.expand (exadd (strlen (dest) + 1, maxlen), &src);
    strncat (dest.base(), src, maxlen);
    return (dest);
}
inline  exarray<char>&
strncat (exarray<char>&dest, const char* src, size_t maxlen)
{
    dest.expand (exadd (strlen (dest) + 1, maxlen), &src);
    strncat (dest.base(), src, maxlen);
    return (dest);
}

inline  size_t
strxfrm (expoint<char> dest, const char* src, size_t maxlen)
{
    dest.expand (maxlen);
    return (strxfrm (dest.base(), src, maxlen));
}
inline  size_t
strxfrm (exarray<char>&dest, const char* src, size_t maxlen)
{
    dest.expand (maxlen);
    return (strxfrm (dest.base(), src, maxlen));
}

#ifndef __STDC__

inline  expoint<char>
memccpy (expoint<char> dest, const void* src, int c, size_t n)
{
    dest.expand (n, (const char**)&src);
    char* p = (char*) memccpy (dest.base(),src, c, n);
    expoint<char> pdest;
    if (p) { pdest = dest;
             pdest += ((size_t)p - (size_t)dest.base()); }
    return (pdest);
}
inline  expoint<char>
memccpy (exarray<char>&dest, const void* src, int c, size_t n)
{
    dest.expand (n, (const char**)&src);
    char* p = (char*) memccpy (dest.base(),src, c, n);
    expoint<char> pdest;
    if (p) { pdest = dest;
             pdest += ((size_t)p - (size_t)dest.base()); }
    return (pdest);
}

inline  expoint<char>
strnset (expoint<char> s, int ch, size_t n)
{
    s.expand (n);
    strnset (s.base(), ch, n);
    return (s);
}
inline  exarray<char>&
strnset (exarray<char>&s, int ch, size_t n)
{
    s.expand (n);
    strnset (s.base(), ch, n);
    return (s);
}

inline  expoint<char>
strset  (expoint<char> s, int ch)
{
    strnset (s.base(), ch, s.size());
    return (s);
}
inline  exarray<char>&
strset  (exarray<char>&s, int ch)
{
    strnset (s.base(), ch, s.size());
    return (s);
}

#endif/*__STDC__*/

#endif/*EXSTRING_H*/
