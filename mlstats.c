/* $Id: mlstats.c,v 1.3 2003/07/22 18:08:30 tim Exp $
 *
 * ML stats form code
 * Created: March 31st 2003
 */

#include "mlstats.h"
#include "mlfiles.h"
#include "net.h"
#include "tnglue.h"

MLcallbackID gMLstatsCbID;
extern ProgressType *gMLconnectProgress;

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

  if (gMLconnectProgress != NULL) {
    PrgUpdateDialog(gMLconnectProgress, errNone, 3, NULL, true);
    PrgStopDialog(gMLconnectProgress, true);
    gMLconnectProgress = NULL;
    FrmGotoForm(FORM_stats);
    NetTrafficEnable();
  }

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
        FilesSetMode(MLFILES_DING);
        FrmGotoForm(FILES_form);
        handled = true;
        break;

      case STATS_ded:
        FilesSetMode(MLFILES_DLED);
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
