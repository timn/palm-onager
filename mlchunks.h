/* $Id: mlchunks.h,v 1.1 2003/07/09 12:05:16 tim Exp $
 *
 * ML chunk gadget code
 * Created: April 10th 2003
 */

#ifndef MLCHUNKS_H
#define MLCHUNKS_H

#include "mldonkey.h"

extern Boolean MLchunkGadgetHandler(FormGadgetTypeInCallback *gadget, UInt16 cmd, void *param);
extern void MLchunkGadgetDraw(FormGadgetTypeInCallback *gadget);



#endif // MLCHUNKS_H
