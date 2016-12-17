/* --------------------------------------------------------------
    Ìàêðîñû äëÿ ïîòîêîâîãî ââîäà-âûâîäà (âåðñèÿ 2.0).

    http://www.imach.uran.ru/cbignum

    Copyright 2005-2010, Ð.Í.Øàêèðîâ, ÈÌàø ÓðÎ PAH.
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
#ifndef _CIOS_H
#define _CIOS_H

#ifndef _CIOS_OLDSTREAM
#ifndef _CIOS_STDSTREAM

#ifdef  _MSC_VER
#if     _MSC_VER >= 1300
#define _CIOS_STDSTREAM
#endif//_MSC_VER
#endif//_MSC_VER

#ifdef  __GNUC__
#if     __GNUC__ >= 3
#define _CIOS_STDSTREAM
#endif/*__GNUC__*/
#endif/*__GNUC__*/

#ifndef _CIOS_STDSTREAM
#define _CIOS_OLDSTREAM
#endif//_CIOS_STDSTREAM

#endif//_CIOS_STDSTREAM
#endif//_CIOS_OLDSTREAM

#ifdef  _CIOS_STDSTREAM
#include <iostream>
#include <iomanip>
#define _CIOSTD std ::
#endif//_CIOS_STDSTREAM

#ifdef  _CIOS_OLDSTREAM
#include <iostream.h>
#include <iomanip.h>
#define _CIOSTD
#endif//_CIOS_OLDSTREAM

#endif//_CIOS_H
