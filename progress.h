/* $Id: progress.h,v 1.2 2003/07/29 20:38:53 tim Exp $
 *
 * Progress comfort code
 * Created: July 23rd 2003
 */
 
#ifndef PROGRESS_H
#define PROGRESS_H

#include "Onager.h"
#include "tnglue.h"

extern void ProgressStartCustom(UInt16 textBase, UInt16 bitmapBase, UInt8 cycleBitmaps, void *data, PrgCallbackFunc cb);
extern void ProgressStart(UInt16 textBase, UInt16 bitmapBase, UInt8 cycleBitmaps, void *data);
extern void ProgressUpdate(Err err, UInt16 stage, void *message, Boolean delay);
extern void ProgressStop(void);
extern Boolean ProgressHandleEvent(EventType *event);


#endif // PROGRESS_H
