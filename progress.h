/* $Id: progress.h,v 1.1 2003/07/23 22:20:42 tim Exp $
 *
 * Progress comfort code
 * Created: July 23rd 2003
 */
 
 #ifndef PROGRESS_H
#define PROGRESS_H

#include "UniDonkey.h"
#include "tnglue.h"

extern void ProgressStartCustom(UInt16 textBase, UInt16 bitmapBase, UInt8 cycleBitmaps, void *data, PrgCallbackFunc cb);
extern void ProgressStart(UInt16 textBase, UInt16 bitmapBase, UInt8 cycleBitmaps, void *data);
extern void ProgressUpdate(Err err, UInt16 stage, void *message, Boolean delay);
extern void ProgressStop(void);
extern Boolean ProgressHandleEvent(EventType *event);


#endif // PROGRESS_H
