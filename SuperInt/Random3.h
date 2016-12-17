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

#ifndef RANDOM_H
#define RANDOM_H

#ifdef  __cplusplus
extern "C" {
#endif/*__cplusplus*/

/* --------------------------------------------------------------
    Original functions.
-------------------------------------------------------------- */

extern  void            SRand3 (unsigned long, unsigned long, unsigned long);
                                        /* Set 32 bit random seed */
extern  unsigned long   Rand3  (void);  /* Get 32 bit random value */

/* --------------------------------------------------------------
   Additional functions.
-------------------------------------------------------------- */

extern  void            RRand3 (void);  /* Initialize random generator */
extern  unsigned long   lRand3 (void);  /* Get random value in long range */
extern  double          dRand3 (void);  /* Get random value in a range [0,1) */

#ifdef  __cplusplus
}
#endif/*__cplusplus*/

#endif/*RANDOM_H*/
