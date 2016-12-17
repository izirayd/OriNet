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
#ifndef _CBIGNUM_H
#define _CBIGNUM_H

#ifndef __cplusplus
#error  Must use C++ for the type cBigNumber.
#endif//__cplusplus

#ifndef _CBNL_H
#include "Cbnl.h"
#endif//_CBNL_H

#ifndef _CTTY_H
#include "Ctty.h"
#endif//_CTTY_H

#ifndef EXARRAY_H
#include "Exarray.h"
#endif//EXARRAY_H

#ifndef EXDEBUG_H
#include "Exdebug.h"
#endif//EXDEBUG_H

//================================================
//      Поддержка многопоточности и DLL.
//      _CBIGNUM_ST         использовать нереентерабельные оптимизации.
//      _CBIGNUM_MT         исключить нереентерабельные методы.
//      EXTHREAD_LOCAL      префикс локальной памяти потока.
//================================================

#if 1
#define _CBIGNUM_MT
#endif

#if 0
#define _CBIGNUM_TOATMP
#endif

#ifndef _CBIGNUM_MT
#define _CBIGNUM_TOATMP
#endif//_CBIGNUM_MT

#ifndef EXTHREAD_LOCAL
#ifndef _CBIGNUM_MT
#define EXTHREAD_LOCAL
#else //_CBIGNUM_MT
#include "Exthread.h"
#endif//_CBIGNUM_MT
#endif//EXTHREAD_LOCAL

//================================================
//      Опции распределения памяти и контроля индексов.
//      _CBIGNUM_DEF_ALLOC  выделять память в конструкторе по умолчанию,
//                          как это делалось в версиях 1.x класса.
//      _CBIGNUM_NCHECKPTR  не проверять индексы во входных массивах.
//================================================

#if 0
#define _CBIGNUM_DEF_ALLOC
#endif

#if 0
#define _CBIGNUM_NCHECKPTR
#endif

#ifdef  _CBIGNUM_NCHECKPTR

#define CBPTR(T)        T*
#define CBPTRTYPE(m)    ((m).base())
#define CBPTRBASE(p)    (p)
#define CBPTRINDEX(p,i) (p)

#else //_CBIGNUM_NCHECKPTR

#define CBPTR(T)        EXPTR(T)
#define CBPTRTYPE(m)    EXPTRTYPE(m)
#define CBPTRBASE(p)    EXPTRBASE(p)
#define CBPTRINDEX(p,i) EXPTRINDEX(p,i)

#endif//_CBIGNUM_NCHECKPTR

//================================================
//      Представление чисел cBigNumber.
//================================================
//      Неограниченное целое число хранится в массиве типа CBNL.
//      Элемент массива с нулевым индексом содержит число CBNL-слов,
//      требуемых для представления неограниченного числа в
//      дополнительном двоичном коде, в последующих элементах
//      содержится код числа в порядке от младших разрядов к
//      старшим. Знак числа находится в последнем слове.
//      Информация о текущем размере числа выдается методом length().
//
//      Для записи дополнительного кода используется минимально
//      возможное число CBNL-слов, но не менее одного слова:
//      - если число слов больше 1, то старшее слово отличается от
//        результата расширения старшего бита предпоследнего слова;
//      - нуль представляется одним нулевым словом.
//      Такое представление мы будем называть нормализованным.
//
//      Условие нормализованности не является обязательным для
//      правильного выполнения операций, т.е. допускается любое
//      число CBNL-слов, начиная с 0, лишь бы дополнительный код
//      представлял требуемое значение. Выдача нормализованного
//      результата обеспечивается, в общем случае, только при
//      условии нормализованности всех исходных чисел.
//
//      Для управления размером массива применяется класс exblock<CBNL>
//      объекты которого содержат ссылку на массив с длинным числом,
//      а также поле с общим числом CBNL-слов, доступных для записи
//      длинного числа. Число доступных слов может быть больше, чем
//      число занятых слов - это требуется для выполнения арифметических
//      операций, которые могут потребовать увеличения размера числа.
//
//      Для управления размером применяются следующие методы:
//      checkexpand(n) - увеличить число доступных слов с резервированием
//                       памяти для дальнейшего расширения массива.
//      checkshrink(n) - увеличить число доступных слов с резервированием
//                       памяти для дальнейшего расширения массива.
//      checkindex(n)  - уменьшить число доступных слов без освобождения
//                       занятой массивом памяти.
//      Число слов указывается без учета слова с нулевым индексом.
//
//      После вызова методов checkexpand() и checkindex() под массив может
//      быть выделен больший объем памяти, чем требуется для размещения
//      указанного числа слов. Незадействованная память применяется как
//      резерв для дальнейшего увеличения размера числа.
//
//      Если определено макро NCHECKPTR, то метод checkexpand() увеличивает
//      указанное число доступных слов до величины, определяемой объемом
//      выделенной под массив памяти, а метод checkindex() не делает ничего.
//
//      Если макро NCHECKPTR не определено, то методы checkexpand() и
//      checkindex() устанавливают число доступных слов равным указанному.
//
//      Иерархия классов:
//
//      exblock<CBNL>           // Массив переменой длины.
//      |
//      +- cBigNumber           // Неограниченное число.
//         |
//         +- cBigTemp          // Неограниченное число в стеке.
//            |
//            +- cBigInc        // Постфиксный инкремент.
//            +- cBigDec        // Постфиксный декремент.
//            +- cBigPos        // Сохранение знака.
//            +- cBigNeg        // Отрицание.
//            +- cBigAbs        // Абсолютное значение.
//            +- cBigUnsign     // Беззнаковое значение.
//            +- cBigCompl      // Побитовая инверсия.
//            +- cBigXor        // Побитовая сумма по модулю 2.
//            +- cBigAnd        // Побитовая конъюнкция.
//            +- cBigOr         // Побитовая дизъюнкция.
//            +- cBigAdd        // Сложение.
//            +- cBigSub        // Вычитание.
//            +- cBigMul        // Умножение.
//            +- cBigDiv        // Деление.
//            +- cBigDivMod     // Деление с остатком.
//            +- cBigMod        // Остаток.
//            +- cBigShl        // Сдвиг влево.
//            +- cBigShr        // Сдвиг вправо.
//            +- cBigPow        // Степень.
//            +- cBigPowMod     // Степень по модулю.
//            +- cBigSqrt       // Квадратный корень.
//            +- cBigSqrtRm     // Квадратный корень с остатком.
//            +- cBigBits       // Число значащих бит.
//            +- cBigExBits     // Число значащих младших 0-бит.
//            +- cBigRandom     // Случайное значение.

//================================================
//      Typedef для преобразования числа в строку.
//================================================

typedef exarray<char> cBigString;

//================================================
//      Основной класс cBigNumber.
//================================================

class cBigNumber: public exblock<CBNL>
{
protected:                      // Оптимизации.
/*
  void  create ()               // Применяется вместо вызова checkexpand (1),
  {                             // если память еще не была распределена.
    e = (CBNL *) exmalloc (EXCALCBLOCKSIZE_1);
    len = max_size (EXCALCBLOCKSIZE_1);
  }
*/
  void  create (CBNL b = 0)     // Создание числа минимального размера
  {                             // с заданным начальным значением.
#ifndef _CBIGNUM_DEF_ALLOC
    if (b == 0) { e = stub(); len = 0; } else
#endif//_CBIGNUM_DEF_ALLOC
    {
      e = (CBNL *) exmalloc (EXCALCBLOCKSIZE_1);
      len = max_size (EXCALCBLOCKSIZE_1);
      e [0] = 1; e [1] = b;
      checkindex (1);
    }
  }

