/* --------------------------------------------------------------
    Timer and priority functions.

    Copyright 1998-2009 by Dr. Raul N.Shakirov, IMach of RAS(UB).
    All Rights Reserved.

    Permission has been granted to copy, distribute and modify
    software in any context without fee, including a commercial
    application, provided that the aforesaid copyright statement
    is present here as well as exhaustive description of changes.

    THE SOFTWARE IS DISTRIBUTED "AS IS". NO WARRANTY OF ANY KIND
    IS EXPRESSED OR IMPLIED. YOU USE AT YOUR OWN RISK. THE AUTHOR
    WILL NOT BE LIABLE FOR DATA LOSS, DAMAGES, LOSS OF PROFITS OR
    ANY OTHER KIND OF LOSS WHILE USING OR MISUSING THIS SOFTWARE.
-------------------------------------------------------------- */
#ifdef  __cplusplus
extern "C" {
#endif/*__cplusplus*/

long    GetTimer (long timer);
int     SetHighPriority (void);         /* true on success */
int     SetIdlePriority (void);         /* true on success */
int     ResetPriority   (void);         /* true on success */

#ifdef  __cplusplus
}
#endif/*__cplusplus*/
