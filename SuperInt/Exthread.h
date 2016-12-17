/* --------------------------------------------------------------
    Ìàêðî äëÿ ëîêàëüíîé ïàìÿòè ïîòîêà (âåðñèÿ 1.2c).

    http://www.imach.uran.ru/exarray

    Copyright 2010, Ð.Í.Øàêèðîâ, ÈÌàø ÓðÎ PAH.
    All Rights Reserved.

    Êîä ïðåäîñòàâëÿåòñÿ íà óñëîâèÿõ ëèöåíçèè "public domain",
    ò.å. äîïóñêàåòñÿ ñâîáîäíîå ïðèìåíåíèå, ìîäèôèêàöèÿ è
    ðàñïðîñòðàíåíèå êîäà ïðè óñëîâèè ñîõðàíåíèÿ êîïèðàéòà
    è äîáàâëåíèÿ ñâåäåíèé î âñåõ âíåñåííûõ èçìåíåíèÿõ.

    ÄÀÍÍÎÅ ÏÐÎÃÐÀÌÌÍÎÅ ÎÁÅÑÏÅ×ÅÍÈÅ ÏÐÅÄÎÑÒÀÂËßÅÒÑß "ÊÀÊ ÅÑÒÜ"
    ÁÅÇ ÊÀÊÈÕ-ËÈÁÎ ÃÀÐÀÍÒÈÉ, ßÂÍÛÕ ÈËÈ ÏÎÄÐÀÇÓÌÅÂÀÅÌÛÕ. ÂÛ
    ÈÑÏÎËÜÇÓÅÒÅ ÅÃÎ ÍÀ ÑÂÎÉ ÑÎÁÑÒÂÅÍÍÛÉ ÑÒÐÀÕ È ÐÈÑÊ. ÀÂÒÎÐ
    ÍÅ ÁÓÄÅÒ ÍÅÑÒÈ ÎÒÂÅÒÑÒÂÅÍÍÎÑÒÜ ÇÀ ÓÒÐÀÒÓ ÄÀÍÍÛÕ, ÓÙÅÐÁ,
    ÓÁÛÒÊÈ ÈËÈ ËÞÁÛÅ ÄÐÓÃÈÅ ÂÈÄÛ ÏÎÒÅÐÜ ÏÐÈ ËÞÁÛÕ ÑÏÎÑÎÁÀÕ
    ÈÑÏÎËÜÇÎÂÀÍÈß ÄÀÍÍÎÃÎ ÏÐÎÃÐÀÌÌÍÎÃÎ ÎÁÅÑÏÅ×ÅÍÈß.
-------------------------------------------------------------- */
#ifndef EXTHREAD_H
#define EXTHREAD_H

/* --------------------------------------------------------------
    EXTHREAD_LOCAL  - Ïðåôèêñ ëîêàëüíîé ïàìÿòè ïîòîêà.
-------------------------------------------------------------- */

/*
    Compatible C++ 0x
*/
#if 0
#ifndef EXTHREAD_LOCAL
#ifdef  __cplusplus
#if     __cplusplus >= 200801L
#define EXTHREAD_LOCAL  thread_local
#endif/*__cplusplus*/
#endif/*__cplusplus*/
#endif/*EXTHREAD_LOCAL*/
#endif

/*
    Borland C++
*/
#if 1
#ifndef EXTHREAD_LOCAL
#ifdef  __TURBOC__
#ifdef  __MT__
#if     __TURBOC__ >= 0x460
#define EXTHREAD_LOCAL  __thread
#endif/*__TURBOC__*/
#else /*__MT__*/
#define EXTHREAD_LOCAL
#endif/*__MT__*/
#endif/*__TURBOC__*/
#endif/*EXTHREAD_LOCAL*/
#endif

/*
    Visual C++, Intel C/C++ for Windows
*/
#if 1
#ifndef EXTHREAD_LOCAL
#ifdef  _MSC_VER
#ifdef  _MT
#if     _MSC_VER >= 800
#define EXTHREAD_LOCAL  __declspec(thread)
#endif/*_MSC_VER*/
#else/* _MT*/
#define EXTHREAD_LOCAL
#endif/*_MT*/
#endif/*_MSC_VER*/
#endif/*EXTHREAD_LOCAL*/
#endif

/*
    GNU gcc, Intel C/C++ for Linux
*/
#if 1
#ifndef EXTHREAD_LOCAL
#ifdef  __GNUC__
#if defined(_REENTRANT) || defined(_LIBC_REENTRANT)
#if     __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3)
#define EXTHREAD_LOCAL  __thread
#endif/*__GNUC__*/
#else /*_REENTRANT*/
#define EXTHREAD_LOCAL
#endif/*_REENTRANT*/
#endif/*__GNUC__*/
#endif/*EXTHREAD_LOCAL*/
#endif

/*
    Sun Studio C/C++
*/
#if 0
#ifndef EXTHREAD_LOCAL
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#define EXTHREAD_LOCAL  __thread
#endif/*__SUNPRO*/
#endif/*EXTHREAD_LOCAL*/
#endif

/*
    IBM XL C/C++ for Linux and AIX with -qtls option
*/
#if 0
#ifndef EXTHREAD_LOCAL
#ifdef  __xlC__
#define EXTHREAD_LOCAL  __thread
#endif/*__xlC__*/
#endif/*EXTHREAD_LOCAL*/
#endif

#endif/*EXTHREAD_H*/