  cBigNumber (exblockstack_t) {}// Пустой конструктор для cBigTemp.

public:

//      Конструкторы.
#ifndef _CBIGNUM_DEF_ALLOC
  cBigNumber ()                 { e = stub(); len = 0; }// По умолчанию (0).
#else //_CBIGNUM_DEF_ALLOC
  cBigNumber ()                 { create (0);   }       // По умолчанию (0).
#endif//_CBIGNUM_DEF_ALLOC
  cBigNumber (CBNL b)           { create (b);   }       // Число.
  cBigNumber (const cBigNumber&);                       // Копирование.
  cBigNumber (const char* psz, unsigned radix);         // Строка.

//      Деструктор.
#ifndef _CBIGNUM_DEF_ALLOC
  ~cBigNumber()                 { if (len) exfree (e);  }
#else //_CBIGNUM_DEF_ALLOC
  ~cBigNumber()                 { exfree (e);           }
#endif//_CBIGNUM_DEF_ALLOC

//      Внутреннее представление числа.
  size_t      length  () const  { return (size_t)(*e);} // Число слов.
  const CBNL* code    () const  { return e + 1;       } // Массив слов.
  const CBNL& loword  () const  { return e [*e != 0]; } // Младшее слово.
  const CBNL& hiword  () const  { return e [length()];} // Старшее слово.
  size_t      isfit   () const;                         // Нормализованное?
  size_t      words   () const;                         // Число значащих слов.
  size_t      exwords () const;                         // Число младших 0-слов.

//      Информация о числе.
  CBNL        bits    () const;                         // Число значащих бит.
  cBigNumber& setbits   (const cBigNumber&);
  cBigNumber& setbits   (CBNL);
  CBNL        exbits  () const;                         // Число младших 0-бит.
  cBigNumber& setexbits (const cBigNumber&);
  cBigNumber& setexbits (CBNL);

//      Служебные преобразования.
//      Число не меняет свое значение, но его внутреннее
//      представление может быть модифицировано.
  cBigNumber& fit   ();                                 // Нормализация.
  cBigNumber& tab   ();                                 // Таблица сдвигов.
  cBigNumber& smp   ();                                 // Готовим mulsmp().
  cBigNumber& gc    ();                                 // Оптимизация памяти.
  cBigNumber& pack  ();                                 // Минимизация памяти.
const cBigNumber& fit   () const { return ((cBigNumber*)this)->fit();   }
const cBigNumber& tab   () const { return ((cBigNumber*)this)->tab();   }
const cBigNumber& smp   () const { return ((cBigNumber*)this)->smp();   }
const cBigNumber& gc    () const { return ((cBigNumber*)this)->gc();    }
const cBigNumber& pack  () const { return ((cBigNumber*)this)->pack();  }

//      Метод clear() из Exarray.h обнуляет число и освобождает память,
//      здесь мы переопределяем метод если освобождение памяти не разрешено.
#ifdef  _CBIGNUM_DEF_ALLOC
  void        clear ();                                 // Оптимизация под 0.
#endif//_CBIGNUM_DEF_ALLOC

//      Быстрый тест на нормализованный или бессловесный 0:
//          0 если 0 с числом слов <= 1
//       не 0 если 0 с числом слов >= 2 или не 0
  CBNL       _testnot0() const  { return (hiword() | (e [0] >> 1)); }

//      Операции сравнения. Выдают  -1 = меньше, 0 = равно, 1 = больше.
  int         comp  (const cBigNumber&) const;          // Сравнение с
  int         comp  (CBNL)              const;          // заданным числом.
  int         comp0 ()                  const;          // Сравнение с 0.

//      Унарные операции c накоплением результата.
  cBigNumber& neg   ()  { return setneg  (*this); }     // Обращение знака.
  cBigNumber& abs   ();                                 // Абсолютное значение.
  cBigNumber& unsign();                                 // Беззнаковое значение.
//cBigNumber& compl ()  { return setcompl(*this); }     // Побитовая инверсия.
  cBigNumber& operator ++();                            // Префиксный инкремент.
  cBigNumber& operator --();                            // Префиксный декремент.
  cBigNumber& mul2  ();                                 // Умножение на 2.
  cBigNumber& div2  ();                                 // Деление на 2.
  cBigNumber& pow2  ();                                 // Квадрат.
  cBigNumber& sqrt  ();                                 // Квадратный корень.

//      Присваивание копии числа или результата операции.
//      Операнды передаются через параметры.
  cBigNumber& set         (const cBigNumber&);          // Копирование.
  cBigNumber& set         (CBNL);
  cBigNumber& setneg      (const cBigNumber&);          // Обращение знака.
  cBigNumber& setneg      (CBNL);
  cBigNumber& setabs      (const cBigNumber&);          // Абсолютное значение.
  cBigNumber& setabs      (CBNL);
  cBigNumber& setunsign   (const cBigNumber&);          // Беззнаковое значение.
  cBigNumber& setunsign   (CBNL);
  cBigNumber& setcompl    (const cBigNumber&);          // Побитовая инверсия.
  cBigNumber& setcompl    (CBNL);
  cBigNumber& setxor (const cBigNumber&, const cBigNumber&);    // Побитовая
  cBigNumber& setxor (const cBigNumber&, CBNL);                 // сумма по
  cBigNumber& setxor (CBNL, const cBigNumber&);                 // модулю 2.
  cBigNumber& setxor (CBNL, CBNL);
  cBigNumber& setand (const cBigNumber&, const cBigNumber&);    // Побитовая
  cBigNumber& setand (const cBigNumber&, CBNL);                 // конъюнкция.
  cBigNumber& setand (CBNL, const cBigNumber&);
  cBigNumber& setand (CBNL, CBNL);
  cBigNumber& setor  (const cBigNumber&, const cBigNumber&);    // Побитовая
  cBigNumber& setor  (const cBigNumber&, CBNL);                 // дизъюнкция.
  cBigNumber& setor  (CBNL, const cBigNumber&);
  cBigNumber& setor  (CBNL, CBNL);
  cBigNumber& setadd (const cBigNumber&, const cBigNumber&);    // Сложение.
  cBigNumber& setadd (const cBigNumber&, CBNL);
  cBigNumber& setadd (CBNL, const cBigNumber&);
  cBigNumber& setadd (CBNL, CBNL);
  cBigNumber& setsub (const cBigNumber&, const cBigNumber&);    // Вычитание.
  cBigNumber& setsub (const cBigNumber&, CBNL);
  cBigNumber& setsub (CBNL, const cBigNumber&);
  cBigNumber& setsub (CBNL, CBNL);
  cBigNumber& setmul (const cBigNumber&, const cBigNumber&);    // Умножение.
  cBigNumber& setmul (const cBigNumber&, CBNL);
  cBigNumber& setmul (CBNL, const cBigNumber&);
  cBigNumber& setmul (CBNL, CBNL);
  cBigNumber& setdiv (const cBigNumber&, const cBigNumber&);    // Деление.
  cBigNumber& setdiv (const cBigNumber&, CBNL);
  cBigNumber& setdiv (CBNL, const cBigNumber&);
  cBigNumber& setdiv (CBNL, CBNL);
  cBigNumber& setmod (const cBigNumber&, const cBigNumber&);    // Остаток.
  cBigNumber& setmod (const cBigNumber&, CBNL);
  cBigNumber& setmod (CBNL, const cBigNumber&);
  cBigNumber& setmod (CBNL, CBNL);
  cBigNumber& setshl (const cBigNumber&, const cBigNumber&);    // Сдвиг
  cBigNumber& setshl (const cBigNumber&, CBNL);                 // влево.
  cBigNumber& setshl (CBNL, const cBigNumber&);
  cBigNumber& setshl (CBNL, CBNL);
  cBigNumber& setshr (const cBigNumber&, const cBigNumber&);    // Сдвиг
  cBigNumber& setshr (const cBigNumber&, CBNL);                 // вправо.
  cBigNumber& setshr (CBNL, const cBigNumber&);
  cBigNumber& setshr (CBNL, CBNL);
  cBigNumber& setpow (const cBigNumber&, const cBigNumber&);    // Степень.
  cBigNumber& setpow (const cBigNumber&, CBNL);
  cBigNumber& setpow (CBNL, const cBigNumber&);
  cBigNumber& setpow (CBNL, CBNL);
  cBigNumber& setpowmod (const cBigNumber&, const cBigNumber&,  // Степень
                         const cBigNumber&);                    // по модулю.
  cBigNumber& setpowmod (const cBigNumber&, const cBigNumber&, CBNL);
  cBigNumber& setpowmod (const cBigNumber&, CBNL, const cBigNumber&);
  cBigNumber& setpowmod (const cBigNumber&, CBNL, CBNL);
  cBigNumber& setpowmod (CBNL, const cBigNumber&, const cBigNumber&);
  cBigNumber& setpowmod (CBNL, const cBigNumber&, CBNL);
  cBigNumber& setpowmod (CBNL, CBNL, const cBigNumber&);
  cBigNumber& setpowmod (CBNL, CBNL, CBNL);
  cBigNumber& setsqrt   (const cBigNumber&);                    // Кв. корень.
  cBigNumber& setsqrt   (CBNL);
  cBigNumber& setrandom (unsigned long (*p)(), unsigned CBNL);  // Случайное.

//      Присваивание.
  cBigNumber& operator =  (const cBigNumber& b) { return set (b);       }
  cBigNumber& operator =  (CBNL b)              { return set (b);       }

//      Бинарные операции c накоплением результата.
//      Второй операнд передается через параметр.
  cBigNumber& operator ^= (const cBigNumber&);  // Побитовое сложение по mod 2.
  cBigNumber& operator ^= (CBNL);
  cBigNumber& operator &= (const cBigNumber&);  // Побитовая конъюнкция.
  cBigNumber& operator &= (CBNL);
  cBigNumber& operator |= (const cBigNumber&);  // Побитовая дизъюнкция.
  cBigNumber& operator |= (CBNL);
  cBigNumber& operator += (const cBigNumber&);  // Сложение.
  cBigNumber& operator += (CBNL);
  cBigNumber& operator -= (const cBigNumber&);  // Вычитание.
  cBigNumber& operator -= (CBNL);
  cBigNumber& operator *= (const cBigNumber&);  // Умножение.
  cBigNumber& operator *= (CBNL);
  cBigNumber& operator /= (const cBigNumber&);  // Деление.
  cBigNumber& operator /= (CBNL);
  cBigNumber& operator %= (const cBigNumber&);  // Остаток.
  cBigNumber& operator %= (CBNL);
  cBigNumber& operator<<= (const cBigNumber&);  // Сдвиг влево.
  cBigNumber& operator<<= (CBNL);
  cBigNumber& operator>>= (const cBigNumber&);  // Сдвиг вправо.
  cBigNumber& operator>>= (CBNL);
  cBigNumber& pow         (const cBigNumber&);  // Степень.
  cBigNumber& pow         (CBNL);

//      Тернарные операции c накоплением результата.
//      Второй и третий операнды передаются через параметры.
  cBigNumber& powmod      (const cBigNumber&, const cBigNumber&);
  cBigNumber& powmod      (const cBigNumber&, CBNL);
  cBigNumber& powmod      (CBNL, const cBigNumber&);
  cBigNumber& powmod      (CBNL, CBNL);

//      Копирование в комбинации с сдвигом CBNL-слов влево или вправо.
  cBigNumber& set         (const cBigNumber&, size_t k);
  cBigNumber& set         (CBNL, size_t k);
  cBigNumber& setr        (const cBigNumber&, size_t k);

//      Накопление в комбинации с сдвигом CBNL-слов влево.
  cBigNumber& add         (const cBigNumber&, size_t k = 0);
  cBigNumber& add         (CBNL, size_t k = 0);
  cBigNumber& sub         (const cBigNumber&, size_t k = 0);
  cBigNumber& sub         (CBNL, size_t k = 0);

//      Комбинированная операция умножения с накоплением.
//      Операнды умножения передаются через параметры,
//      результат умножения накапливается.
  cBigNumber& addmul      (const cBigNumber&, const cBigNumber&);
  cBigNumber& addmul      (const cBigNumber&, CBNL);
  cBigNumber& addmul      (CBNL, const cBigNumber&);
  cBigNumber& addmul      (CBNL, CBNL);
  cBigNumber& submul      (const cBigNumber&, const cBigNumber&);
  cBigNumber& submul      (const cBigNumber&, CBNL);
  cBigNumber& submul      (CBNL, const cBigNumber&);
  cBigNumber& submul      (CBNL, CBNL);

//      Комбинированная операция деления с остатком.
//      Остаток помещается на место делимого, которое
//      не должно перекрываться с результатом.
  cBigNumber& setdivmod   (cBigNumber&, const cBigNumber&);
  cBigNumber& setdivmod   (cBigNumber&, CBNL);

//      Комбинированная операция квадратного корня с остатком.
//      Остаток помещается на место основания, которое
//      не должно перекрываться с результатом.
  cBigNumber& setsqrtrm   (cBigNumber&);

//      Умножение с накоплением предварительно подготовленных операндов.
//      Множимое должно иметь таблицу сдвигов, подготовленную методом tab().
//      Множитель должен быть неотрицательным.
//      Операнды не должны перекрываться с буфером результата.
  cBigNumber& addmultab   (const cBigNumber&, const cBigNumber&, size_t k = 0);
  cBigNumber& addmultab   (const cBigNumber&, CBNL, size_t k = 0);
  cBigNumber& submultab   (const cBigNumber&, const cBigNumber&, size_t k = 0);
  cBigNumber& submultab   (const cBigNumber&, CBNL, size_t k = 0);
//      Множимое должно быть подготовлено методами smp() или tab().
//      Множитель должен быть неотрицательным.
//      Операнды не должны перекрываться с буфером результата.
  cBigNumber& addmulsmp   (const cBigNumber&, const cBigNumber&, size_t k = 0);
  cBigNumber& addmulsmp   (const cBigNumber&, CBNL, size_t k = 0);
  cBigNumber& submulsmp   (const cBigNumber&, const cBigNumber&, size_t k = 0);
  cBigNumber& submulsmp   (const cBigNumber&, CBNL, size_t k = 0);

//      Деление и остаток предварительно подготовленных операндов.
//      Операнды должны иметь одинаковый знак.
//      Делимое должно быть нормализованным,
//      делитель должен иметь таблицу сдвигов, подготовленную методом tab(),
//      и не должен перекрываться с буфером результата.
  cBigNumber& setdivtab (const cBigNumber&, const cBigNumber&, size_t k = 0);
  cBigNumber& setdivtab (CBNL, const cBigNumber&, size_t k = 0);
  cBigNumber& setmodtab (const cBigNumber&, const cBigNumber&, size_t k = 0);
  cBigNumber& setmodtab (CBNL, const cBigNumber&, size_t k = 0);

//      Деление с остатком предварительно подготовленных операндов.
//      Операнды должны иметь одинаковый знак и не должны
//      перекрываться с буфером результата и друг с другом.
//      Делимое должно быть нормализованным.
//      делитель должен иметь таблицу сдвигов, подготовленную методом tab(),
  cBigNumber& setdivmodtab    (cBigNumber&, const cBigNumber&, size_t k = 0);

//      Сообщения.
static  const char* pszConverting;
static  const char* pszFormatting;

//      Строковые преобразования.
#define MAX_RADIX 16
static  const char  numtochar [2] [MAX_RADIX + 1];      // Недокументированные
static        char  chartonum [UCHAR_MAX + 1];          // внутренние массивы.
static  size_t  SqrTab  (unsigned radix, size_t len = 0, size_t max_np = 0);
  cBigNumber& setunsign (const EXPTR(char) ps,          // Преобразование в
                         unsigned radix,                // число без знака.
                         size_t   width);
  cBigNumber& set       (const char* psz,               // Преобразование
                         unsigned radix = 10);          // строки в число.
#define cBigNumber_uppercase    0x00040000L
#define cBigNumber_showbase     0x00080000L
#define cBigNumber_showpos      0x00200000L
#define cBigNumber_unsign       0x80000000L
  char*       toa       (cBigString& buf_,
                         unsigned radix = 10,           // Форматирование в
                         size_t   width = 0,            // расширяемом буфере.
                         long     fill  = 0)    const;
#ifdef  _CBIGNUM_TOATMP
  char*       toatmp    (unsigned radix = 10,           // Форматирование в
                         size_t   width = 0,            // статическом буфере
                         long     fill  = 0)    const;  // (не рекомендуется).
#endif//_CBIGNUM_TOATMP
  void        dump      ()  const;                      // Вывод массива слов.
  void        info      ()  const;                      // Информация о словах.

//      Формирование целого числа с проверкой диапазона.
  void        erange    ()  const;                      // Ошибка диапазона.
  CBNL        toCBNL    ()  const;
  long        tolong    ()  const;
  int         toint     ()  const;
  short       toshort   ()  const;

//      Преобразования для выполнения штатных логических операций языка C.
  operator const void * ()  const       { return (comp0()? e: 0);       }
  int        operator ! ()  const       { return (comp0() == 0);        }

//      Управление вычислениями.
static  void  maskdiv0  (int);          // Маскирование деления на 0.
static  int   testdiv0  ();             // Было ли деление на 0?

//      Доступ к внутренним статическим буферам (не рекомендуется).
#ifndef _CBIGNUM_MT
static  cBigNumber& lastdivmod ();      // Остаток последнего деления.
static  cBigNumber& lastrootrm ();      // Остаток последнего корня.
#endif//_CBIGNUM_MT
};

