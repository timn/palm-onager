/* $Id: mlfiles.h,v 1.2 2003/07/10 10:01:47 tim Exp $
 *
 * ML files code
 * Created: May 28th 2003
 */

#ifndef MLFILES_H
#define MLFILES_H

#include "mldonkey.h"

#define MLFILES_MAX_LISTHEIGHT 6
#define MLFILES_NUM_STRINGS 4

#define MLFILES_SIZE 0
#define MLFILES_DLED 1
#define MLFILES_RATE 2
#define MLFILES_DONE 3

extern void MLfilesFree(void);
extern void MLfilesCb(MLcoreCode opc, UInt32 dataSize);
Boolean FilesFormHandleEvent(EventType *event);

#endif //MLFILES_H
