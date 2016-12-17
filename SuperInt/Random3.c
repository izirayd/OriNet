/* --------------------------------------------------------------
    Tausworth3 random generator.

    Original code from Glenn Rhoads' Home Page
    http://remus.rutgers.edu/~rhoads/Code/code.html

    Adapted by Dr. Raul N.Shakirov, IMach of RAS(UB).

    Permission has been granted to copy, distribute and modify
    software in any context without fee, including a commercial
    application.

    THE SOFTWARE IS DISTRIBUTED "AS IS". NO WARRANTY OF ANY KIND
    IS EXPRESSED OR IMPLIED. YOU USE AT YOUR OWN RISK. THE AUTHOR
    WILL NOT BE LIABLE FOR DATA LOSS, DAMAGES, LOSS OF PROFITS OR
    ANY OTHER KIND OF LOSS WHILE USING OR MISUSING THIS SOFTWARE.
-------------------------------------------------------------- */
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include "Random3.h"

#define TAUS_MAX 0xFFFFFFFFUL
#define BITS    (CHAR_BIT * sizeof (long))

#if ULONG_MAX > TAUS_MAX
#define ATM & 0xFFFFFFFFUL
#else
#define ATM
#endif

#ifdef  _MSC_VER
#pragma auto_inline (off)
#endif/*_MSC_VER*/

/* --------------------------------------------------------------
    Combination of 3 tausworth generators.
-------------------------------------------------------------- */

/* The seeds */
static unsigned long s1=390451501UL, s2=613566701UL, s3=858993401UL;

/* Constant values */
#define k1 31
#define k2 29
#define k3 28

/* use either of the following two sets of parameters*/
#define q1 13
#define q2 2
#define q3 3

#define p1 12
#define p2 4
#define p3 17
/*
#define q1 3
#define q2 2
#define q3 13

#define p1 20
#define p2 16
#define p3 7
*/
#define shft1 (k1-p1)
#define shft2 (k2-p2)
#define shft3 (k3-p3)

#define mask1 ((TAUS_MAX << (32-k1)) & TAUS_MAX)
#define mask2 ((TAUS_MAX << (32-k2)) & TAUS_MAX)
#define mask3 ((TAUS_MAX << (32-k3)) & TAUS_MAX)

#ifdef  __cplusplus
extern "C" {
#endif/*__cplusplus*/

/* --------------------------------------------------------------
    Set random seed.
-------------------------------------------------------------- */

void    SRand3 (unsigned long a, unsigned long b, unsigned long c)
{
    /* Set seeds */
    if (a > (1u<<shft1)) s1 = a;
    if (b > (1u<<shft2)) s2 = b;
    if (c > (1u<<shft3)) s3 = c;

    Rand3();
}

/* --------------------------------------------------------------
    Generate random value in a range 0..TAUS_MAX
-------------------------------------------------------------- */

unsigned long Rand3 (void)
{
   unsigned long temp = (s1 ^ s2 ^ s3) ATM;

   s1 = (((s1 & mask1) << p1) ATM) ^ ((((s1 << q1) ATM) ^ s1) >> shft1);
   s2 = (((s2 & mask2) << p2) ATM) ^ ((((s2 << q2) ATM) ^ s2) >> shft2);
   s3 = (((s3 & mask3) << p3) ATM) ^ ((((s3 << q3) ATM) ^ s3) >> shft3);

   return (temp);
}

/* --------------------------------------------------------------
    Set arbitrary random seed.
-------------------------------------------------------------- */

void    RRand3 (void)
{
   /* Initialize Microsoft random generator */
   srand ((unsigned) time (NULL));

   /* Initialize tausworth3 random generator */
   SRand3 ((unsigned long) rand() * (unsigned long) rand(),
           (unsigned long) rand() * (unsigned long) rand(),
           (unsigned long) rand() * (unsigned long) rand());
}

/* --------------------------------------------------------------
    Generate random value in a range 0..ULONG_MAX
-------------------------------------------------------------- */

unsigned long lRand3 (void)
{
#if ULONG_MAX > TAUS_MAX
   unsigned long temp = Rand3();
   int sh = 32; do temp |= (Rand3() << sh); while ((sh += 32) < BITS);
   return (temp);
#else
   return Rand3();
#endif
}

/* --------------------------------------------------------------
    Generate random value in a range [0,1)
-------------------------------------------------------------- */

double  dRand3 (void)
{
    return ((double) Rand3() / ((double) TAUS_MAX + 1));
}

#ifdef  __cplusplus
}
#endif/*__cplusplus*/