//================================================
//      Элементарный тест на простоту.
//================================================

//      Тест Strong Probable Primality для основания b.
int     b_SPRP      (const cBigNumber& n, const cBigNumber& b);

//================================================
//      Интегральные тесты не простоту (prime.cpp).
//================================================

//      Тест Strong Probable Primality.
int     SPRP        (const cBigNumber&);
int     SPRP        (const cBigNumber&, cBigNumber&);
int FastSPRP        (const cBigNumber&);
int FastSPRP        (const cBigNumber&, cBigNumber&);
int LastSPRP        (const cBigNumber&);
int LastSPRP        (const cBigNumber&, cBigNumber&);

//      Тест факторизации.
int TestFactor      (const cBigNumber&);
int TestFactor      (const cBigNumber&, cBigNumber&);
int TestSmallFactor (const cBigNumber&);
int TestSmallFactor (const cBigNumber&, cBigNumber&);
int TestLargeFactor (const cBigNumber&);
int TestLargeFactor (const cBigNumber&, cBigNumber&);

//      Комбинированный тест (факторизация + SPRP).
int IsPrime         (const cBigNumber&);
int IsProvedPrime   (const cBigNumber&);
int IsMillerPrime   (const cBigNumber&);
int IsStrongPrime   (const cBigNumber&);

