/* $Id: progress.c,v 1.1 2003/07/23 22:20:42 tim Exp $
 *
 * Progress comfort code
 * Created: July 23rd 2003
 */
 
#include "progress.h"

UInt16 gProgTextBase=0;
UInt16 gProgBitmapBase=0;
UInt8 gProgCycleBitmaps=0;
UInt8 gProgCycleCurrent=0;
Boolean gProgDelay = false;
ProgressType *gProgProgress=NULL;

#define timeoutTicks (sysTicksPerSecond / 2)
static Boolean ProgressCallback(PrgCallbackDataPtr cbP) {
  Char *string;

  if (gProgCycleBitmaps) {
    cbP->timeout = timeoutTicks;
  }

  if (cbP->timedOut) {
    if (gProgCycleBitmaps) {
      gProgCycleCurrent = ((gProgCycleCurrent+1) % gProgCycleBitmaps);
      cbP->bitmapId = gProgBitmapBase + gProgCycleCurrent;
      cbP->textChanged = false; // do not update text
      return true;
    }
  }

  if (gProgDelay) {
    cbP->timeout = 0;
    cbP->delay = gProgDelay;
    gProgDelay = false;
  }

  if (!cbP->canceled) {  
    MemSet(cbP->textP, cbP->textLen, 0);

    string = TNGetLockedString(gProgTextBase+cbP->stage);
    StrCopy(cbP->textP, string);
    MemPtrUnlock(string);

    if (! gProgCycleBitmaps) {
      cbP->bitmapId = gProgBitmapBase;
    }
  }

  return true;
}

void ProgressStartCustom(UInt16 textBase, UInt16 bitmapBase, UInt8 cycleBitmaps, void *data, PrgCallbackFunc cb) {
  Char *string;

  gProgTextBase = textBase;
  gProgBitmapBase = bitmapBase;
  gProgCycleBitmaps = cycleBitmaps;
  string = TNGetLockedString(textBase);
  gProgProgress = PrgStartDialog(string, cb, data);
  MemPtrUnlock(string);
  PrgUpdateDialog(gProgProgress, errNone, 1, NULL, true);
}

void ProgressStart(UInt16 textBase, UInt16 bitmapBase, UInt8 cycleBitmaps, void *data) {
  ProgressStartCustom(textBase, bitmapBase, cycleBitmaps, data, ProgressCallback);
}


void ProgressUpdate(Err err, UInt16 stage, void *message, Boolean delay) {
  if (gProgProgress) {
    gProgDelay = delay;
    PrgUpdateDialog(gProgProgress, err, stage, message, true);
  }
}

void ProgressStop(void) {
  if (gProgProgress) {
    PrgStopDialog(gProgProgress, true);
    gProgProgress = NULL;
  }
}

Boolean ProgressHandleEvent(EventType *event) {
  if (gProgProgress) {
    if (! PrgHandleEvent(gProgProgress, event) && PrgUserCancel(gProgProgress)) {
      ProgressStop();
    }
  }

  return false;
}
