/* $Id: mlfiles.h,v 1.1 2003/07/09 12:05:16 tim Exp $
 *
 * ML files code
 * Created: May 28th 2003
 */

#ifndef MLFILES_H
#define MLFILES_H

#include "mldonkey.h"

extern void MLfilesFree(void);
extern void MLfilesCb(MLcoreCode opc, UInt32 dataSize);
Boolean FilesFormHandleEvent(EventType *event);

#endif //MLFILES_H