//      Старая функция, совместимая с версией 1.1a
#ifdef  _CBIGNUM_HASFACTOR
inline int HasFactor(const cBigNumber& n) { return TestFactor(n);   }
#endif//_CBIGNUM_HASFACTOR

//================================================
//      Независимый потоковый вывод.
//================================================

cTTY& operator << (cTTY& os, const cBigNumber& b);
void operator <<= (cTTY& os, const cBigNumber& b);

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

extern cTTY cBigNumberMessages;         // Обработчик сообщений.
extern cTTY cBigNumberProgress;         // Обработчик прогресс-индикации.

#ifdef  __cplusplus
}
#endif//__cplusplus

//================================================
//      Класс для шестнадцатеричного дампа.
//================================================

class cHexDump
{
  char buffer [CHAR_BIT * sizeof (CBNL) * 3 / 8];
public:
  cHexDump (unsigned CBNL n);
  operator const char*() { return (buffer); }
        // const избавляет от проблем с Borland C++

};

//================================================
//      Вспомогательный класс для передачи
//      числа типа CBNL в служебные функции.
//================================================

class _cBigLong
{
  CBNL e [2];           // Рабочий массив.

public:                 // Присваивания рабочему массиву.

  _cBigLong         (CBNL n = 0) { e [0] = 1; e [1] = n;        }

  _cBigLong& operator = (CBNL n) { e [1] = n; return *this;     }

//      Формирование обычного указателя для
//      обращения к массиву без контроля индексов.

  CBNL*       base()            { return (e);                   }

#ifndef NCHECKPTR

//      Формирование ограниченного указателя для
//      обращения к массиву с контролем индексов:
//      при выходе индекса за границы массива вызывается
//      функция, присвоенная ::exalloc_status.range_handler.

  operator    exptr<CBNL>()     { return exptr<CBNL> (e, 2);    }

#endif//NCHECKPTR
};

#ifdef  NCHECKPTR
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
#endif//NCHECKPTR

//================================================
//      Служебные переменные.
//================================================

extern  size_t      cBigNumberMaskDiv0;     // Маска деления на 0.
#ifndef _CBIGNUM_MT
extern  cBigNumber  cBigNumberLastDivMod;   // Остаток последнего деления.
extern  cBigNumber  cBigNumberLastRootRm;   // Остаток последнего корня.
#endif//_CBIGNUM_MT

//================================================
//      Базовые функции.
//================================================

void    cBigNumberDump    (const CBPTR(CBNL) p1);
void    cBigNumberInfo    (const CBPTR(CBNL) p1);
void    cBigNumberERange  (const CBPTR(CBNL) p1);
size_t  cBigNumberWords   (const CBPTR(CBNL) p1);
int     cLongBits         (CBNL);
int     cULongBits        (unsigned CBNL);
CBNL    cBigNumberBits    (const CBPTR(CBNL) p1);
size_t  cBigNumberExWords (const CBPTR(CBNL) p1);
int     cLongExBits       (CBNL);
int     cLongExactLog2    (CBNL);
CBNL    cBigNumberExBits  (const CBPTR(CBNL) p1);
size_t _CBNL_C  cBigNumberIsFit
                          (const CBPTR(CBNL) p1);
size_t _CBNL_C _cBigNumberFit   (EXPTR(CBNL) p1);
size_t _CBNL_C _cBigNumberFitTo (EXPTR(CBNL) p1, size_t n);
int    _CBNL_C  cBigNumberComp
                          (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2);
int    _CBNL_C  cBigNumberCompHigh
                          (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2);
void   _CBNL_C  cBigNumberCopy
                          (const CBPTR(CBNL) p1, EXPTR(CBNL) p);
void    cBigNumberCompl   (const CBPTR(CBNL) p1, EXPTR(CBNL) p);
void    cBigNumberCopyShl (const CBPTR(CBNL) p1, size_t k1, EXPTR(CBNL) p);
void    cBigNumberCopyShr (const CBPTR(CBNL) p1, size_t k1, EXPTR(CBNL) p);
void    cBigNumberCutOut  (const CBPTR(CBNL) p1, size_t k1, size_t n,
                                 EXPTR(CBNL) p);
size_t _CBNL_C _cBigNumberNeg
                          (const CBPTR(CBNL) p1, EXPTR(CBNL) p);
