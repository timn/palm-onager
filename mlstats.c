/* $Id: mlstats.c,v 1.1.1.1 2003/07/09 12:05:16 tim Exp $
 *
 * ML stats form code
 * Created: March 31st 2003
 */

#include "mlstats.h"
#include "mlchunks.h"
#include "net.h"
#include "tnglue.h"

MLcallbackID gMLstatsCbID;

static void MLstatsDraw(MLinfo *info) {
  UInt32 downed_high=0, downed_low=0, upped_high=0, upped_low=0;
  UInt8 n;
  Char temp[STATS_LABEL_MAXLEN], unit[NET_TRAFFICUNIT_MAXLEN];
  RectangleType rect;
  FormType *frm;

  n = NetUInt64ToBytes(info->download_counter, &downed_high, &downed_low, true);
  NetTrafficUnit(unit, n);
  StrPrintF(temp, "%lu.%lu %s ", downed_high, downed_low, unit);

  frm = FrmGetActiveForm();
  FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, STATS_LABEL_down), &rect);
  WinEraseRectangle(&rect, 0);
  FrmCopyLabel(frm, STATS_LABEL_down, temp); 


  n = NetUInt64ToBytes(info->upload_counter, &upped_high, &upped_low, true);
  NetTrafficUnit(unit, n);
  StrPrintF(temp, "%lu.%lu %s ", upped_high, upped_low, unit);

  frm = FrmGetActiveForm();
  FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, STATS_LABEL_up), &rect);
  WinEraseRectangle(&rect, 0);

  FrmCopyLabel(frm, STATS_LABEL_up, temp); 
}


void MLstatsFooterCb(MLcoreCode opc, UInt32 dataSize) {
  UInt32 bytes;
  MLcoreCode opcode;
  MemHandle data=MemHandleNew(1);

/*
  if (req == 1) {
    MLbuffer_create(0);
    MLbuffer_write(GetDownloadFiles);
    MLbuffer_destroy();
  }
  req++;
*/
  
  if (MLread(&bytes, &opcode, &data) == errNone) {
    // Successfully read packet

    if (opcode == Client_stats_v4) {
      Char temp[100];
      MLinfo *info = (MLinfo *)MemHandleLock(data);
      RectangleType rect;
      UInt32 download, upload;

      if (FrmGetFormId(FrmGetActiveForm()) == FORM_stats) {
        MLstatsDraw(info);
      }

      download = NetSwap32(info->tcp_download_rate) + NetSwap32(info->udp_download_rate);
      upload = NetSwap32(info->tcp_upload_rate) + NetSwap32(info->udp_upload_rate);
      StrPrintF(temp, "Down: %lu.%lu  KB/s  Up: %lu.%lu KB/s",
                      download / 1024, (download % 1024) / 103,
                      upload / 1024, (upload % 1024) / 103);

      RctSetRectangle(&rect, (160-FntLineWidth(temp, StrLen(temp)))/2, 160-FntLineHeight(), FntLineWidth(temp, StrLen(temp)), FntLineHeight());
      TNDrawCharsToFitWidth(temp, &rect);
      MemHandleUnlock(data);
    }
  }

  MemHandleFree(data);
}


Boolean StatsFormHandleEvent(EventType *event) {
  Boolean handled=false;
  FormType *frm;

  if (event->eType == ctlSelectEvent) {
    // button handling
    switch (event->data.ctlSelect.controlID) {

      case STATS_disco:
        MLdisconnect();
        FrmGotoForm(FORM_main);
        handled = true;
        break;

      case STATS_ding:
        FrmGotoForm(FILES_form);
        handled = true;
        break;

      default:
        break;
    }
  } else if (event->eType == menuEvent) {
    // forwarding of menu events
    return HandleMenuEvent(event->data.menu.itemID);
  } else if (event->eType == frmUpdateEvent) {
    // redraws the form if needed
    frm = FrmGetActiveForm();
    FrmDrawForm (frm);
    handled = true;
  } else if (event->eType == frmOpenEvent) {
    // initializes and draws the form at program launch
    frm = FrmGetActiveForm();
    FrmDrawForm (frm);
    handled = true;
  } else if (event->eType == frmCloseEvent) {
    // this is done if program is closed
  }
  
  return handled;
}
