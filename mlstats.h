/* $Id: mlstats.h,v 1.1.1.1 2003/07/09 12:05:16 tim Exp $
 *
 * ML stats form code
 * Created: March 31st 2003
 */

#ifndef MLSTATS_H
#define MLSTATS_H

#include "mldonkey.h"

#define STATS_LABEL_MAXLEN 20

extern void MLstatsFooterCb(MLcoreCode opc, UInt32 dataSize);
extern Boolean StatsFormHandleEvent(EventType *event);

#endif //MLSTATS_H