size_t _cBigNumberMInc          (EXPTR(CBNL) p1);
size_t _cBigNumberMDec          (EXPTR(CBNL) p1);
size_t _cBigNumberMMul2         (EXPTR(CBNL) p1);
size_t _cBigNumberMDiv2         (EXPTR(CBNL) p1);
size_t _cBigNumberMul2    (const CBPTR(CBNL) p1, EXPTR(CBNL) p);
size_t _cBigNumberDiv2    (const CBPTR(CBNL) p1, EXPTR(CBNL) p);
void    cBigNumberTab           (EXPTR(CBNL) p1);
size_t _cBigNumberXor     (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
size_t _cBigNumberAnd     (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
size_t _cBigNumberOr      (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
size_t _cBigNumberAdd     (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
size_t _cBigNumberSub     (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
size_t _cBigNumberSubS    (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
size_t _cBigNumberMAdd          (EXPTR(CBNL) p1, const CBPTR(CBNL) p2);
size_t _cBigNumberMSub          (EXPTR(CBNL) p1, const CBPTR(CBNL) p2);
size_t _CBNL_C  cBigNumberSkipLow0
                          (const CBPTR(CBNL) p1);
size_t _CBNL_C  cBigNumberCopySkipLow0
                          (const CBPTR(CBNL) p1, EXPTR(CBNL) p);
void   _CBNL_C  cBigNumberMAddM (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k2);
void   _CBNL_C  cBigNumberMSubM (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k2);
size_t _CBNL_C _cBigNumberMSubD (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k2);
size_t _CBNL_C _cBigNumberMMul2M(EXPTR(CBNL) p1);
size_t _CBNL_C _cBigNumberMDiv2D(EXPTR(CBNL) p1);
void _CBNL_C cBigNumberMAddMulM (EXPTR(CBNL) p1, const EXPTR(CBNL) p2,
                                 unsigned CBNL l2, size_t k2);
void _CBNL_C cBigNumberMSubMulM (EXPTR(CBNL) p1, const EXPTR(CBNL) p2,
                                 unsigned CBNL l2, size_t k2);
void    cBigNumberMShrM         (EXPTR(CBNL) p1);
void    cBigNumberMShlD         (EXPTR(CBNL) p1);
void    cBigNumberMAddShl       (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k2);
void    cBigNumberMSubShl       (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k2);
void    cBigNumberMAddMulShl    (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k, EXPTR(CBNL) p);
void    cBigNumberMSubMulShl    (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k, EXPTR(CBNL) p);
void    cBigNumberMAddMulShlTab
                          (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k, EXPTR(CBNL) p);
void    cBigNumberMSubMulShlTab
                          (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k, EXPTR(CBNL) p);
void    cBigNumberMAddMulShlKar (EXPTR(CBNL) p1, EXPTR(CBNL) p2,
                                 size_t k, EXPTR(CBNL) p);
void    cBigNumberMSubMulShlKar (EXPTR(CBNL) p1, EXPTR(CBNL) p2,
                                 size_t k, EXPTR(CBNL) p);
void    cBigNumberMAddMul (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
void    cBigNumberMSubMul (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
void    cBigNumberMul     (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
void   _CBNL_C  cLongMul  (CBNL, CBNL, CBNL p [2]);
void   _CBNL_C  cULongMul (unsigned CBNL, unsigned CBNL, unsigned CBNL p [2]);
void    cBigNumberDiv0();
void    cBigNumberMModDivShlTab (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k2, EXPTR(CBNL) p);
void    cBigNumberMModDiv       (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
void    cBigNumberMModShlTab    (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 size_t k2);
void    cBigNumberMMod          (EXPTR(CBNL) p1, const CBPTR(CBNL) p2);
void    cBigNumberPow           (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                 EXPTR(CBNL) p);
void    cBigNumberPowMod        (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                                 const CBPTR(CBNL) mod,
                                 EXPTR(CBNL) p);
void    cBigNumberMRmSqrt       (EXPTR(CBNL) p1, EXPTR(CBNL) p);
void    cBigNumberRandom  (unsigned long (*pfnRand)(), unsigned CBNL lBits,
                                 EXPTR(CBNL) p);
unsigned CBNL _CBNL_C cLongRandom (unsigned long (*pfnRand)());

#ifdef  NCHECKPTR
#ifdef  __cplusplus
}
#endif//__cplusplus
#endif//NCHECKPTR

//================================================
//      Базовые inline-функции.
//================================================

inline  size_t  cBigNumberNeg     (const CBPTR(CBNL) p1, EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberNeg (p1, p));
}

inline  size_t  cBigNumberMInc          (EXPTR(CBNL) p1)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMInc (p1));
}

inline  size_t  cBigNumberMDec          (EXPTR(CBNL) p1)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMDec (p1));
}

inline  size_t  cBigNumberMMul2         (EXPTR(CBNL) p1)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMMul2 (p1));
}

inline  size_t  cBigNumberMDiv2         (EXPTR(CBNL) p1)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMDiv2 (p1));
}

inline  size_t  cBigNumberMul2   (const CBPTR(CBNL) p1, EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberMul2 (p1, p));
}

inline  size_t  cBigNumberDiv2   (const CBPTR(CBNL) p1, EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberDiv2 (p1, p));
}

inline  size_t  cBigNumberFit          (EXPTR(CBNL) p1)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberFit (p1));
}

inline  size_t  cBigNumberFitTo        (EXPTR(CBNL) p1, size_t n)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberFitTo (p1, n));
}

inline  size_t  cBigNumberXor    (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                        EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberXor (p1, p2, p));
}

inline  size_t  cBigNumberAnd    (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                        EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberAnd (p1, p2, p));
}

inline  size_t  cBigNumberOr     (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                        EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberOr  (p1, p2, p));
}

inline  size_t  cBigNumberAdd    (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                        EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberAdd (p1, p2, p));
}

inline  size_t  cBigNumberSub    (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                        EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberSub (p1, p2, p));
}

inline  size_t  cBigNumberSubS   (const CBPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                        EXPTR(CBNL) p)
{
  return (size_t)(*(p)=(CBNL)_cBigNumberSubS (p1, p2, p));
}

inline  size_t  cBigNumberMAdd         (EXPTR(CBNL) p1, const CBPTR(CBNL) p2)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMAdd (p1, p2));
}

inline  size_t  cBigNumberMSub         (EXPTR(CBNL) p1, const CBPTR(CBNL) p2)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMSub (p1, p2));
}

inline  size_t  cBigNumberMSubD        (EXPTR(CBNL) p1, const CBPTR(CBNL) p2,
                                        size_t k2)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMSubD (p1, p2, k2));
}

inline  size_t  cBigNumberMMul2M        (EXPTR(CBNL) p1)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMMul2M (p1));
}

inline  size_t  cBigNumberMDiv2D        (EXPTR(CBNL) p1)
{
  return (size_t)(*(p1)=(CBNL)_cBigNumberMDiv2D (p1));
}

#if UCBNL_MAX <= ULONG_MAX
inline unsigned CBNL _CBNL_C cLongRandom (unsigned long (*pfnRand)())
{
  return (unsigned CBNL)(*pfnRand)();
}
#endif

//================================================
//      Реализация конструкторов.
//================================================

inline cBigNumber::cBigNumber (const cBigNumber& b)
{
  e = stub(); len = 0;                                  // Обнуление полей.
#ifndef _CBIGNUM_DEF_ALLOC
  if (b._testnot0() != 0)
#endif
  {
    checkexpand (b.length());                           // Установка размера.
    cBigNumberCopy (CBPTRTYPE(b), EXPTRTYPE(*this));    // Копирование.
    checkindex (length());                              // Отладочная проверка.
  }
}

inline cBigNumber::cBigNumber (const char* psz, unsigned radix)
{
#ifndef _CBIGNUM_DEF_ALLOC
  e = stub(); len = 0;                                  // Обнуление полей.
#else //_CBIGNUM_DEF_ALLOC
  create (0);                                           // Создание числа.
#endif//_CBIGNUM_DEF_ALLOC
  set (psz, radix);                                     // Разбор константы.
}

//================================================
//      Реализация информационных методов.
//================================================

inline void   cBigNumber::dump()    const
{
  cBigNumberDump (CBPTRTYPE(*this));
}

inline void   cBigNumber::info()    const
{
  cBigNumberInfo (CBPTRTYPE(*this));
}

inline void   cBigNumber::erange()  const
{
  cBigNumberERange (CBPTRTYPE(*this));
}

inline size_t cBigNumber::isfit()   const
{
  return cBigNumberIsFit (CBPTRTYPE(*this));
}

inline size_t cBigNumber::words()   const
{
  return cBigNumberWords (CBPTRTYPE(*this));
}

inline size_t cBigNumber::exwords() const
{
  return cBigNumberExWords (CBPTRTYPE(*this));
}

inline CBNL   cBigNumber::bits()    const
{
  return cBigNumberBits (CBPTRTYPE(*this));
}

inline CBNL   cBigNumber::exbits()  const
{
  return cBigNumberExBits (CBPTRTYPE(*this));
}

//================================================
//      Реализация методов сравнения.
//================================================

inline int  cBigNumber::comp (const cBigNumber& b)  const
{
  return cBigNumberComp (CBPTRTYPE(*this), CBPTRTYPE(b));
}

inline int  cBigNumber::comp (CBNL b)               const
{
  return cBigNumberComp (CBPTRTYPE(*this), CBPTRTYPE(_cBigLong(b)));
}

inline int  cBigNumber::comp0 ()                    const
{
  return cBigNumberComp (CBPTRTYPE(*this), CBPTRTYPE(_cBigLong(0)));
}

//================================================
//      Реализация унарных операций накопления.
//================================================

inline cBigNumber& cBigNumber::abs()
{
  if (hiword() < 0L) neg();
  return *this;
}

inline cBigNumber& cBigNumber::unsign()
{
  if (hiword() < 0L)
  {
    size_t len = length() + 1;
    checkexpand (len); e [0] = (CBNL)len; e [len] = 0;
  }
  return *this;
}

//================================================
//      Реализация операций присваивания.
//================================================

inline cBigNumber& cBigNumber::set (const cBigNumber& b)
{
#ifndef _CBIGNUM_DEF_ALLOC
  if ((e [0] | b._testnot0()) != 0)
#endif
  {
    checkexpand (b.length());
    cBigNumberCopy (CBPTRTYPE(b), EXPTRTYPE(*this));
    checkindex (length());
  }
  return *this;
}

