/* $Id: mlfiles.h,v 1.3 2003/07/16 16:45:08 tim Exp $
 *
 * ML files code
 * Created: May 28th 2003
 */

#ifndef MLFILES_H
#define MLFILES_H

#include "mldonkey.h"

#define MLFILES_DING 0
#define MLFILES_DLED 1

#define MLFILES_MAX_LISTHEIGHT 6
#define MLFILES_NUM_STRINGS 5

#define MLFILES_SIZE 0
#define MLFILES_DLED 1
#define MLFILES_RATE 2
#define MLFILES_DONE 3
#define MLFILES_NET 4

extern void MLfilesFree(void);
extern void MLfilesCb(MLcoreCode opc, UInt32 dataSize);
extern void FilesSetMode(UInt8 newMode);
extern Boolean FilesFormHandleEvent(EventType *event);

#endif //MLFILES_H
