/* $Id: mlcallback.h,v 1.1 2003/07/09 12:05:16 tim Exp $
 *
 * Functions for mldonkey callback stuff
 * Created: April 7th 2003
 */

#ifndef MLCALLBACK_H
#define MLCALLBACK_H

#include "mldonkey.h"

extern Err MLcallbackRegister(MLcoreCode coreCode, MLcallbackID *callbackID, MLcallbackFunc *func);
extern Int16 MLcallbackCompareID(void *a, void *b);
extern Int16 MLcallbackCompareCC(void *a, void *b);
extern Err MLcallbackUnregister(MLcallbackID callbackID);
extern void MLcallbackFindAndRun(MLcoreCode opcode, UInt32 bytes);

#endif //MLCALLBACK_H
