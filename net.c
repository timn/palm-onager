/* $Id: net.c,v 1.1 2003/07/09 12:05:16 tim Exp $
 *
 * Network related functions
 * Created: March 12th 2003
 */

#include "net.h"
#include "tnglue.h"


UInt16 gNetReference=0;
MemHandle gNetBitmapHandle=NULL;
BitmapType *gNetBitmap=NULL;
RectangleType gNetBitmapRect;
Boolean gNetTrafficEnabled = true;
 
Err NetInit(void) {
  Err err=0;
  UInt32 version=0;
  UInt16 ifErrs=0;

  gNetBitmapHandle = DmGetResource(bitmapRsc, BITMAP_net);
  gNetBitmap = (BitmapType*) MemHandleLock(gNetBitmapHandle);
  RctSetRectangle(&gNetBitmapRect, NET_BITMAP_X, NET_BITMAP_Y, NET_BITMAP_WIDTH, NET_BITMAP_HEIGHT);
  NetTrafficStart();

  err = FtrGet(netFtrCreator, netFtrNumVersion, &version);

  if ((err != errNone) || (version < NET_MINVERSION)) {
    FrmAlert(ALERT_noNet);
    err = -1;
  } else {

    err = SysLibFind("Net.lib", &gNetReference);

    if (err != errNone) {
      FrmAlert(ALERT_noNet);
    } else {

      err = NetLibOpen(gNetReference, &ifErrs);

      if ((err != errNone) && (err != netErrAlreadyOpen)) {
        // Could not load lib
        FrmAlert(ALERT_noNet);
      } else {

        if (ifErrs) {
          // Some interfaces failed
          FrmAlert(ALERT_iff);
          err = -1;
        }
      }
    }
  }

  NetTrafficStop();

  if (err != errNone) {
    MemHandleUnlock(gNetBitmapHandle);
    DmReleaseResource(gNetBitmapHandle);
  }

return err;
}

Err NetTerm(void) {
  NetLibClose(gNetReference, false);

  MemHandleUnlock(gNetBitmapHandle);
  DmReleaseResource(gNetBitmapHandle);

return errNone;
}

void NetTrafficStart(void) {
  if (gNetTrafficEnabled) {
    WinDrawBitmap(gNetBitmap, NET_BITMAP_X, NET_BITMAP_Y);
  }
}

void NetTrafficStop(void) {
  if (gNetTrafficEnabled) {
    WinEraseRectangle(&gNetBitmapRect, 0);
  }
}

void NetTrafficEnable(void) {
  gNetTrafficEnabled = true;
}

void NetTrafficDisable(void) {
  gNetTrafficEnabled = false;
}


// This operates on NETWORK BYTE ORDER!
UInt8 NetUInt64ToBytes(UInt64 ui64, UInt32 *low, UInt32 *veryLow, Boolean swap) {
  UInt32 newHigh=0, newLow=0, newVeryLow=0;
  UInt8 n=0;

  if (swap) {
    UInt32 tmp = NetSwap32(ui64.high);
    ui64.high = NetSwap32(ui64.low);
    ui64.low = tmp;
  }

  while (ui64.high || (ui64.low >= 100)) {
    // Reduce
    newHigh = ui64.high & 0x000003FF; // Only last ten bits
    newHigh <<= 22; // Shift last ten bits to first ten bits

    newVeryLow = ui64.low & 0x000003FF;
    newVeryLow /= 10;
    if (newVeryLow >= 100) {
      // Shorten it
      newVeryLow = 99;
    }

    newLow = ui64.low >> 10;
    newLow |= newHigh;

    ui64.high = newHigh;
    ui64.low = newLow;

    n += 1;
  }

  *low = ui64.low;
  *veryLow = newVeryLow;

  return n;
}

void NetTrafficUnit(Char *dst, UInt8 n) {
  Char sizes[6][NET_TRAFFICUNIT_MAXLEN]={"B\0", "KB\0", "MB\0", "GB\0", "TB\0", "PB\0"};
  MemSet(dst, NET_TRAFFICUNIT_MAXLEN, 0);
  if (n > 5) {
    StrCopy(dst, "U");
    return;
  }
  StrNCopy(dst, sizes[n], NET_TRAFFICUNIT_MAXLEN);
}