inline cBigNumber& cBigNumber::set (CBNL b)
{
#ifndef _CBIGNUM_DEF_ALLOC
  if ((e [0] | b) != 0)
#endif//_CBIGNUM_DEF_ALLOC
  {
    checkexpand (1); e [0] = 1; e [1] = b;
    checkindex (1);
  }
  return *this;
}

inline cBigNumber& cBigNumber::setneg (const cBigNumber& b)
{
  checkexpand (b.length() + 1);
  cBigNumberNeg (CBPTRTYPE(b), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::setneg (CBNL b)
{
  checkexpand (2); e [0] = 1 + (b == LONG_MIN); e [1] = -b; e [2] = 0;
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::setabs (const cBigNumber& b)
{
  return (b.hiword()>=0? set (b): setneg (b));
}

inline cBigNumber& cBigNumber::setabs (CBNL b)
{
  if (b < 0) b = -b;
  checkexpand (2); e [0] = 1 + (b == LONG_MIN); e [1] = b; e [2] = 0;
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::setunsign (const cBigNumber& b)
{
  set (b); return unsign();
}

inline cBigNumber& cBigNumber::setunsign (CBNL b)
{
  checkexpand (2); e [0] = 1 + (b < 0); e [1] = b; e [2] = 0;
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::setcompl (const cBigNumber& b)
{
  checkexpand (b.length() + (b.length() == 0));
  cBigNumberCompl (CBPTRTYPE(b), EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::setcompl (CBNL b)
{
  checkexpand (1); e [0] = 1; e [1] = ~b;
  checkindex (length());
  return *this;
}

//================================================
//      Реализация бинарных операций накопления.
//================================================

inline cBigNumber& cBigNumber::operator ^= (const cBigNumber& b)
{
  return setxor (*this, b);
}

inline cBigNumber& cBigNumber::operator ^= (CBNL b)
{
  return setxor (*this, b);
}

inline cBigNumber& cBigNumber::operator &= (const cBigNumber& b)
{
  return setand (*this, b);
}

inline cBigNumber& cBigNumber::operator &= (CBNL b)
{
  return setand (*this, b);
}

inline cBigNumber& cBigNumber::operator |= (const cBigNumber& b)
{
  return setor (*this, b);
}

inline cBigNumber& cBigNumber::operator |= (CBNL b)
{
  return setor (*this, b);
}

inline cBigNumber& cBigNumber::operator += (const cBigNumber& b)
{
  return add (b);
}

inline cBigNumber& cBigNumber::operator += (CBNL b)
{
  return add (b);
}

inline cBigNumber& cBigNumber::operator -= (const cBigNumber& b)
{
  return sub (b);
}

inline cBigNumber& cBigNumber::operator -= (CBNL b)
{
  return sub (b);
}

inline cBigNumber& cBigNumber::operator *= (const cBigNumber& b)
{
  return setmul (*this, b);
}

inline cBigNumber& cBigNumber::operator *= (CBNL b)
{
  return setmul (*this, b);
}

inline cBigNumber& cBigNumber::operator /= (const cBigNumber& b)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod;
#endif//_CBIGNUM_MT
  swap (cBigNumberLastDivMod);
  return setdivmod (cBigNumberLastDivMod, b);
}

inline cBigNumber& cBigNumber::operator /= (CBNL b)
{
#ifdef  _CBIGNUM_MT
  cBigNumber cBigNumberLastDivMod;
#endif//_CBIGNUM_MT
  swap (cBigNumberLastDivMod);
  return setdivmod (cBigNumberLastDivMod, b);
}

inline cBigNumber& cBigNumber::operator %= (const cBigNumber& b)
{
  fit();
  cBigNumberMMod (EXPTRTYPE(*this), CBPTRTYPE(b));
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::operator %= (CBNL b)
{
  fit();
  cBigNumberMMod (EXPTRTYPE(*this), CBPTRTYPE(_cBigLong(b)));
  checkindex (length());
  return *this;
}

//================================================
//      Реализация копирования со сдвигом.
//================================================

inline cBigNumber& cBigNumber::set (const cBigNumber& b, size_t k)
{
  checkexpand (b.length() + (b.length() == 0) + k);
  cBigNumberCopyShl (CBPTRTYPE(b), k, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::set (CBNL b, size_t k)
{
  checkexpand (k + 1);
  cBigNumberCopyShl (CBPTRTYPE(_cBigLong(b)), k, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

inline cBigNumber& cBigNumber::setr (const cBigNumber& b, size_t k)
{
  checkexpand (b.length() > k? b.length() - k: 1);
  cBigNumberCopyShr (CBPTRTYPE(b), k, EXPTRTYPE(*this));
  checkindex (length());
  return *this;
}

//================================================
//      Реализация проверки диапазона.
//================================================

inline  CBNL    cBigNumber::toCBNL()  const
{
  if (size() == 0)      return (0);
  if (length() > 1)     erange();
  return (hiword());
}

inline  long    cBigNumber::tolong()  const
{
  CBNL n = toCBNL();
  if ((long) n != n)    erange();
  return ((long)n);
}

inline  int     cBigNumber::toint()   const
{
  CBNL n = toCBNL();
  if ((int) n != n)     erange();
  return ((int)n);
}

inline  short   cBigNumber::toshort() const
{
  CBNL n = toCBNL();
  if ((short) n != n)   erange();
  return ((short)n);
}

//================================================
//      Реализация управляющих методов.
//================================================

inline void cBigNumber::maskdiv0 (int mask) { cBigNumberMaskDiv0 = mask << 1; }
inline int  cBigNumber::testdiv0()          { return cBigNumberMaskDiv0 == 1; }

#ifndef _CBIGNUM_MT
inline cBigNumber& cBigNumber::lastdivmod() { return cBigNumberLastDivMod; }
inline cBigNumber& cBigNumber::lastrootrm() { return cBigNumberLastRootRm; }
#endif//_CBIGNUM_MT

//================================================
//      Реализация штатных операций сравнения.
//================================================

inline  int  operator == (const cBigNumber& a, const cBigNumber& b)
{
  return (a.comp(b) == 0);
}

inline  int  operator == (const cBigNumber& a, CBNL b)
{
  return (a.comp(b) == 0);
}

inline  int  operator == (CBNL b, const cBigNumber& a)
{
  return (a.comp(b) == 0);
}

inline  int  operator != (const cBigNumber& a, const cBigNumber& b)
{
  return (a.comp(b) != 0);
}

inline  int  operator != (const cBigNumber& a, CBNL b)
{
  return (a.comp(b) != 0);
}

inline  int  operator != (CBNL b, const cBigNumber& a)
{
  return (a.comp(b) != 0);
}

inline  int  operator >= (const cBigNumber& a, const cBigNumber& b)
{
  return (a.comp(b) >= 0);
}

inline  int  operator >= (const cBigNumber& a, CBNL b)
{
  return (a.comp(b) >= 0);
}

inline  int  operator >= (CBNL b, const cBigNumber& a)
{
  return (a.comp(b) <= 0);
}

inline  int  operator <= (const cBigNumber& a, const cBigNumber& b)
{
  return (a.comp(b) <= 0);
}

inline  int  operator <= (const cBigNumber& a, CBNL b)
{
  return (a.comp(b) <= 0);
}

inline  int  operator <= (CBNL b, const cBigNumber& a)
{
  return (a.comp(b) >= 0);
}

inline  int  operator >  (const cBigNumber& a, const cBigNumber& b)
{
  return (a.comp(b) > 0);
}

inline  int  operator >  (const cBigNumber& a, CBNL b)
{
  return (a.comp(b) > 0);
}

inline  int  operator >  (CBNL b, const cBigNumber& a)
{
  return (a.comp(b) < 0);
}

inline  int  operator <  (const cBigNumber& a, const cBigNumber& b)
{
  return (a.comp(b) < 0);
}

inline  int  operator <  (const cBigNumber& a, CBNL b)
{
  return (a.comp(b) < 0);
}

inline  int  operator <  (CBNL b, const cBigNumber& a)
{
  return (a.comp(b) > 0);
}

//================================================
//      Стек временных неограниченных чисел
//      в локальной памяти потока и фиктивный
//      массив для подстановки шаблона.
//================================================

#ifdef  EXTHREAD_LOCAL
extern exblockstack_t EXTHREAD_LOCAL cBigTemp_stack;
extern exblockstack<long>            cBigTemp_sfict;
#else //EXTHREAD_LOCAL
extern exblockstack_t                cBigTemp_stack;
#endif//EXTHREAD_LOCAL

//================================================
//      Классы-наследники для реализации операций,
//      требующих создания временных объектов.
//================================================

class cBigTemp: public cBigNumber               // Число во временном объекте.
{
//      Класс cBigTemp описывает временные объекты
//      для результатов арифметических операций.
//
//      Для оптимизации распределения памяти конструктор cBigTemp
//      использует стек динамических массивов, позволяющий
//      избежать распределения динамической памяти:
//      во время удаления объекта cBigTemp ссылка на его
//      базовый массив заносится в стек, а при создании
//      объекта cBigTemp ему присваивается массив из стека.
//      Обращение к процедурам распределения памяти проводится
//      только в том случае, если стек пуст.
//
//      Стек может дать многократное ускорение на небольших числах,
//      но он применяется только если установлено макро EXTHREAD_LOCAL.
//      1) Для однопоточных приложений макро моет быть пустым.
//      2) Для многопоточных приложений макро должно содержать
//         префикс локальной памяти потока (thread local storage),
//         зависящий от транслятора. Локальная память потока
//         распределяется медленнее на 5-10%.

public:

#ifdef  EXTHREAD_LOCAL

  cBigTemp (): cBigNumber (cBigTemp_stack)      // Конструктор.
              { (*(exblockstack<CBNL>*)&cBigTemp_stack).get (*this); }
  ~cBigTemp() { (*(exblockstack<CBNL>*)&cBigTemp_stack).put (*this); }
                                                // Деструктор.
#else //EXTHREAD_LOCAL

  cBigTemp (): cBigNumber (cBigTemp_stack)
               { e = NULL; len = 0; }           // Конструктор.
  ~cBigTemp()  {}                               // Деструктор.

#endif//EXTHREAD_LOCAL

//      Блокировка операций присваивания класса cBigNumber.
//      Если это не сделать, то будут возможны некорректные
//      присваивания временным объектам вида (-x) = 10;

private:

  cBigNumber& operator ++ ();                   // Префиксный инкремент.
  cBigNumber& operator -- ();                   // Префиксный декремент.
  cBigNumber& operator =  (const cBigNumber&);  // Присваивание.
  cBigNumber& operator =  (CBNL);
  cBigNumber& operator ^= (const cBigNumber&);  // Побитовое сложение по mod 2.
  cBigNumber& operator ^= (CBNL);
  cBigNumber& operator &= (const cBigNumber&);  // Побитовая конъюнкция.
  cBigNumber& operator &= (CBNL);
  cBigNumber& operator |= (const cBigNumber&);  // Побитовая дизъюнкция.
  cBigNumber& operator |= (CBNL);
  cBigNumber& operator += (const cBigNumber&);  // Сложение.
  cBigNumber& operator += (CBNL);
  cBigNumber& operator -= (const cBigNumber&);  // Вычитание.
  cBigNumber& operator -= (CBNL);
  cBigNumber& operator *= (const cBigNumber&);  // Умножение.
  cBigNumber& operator *= (CBNL);
  cBigNumber& operator /= (const cBigNumber&);  // Деление.
  cBigNumber& operator /= (CBNL);
  cBigNumber& operator %= (const cBigNumber&);  // Остаток.
  cBigNumber& operator %= (CBNL);
  cBigNumber& operator<<= (const cBigNumber&);  // Сдвиг влево.
  cBigNumber& operator<<= (CBNL);
  cBigNumber& operator>>= (const cBigNumber&);  // Сдвиг вправо.
  cBigNumber& operator>>= (CBNL);
};

struct cBigInc: public cBigTemp                 // Постфиксный инкремент.
{
  cBigInc   (cBigNumber& a)                             { set (a); ++a; }
};

struct cBigDec: public cBigTemp                 // Постфиксный декремент.
{
  cBigDec   (cBigNumber& a)                             { set (a); --a; }
};

struct cBigPos: public cBigTemp                 // Сохранение знака.
{
  cBigPos   (const cBigNumber& a)                       { set (a);      }
  cBigPos   (CBNL a)                                    { set (a);      }
};

struct cBigNeg: public cBigTemp                 // Обращение знака.
{
  cBigNeg   (const cBigNumber& a)                       { setneg (a);   }
  cBigNeg   (CBNL a)                                    { setneg (a);   }
};

struct cBigAbs: public cBigTemp                 // Абсолютное значение.
{
  cBigAbs   (const cBigNumber& a)                       { setabs (a);   }
  cBigAbs   (CBNL a)                                    { setabs (a);   }
};

struct cBigUnsign: public cBigTemp              // Беззнаковое значение.
{
  cBigUnsign(const cBigNumber& a)                       { setunsign (a);}
  cBigUnsign(CBNL a)                                    { setunsign (a);}
};

struct cBigCompl: public cBigTemp               // Побитовая инверсия.
{
  cBigCompl (const cBigNumber& a)                       { setcompl (a); }
  cBigCompl (CBNL a)                                    { setcompl (a); }
};

struct cBigXor: public cBigTemp                 // Побитовая сумма по модулю 2.
{
  cBigXor (const cBigNumber& a, const cBigNumber& b)    { setxor (a, b); }
  cBigXor (const cBigNumber& a, CBNL b)                 { setxor (a, b); }
  cBigXor (CBNL a, const cBigNumber& b)                 { setxor (a, b); }
  cBigXor (CBNL a, CBNL b)                              { setxor (a, b); }
};

struct cBigAnd: public cBigTemp                 // Побитовая конъюнкция.
{
  cBigAnd (const cBigNumber& a, const cBigNumber& b)    { setand (a, b); }
  cBigAnd (const cBigNumber& a, CBNL b)                 { setand (a, b); }
  cBigAnd (CBNL a, const cBigNumber& b)                 { setand (a, b); }
  cBigAnd (CBNL a, CBNL b)                              { setand (a, b); }
};

struct cBigOr: public cBigTemp                  // Побитовая дизъюнкция.
{
  cBigOr  (const cBigNumber& a, const cBigNumber& b)    { setor  (a, b); }
  cBigOr  (const cBigNumber& a, CBNL b)                 { setor  (a, b); }
  cBigOr  (CBNL a, const cBigNumber& b)                 { setor  (a, b); }
  cBigOr  (CBNL a, CBNL b)                              { setor  (a, b); }
};

struct cBigAdd: public cBigTemp                 // Сложение.
{
  cBigAdd (const cBigNumber& a, const cBigNumber& b)    { setadd (a, b); }
  cBigAdd (const cBigNumber& a, CBNL b)                 { setadd (a, b); }
  cBigAdd (CBNL a, const cBigNumber& b)                 { setadd (a, b); }
  cBigAdd (CBNL a, CBNL b)                              { setadd (a, b); }
};

struct cBigSub: public cBigTemp                 // Вычитание.
{
  cBigSub (const cBigNumber& a, const cBigNumber& b)    { setsub (a, b); }
  cBigSub (const cBigNumber& a, CBNL b)                 { setsub (a, b); }
  cBigSub (CBNL a, const cBigNumber& b)                 { setsub (a, b); }
  cBigSub (CBNL a, CBNL b)                              { setsub (a, b); }
};

struct cBigMul: public cBigTemp                 // Умножение.
{
  cBigMul (const cBigNumber& a, const cBigNumber& b)    { setmul (a, b); }
  cBigMul (const cBigNumber& a, CBNL b)                 { setmul (a, b); }
  cBigMul (CBNL a, const cBigNumber& b)                 { setmul (a, b); }
  cBigMul (CBNL a, CBNL b)                              { setmul (a, b); }
};

struct cBigDiv: public cBigTemp                 // Деление.
{
  cBigDiv (const cBigNumber& a, const cBigNumber& b)    { setdiv (a, b); }
  cBigDiv (const cBigNumber& a, CBNL b)                 { setdiv (a, b); }
  cBigDiv (CBNL a, const cBigNumber& b)                 { setdiv (a, b); }
  cBigDiv (CBNL a, CBNL b)                              { setdiv (a, b); }
};

struct cBigDivMod: public cBigTemp              // Деление с остатком.
{
  cBigDivMod (cBigNumber& a, const cBigNumber& b)       { setdivmod (a, b); }
  cBigDivMod (cBigNumber& a, CBNL b)                    { setdivmod (a, b); }
};

struct cBigMod: public cBigTemp                 // Остаток.
{
  cBigMod (const cBigNumber& a, const cBigNumber& b)    { setmod (a, b); }
  cBigMod (const cBigNumber& a, CBNL b)                 { setmod (a, b); }
  cBigMod (CBNL a, const cBigNumber& b)                 { setmod (a, b); }
  cBigMod (CBNL a, CBNL b)                              { setmod (a, b); }
};

struct cBigShl: public cBigTemp                 // Сдвиг влево.
{
  cBigShl (const cBigNumber& a, const cBigNumber& b)    { setshl (a, b); }
  cBigShl (const cBigNumber& a, CBNL b)                 { setshl (a, b); }
  cBigShl (CBNL a, const cBigNumber& b)                 { setshl (a, b); }
  cBigShl (CBNL a, CBNL b)                              { setshl (a, b); }
};

struct cBigShr: public cBigTemp                 // Сдвиг вправо.
{
  cBigShr (const cBigNumber& a, const cBigNumber& b)    { setshr (a, b); }
  cBigShr (const cBigNumber& a, CBNL b)                 { setshr (a, b); }
  cBigShr (CBNL a, const cBigNumber& b)                 { setshr (a, b); }
  cBigShr (CBNL a, CBNL b)                              { setshr (a, b); }
};

struct cBigPow: public cBigTemp                 // Степень.
{
  cBigPow (const cBigNumber& a, const cBigNumber& b)    { setpow (a, b); }
  cBigPow (const cBigNumber& a, CBNL b)                 { setpow (a, b); }
  cBigPow (CBNL a, const cBigNumber& b)                 { setpow (a, b); }
  cBigPow (CBNL a, CBNL b)                              { setpow (a, b); }
};

struct cBigPowMod: public cBigTemp              // Степень по модулю.
{
  cBigPowMod (const cBigNumber& a, const cBigNumber& b, cBigNumber& mod)
                                                { setpowmod (a, b, mod); }
  cBigPowMod (const cBigNumber& a, const cBigNumber& b, CBNL mod)
                                                { setpowmod (a, b, mod); }
  cBigPowMod (const cBigNumber& a, CBNL b, cBigNumber& mod)
                                                { setpowmod (a, b, mod); }
  cBigPowMod (const cBigNumber& a, CBNL b, CBNL mod)
                                                { setpowmod (a, b, mod); }
  cBigPowMod (CBNL a, const cBigNumber& b, cBigNumber& mod)
                                                { setpowmod (a, b, mod); }
  cBigPowMod (CBNL a, const cBigNumber& b, CBNL mod)
                                                { setpowmod (a, b, mod); }
  cBigPowMod (CBNL a, CBNL b, cBigNumber& mod)
                                                { setpowmod (a, b, mod); }
  cBigPowMod (CBNL a, CBNL b, CBNL mod)
                                                { setpowmod (a, b, mod); }
};

struct cBigSqrt: public cBigTemp                // Квадратный корень.
{
  cBigSqrt (const cBigNumber& a)                        { setsqrt (a);   }
  cBigSqrt (CBNL a)                                     { setsqrt (a);   }
};

struct cBigSqrtRm: public cBigTemp              // Кв. корень с остатком.
{
  cBigSqrtRm (cBigNumber& a)                            { setsqrtrm (a); }
};

struct cBigBits: public cBigTemp                // Число значащих бит.
{
  cBigBits (cBigNumber& a)                              { setbits (a);   }
};

struct cBigExBits: public cBigTemp              // Число значащих младших 0-бит.
{
  cBigExBits (cBigNumber& a)                            { setexbits (a); }
};

struct cBigRandom: public cBigTemp              // Случайное значение.
{
  cBigRandom (unsigned long (*p)(), unsigned CBNL n)    { setrandom (p, n); }
};

//================================================
//      Реализация штатных унарных операций,
//      требующих создания временного объекта.
//================================================

inline cBigInc   operator ++ (cBigNumber &a, int)
{
  return cBigInc (a);
}

inline cBigDec   operator -- (cBigNumber &a, int)
{
  return cBigDec (a);
}

inline cBigPos   operator + (const cBigNumber &a)
{
  return cBigPos (a);
}

inline cBigNeg   operator - (const cBigNumber &a)
{
  return cBigNeg (a);
}

inline cBigCompl operator ~ (const cBigNumber &a)
{
  return cBigCompl (a);
}

//================================================
//      Реализация штатных бинарных операций,
//      требующих создания временного объекта.
//================================================

inline cBigXor operator ^ (const cBigNumber& a, const cBigNumber& b)
{
  return cBigXor (a, b);
}

inline cBigXor operator ^ (const cBigNumber& a, CBNL b)
{
  return cBigXor (a, b);
}

inline cBigXor operator ^ (CBNL b, const cBigNumber& a)
{
  return cBigXor (a, b);
}

inline cBigAnd operator & (const cBigNumber& a, const cBigNumber& b)
{
  return cBigAnd (a, b);
}

inline cBigAnd operator & (const cBigNumber& a, CBNL b)
{
  return cBigAnd (a, b);
}

inline cBigAnd operator & (CBNL b, const cBigNumber& a)
{
  return cBigAnd (a, b);
}

inline cBigOr  operator | (const cBigNumber& a, const cBigNumber& b)
{
  return cBigOr  (a, b);
}

inline cBigOr  operator | (const cBigNumber& a, CBNL b)
{
  return cBigOr  (a, b);
}

inline cBigOr  operator | (CBNL b, const cBigNumber& a)
{
  return cBigOr  (a, b);
}

inline cBigAdd operator + (const cBigNumber& a, const cBigNumber& b)
{
  return cBigAdd (a, b);
}

inline cBigAdd operator + (const cBigNumber& a, CBNL b)
{
  return cBigAdd (a, b);
}

inline cBigAdd operator + (CBNL b, const cBigNumber& a)
{
  return cBigAdd (a, b);
}

inline cBigSub operator - (const cBigNumber& a, const cBigNumber& b)
{
  return cBigSub (a, b);
}

inline cBigSub operator - (const cBigNumber& a, CBNL b)
{
  return cBigSub (a, b);
}

inline cBigSub operator - (CBNL a, const cBigNumber& b)
{
  return cBigSub (a, b);
}

inline cBigMul operator * (const cBigNumber& a, const cBigNumber& b)
{
  return cBigMul (a, b);
}

inline cBigMul operator * (const cBigNumber& a, CBNL b)
{
  return cBigMul (a, b);
}

inline cBigMul operator * (CBNL b, const cBigNumber& a)
{
  return cBigMul (a, b);
}

inline cBigDiv operator / (const cBigNumber& a, const cBigNumber& b)
{
  return cBigDiv (a, b);
}

inline cBigDiv operator / (const cBigNumber& a, CBNL b)
{
  return cBigDiv (a, b);
}

inline cBigDiv operator / (CBNL a, const cBigNumber& b)
{
  return cBigDiv (a, b);
}

inline cBigMod operator % (const cBigNumber& a, const cBigNumber& b)
{
  return cBigMod (a, b);
}

inline cBigMod operator % (const cBigNumber& a, CBNL b)
{
  return cBigMod (a, b);
}

inline cBigMod operator % (CBNL a, const cBigNumber& b)
{
  return cBigMod (a, b);
}

inline cBigShl operator << (const cBigNumber& a, const cBigNumber& b)
{
  return cBigShl (a, b);
}

inline cBigShl operator << (const cBigNumber& a, CBNL b)
{
  return cBigShl (a, b);
}

inline cBigShl operator << (CBNL a, const cBigNumber& b)
{
  return cBigShl (a, b);
}

inline cBigShr operator >> (const cBigNumber& a, const cBigNumber& b)
{
  return cBigShr (a, b);
}

inline cBigShr operator >> (const cBigNumber& a, CBNL b)
{
  return cBigShr (a, b);
}

inline cBigShr operator >> (CBNL a, const cBigNumber& b)
{
  return cBigShr (a, b);
}

#endif//_CBIGNUM_H
