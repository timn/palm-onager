/* $Id: mlfiles.c,v 1.7 2003/07/17 00:22:13 tim Exp $
 *
 * ML files code
 * Created: May 28th 2003
 */

#include "mlfiles.h"
#include "mldonkey.h"
#include "net.h"
#include "tnglue.h"
#include "mlchunks.h"

MLcallbackID gMLfilesCbID;
TNlist *gMLfilesList=NULL;
UInt16 gMLfilesNumFiles=0;
Char *gMLfilesTrigger = NULL;
ProgressStatus gMLfilesProgStat;
ProgressType *gMLfilesProgress=NULL;
MLchunkInfo gMLfilesChunkInfo;
Char *gMLfilesStrings[MLFILES_NUM_STRINGS];
UInt8 gMLfilesMode=MLFILES_DING;

static void FilesFree(void) {
  TNlist *tmpList = gMLfilesList;
  UInt8 i;

  while (tmpList != NULL) {
    MLfileInfo *file = (MLfileInfo *)tmpList->data;
    MemHandleFree(file->chunks);
    MemHandleFree(file->availability);
    MemHandleFree(file->download_rate);
    MemHandleFree(file->file_age);
    MemHandleFree(file->name);
    tmpList = tmpList->next; 
  }
  TNlistFree(gMLfilesList);
  gMLfilesList = NULL;
  if (gMLfilesChunkInfo.chunks != NULL) {
    MemPtrFree(gMLfilesChunkInfo.chunks);
    gMLfilesChunkInfo.chunks = NULL;
  }
  if (gMLfilesChunkInfo.availability != NULL) {
    MemPtrFree(gMLfilesChunkInfo.availability);
    gMLfilesChunkInfo.availability = NULL;
  }
  for (i=0; i < MLFILES_NUM_STRINGS; ++i) {
    if (gMLfilesStrings[i] != NULL) {
      MemPtrFree(gMLfilesStrings[i]);
      gMLfilesStrings[i] = NULL;
    }
  }
  if (gMLfilesTrigger != NULL) {
    MemPtrFree(gMLfilesTrigger);
    gMLfilesTrigger = NULL;
  }
}


static MLfileInfo* FilesGetFile(UInt32 n) {
  TNlist *list = TNlistGetNth(gMLfilesList, n);
  
  if (list == NULL) return NULL;

  return (MLfileInfo *)list->data;
}

static void FilesListDrawFunc(Int16 itemNum, RectangleType *bounds, Char **itemsText) {
  MLfileInfo *file;
  Char *name;

  file = FilesGetFile(itemNum);
  if (file == NULL) return;

  name = MemHandleLock(file->name);
  TNDrawCharsToFitWidth(name, bounds);
  MemHandleUnlock(file->name);
}


/* THIS IS BadCode(TM)! Better solutions welcome */
#define FILES_STRING_LEN 20
static void FilesFloatToString(FlpCompDouble f, Char *dst, Char *add) {
  Char *tmp1, *tmp2, *tmp3;
  UInt32 mantissa;
  Int16 exponent, sign;

  tmp1 = MemPtrNew(FILES_STRING_LEN);
  tmp2 = MemPtrNew(FILES_STRING_LEN);
  tmp3 = MemPtrNew(FILES_STRING_LEN);
  MemSet(dst, FILES_STRING_LEN, 0);
  MemSet(tmp1, FILES_STRING_LEN, 0);
  MemSet(tmp2, FILES_STRING_LEN, 0);
  MemSet(tmp3, FILES_STRING_LEN, 0);

  FlpBase10Info(f.fd, &mantissa, &exponent, &sign);

  StrIToA(tmp1, mantissa);
  StrNCopy(tmp2, tmp1, StrLen(tmp1)+exponent);
  StrNCopy(tmp3, tmp1+StrLen(tmp1)+exponent, 1);
  // We want at least one digit
  if (StrLen(tmp3) == 0)  StrCopy(tmp3, "0");
  if (StrLen(tmp2) == 0)  StrCopy(tmp2, "0");

  StrPrintF(dst, "%s.%s %s", tmp2, tmp3, add);
  
  MemPtrFree(tmp1);
  MemPtrFree(tmp2);
  MemPtrFree(tmp3);
}


static void FilesUpdate(UInt16 n) {
  MLfileInfo *file = FilesGetFile(n);
  Char *name, *chunks, *availability, *tmp;
  ControlType *ctl=TNGetObjectPtr(FILES_trigger);
  RectangleType list_bounds;
  FormType *frm=FrmGetActiveForm();
  UInt16 numChars = 0;
  FlpCompDouble f;
  MLnetInfo *net;

  if (! file)  return;
  //FrmCustomAlert(ALERT_debug, "FilesUpdate", "", "");

  /* Trigger text */
  FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, FILES_list), &list_bounds);

  name = MemHandleLock(file->name);
  numChars = FntWordWrap(name, list_bounds.extent.x-15); // -15 for drop down arrow

  if (gMLfilesTrigger != NULL)  MemPtrFree(gMLfilesTrigger);

  gMLfilesTrigger = MemPtrNew(numChars+1);
  MemSet(gMLfilesTrigger, numChars+1, 0);
  StrNCopy(gMLfilesTrigger, name, numChars);

  CtlSetLabel(ctl, gMLfilesTrigger);
  MemHandleUnlock(file->name);
  FrmShowObject(frm, FrmGetObjectIndex(frm, FILES_trigger));

  /* Gadget Data */
  chunks =(Char *)MemHandleLock(file->chunks);
  availability =(Char *)MemHandleLock(file->availability);

  if (gMLfilesChunkInfo.chunks != NULL) {
    MemPtrFree(gMLfilesChunkInfo.chunks);
    gMLfilesChunkInfo.chunks = NULL;
  }
  if (gMLfilesChunkInfo.availability != NULL) {
    MemPtrFree(gMLfilesChunkInfo.availability);
    gMLfilesChunkInfo.availability = NULL;
  }

  gMLfilesChunkInfo.chunks = MemPtrNew(StrLen(chunks)+1);
  MemSet(gMLfilesChunkInfo.chunks, StrLen(chunks)+1, 0);
  StrNCopy(gMLfilesChunkInfo.chunks, chunks, StrLen(chunks));

  // DO NOT USE STRLEN! availability is not a string but more an array of
  // chars, availability can be 0, C then interprets this as the end of the
  // string...
  gMLfilesChunkInfo.availability = MemPtrNew(MemHandleSize(file->availability));
  MemSet(gMLfilesChunkInfo.availability, MemHandleSize(file->availability), 0);
  StrNCopy(gMLfilesChunkInfo.availability, availability, MemHandleSize(file->availability));


  FrmSetGadgetData(frm, FrmGetObjectIndex(frm, FILES_GADGET_chunks), &gMLfilesChunkInfo);   
  FrmSetGadgetHandler(frm, FrmGetObjectIndex(frm, FILES_GADGET_chunks), MLchunkGadgetHandler);

  /* Set labels */
  FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_size));
  FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_dled));
  FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_rate));
  FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_done));
  FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_net));
  // Size
  if (gMLfilesStrings[MLFILES_SIZE] != NULL) MemPtrFree(gMLfilesStrings[MLFILES_SIZE]);
  gMLfilesStrings[MLFILES_SIZE] = MemPtrNew(NET_TRAFFIC_MAXLEN);
  NetTrafficStringFromBytes(file->size, gMLfilesStrings[MLFILES_SIZE]);
  ctl = TNGetObjectPtr(FILES_size);
  CtlSetLabel(ctl, gMLfilesStrings[MLFILES_SIZE]);

  // Downloaded
  if (gMLfilesStrings[MLFILES_DLED] != NULL) MemPtrFree(gMLfilesStrings[MLFILES_DLED]);
  gMLfilesStrings[MLFILES_DLED] = MemPtrNew(NET_TRAFFIC_MAXLEN);
  NetTrafficStringFromBytes(file->downloaded, gMLfilesStrings[MLFILES_DLED]);
  ctl = TNGetObjectPtr(FILES_dled);
  CtlSetLabel(ctl, gMLfilesStrings[MLFILES_DLED]);
  if (gMLfilesMode == MLFILES_DING) {
    FrmShowObject(frm, FrmGetObjectIndex(frm, FILES_dled));
    FrmShowObject(frm, FrmGetObjectIndex(frm, FILES_dled_label));
  } else {
    FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_dled_label));
  }

  // Download rate
  if (gMLfilesStrings[MLFILES_RATE] != NULL) MemPtrFree(gMLfilesStrings[MLFILES_RATE]);
  tmp = MemHandleLock(file->download_rate);
  FlpBufferAToF(&f.fd, tmp);
  f.d /= 1024;
  MemHandleUnlock(file->download_rate);
  gMLfilesStrings[MLFILES_RATE] = MemPtrNew(FILES_STRING_LEN);
  FilesFloatToString(f, gMLfilesStrings[MLFILES_RATE], "KB/s");
  ctl = TNGetObjectPtr(FILES_rate);
  CtlSetLabel(ctl, gMLfilesStrings[MLFILES_RATE]);
  if (gMLfilesMode == MLFILES_DING) {
    FrmShowObject(frm, FrmGetObjectIndex(frm, FILES_rate));
    FrmShowObject(frm, FrmGetObjectIndex(frm, FILES_rate_label));
  } else {
    FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_rate_label));
  }

  // Percent Done
  if (gMLfilesStrings[MLFILES_DONE] != NULL) MemPtrFree(gMLfilesStrings[MLFILES_DONE]);
  f.d = file->downloaded;
  f.d /= file->size;
  f.d *= 100;
  gMLfilesStrings[MLFILES_DONE] = MemPtrNew(FILES_STRING_LEN);
  FilesFloatToString(f, gMLfilesStrings[MLFILES_DONE], "%");
  ctl = TNGetObjectPtr(FILES_done);
  CtlSetLabel(ctl, gMLfilesStrings[MLFILES_DONE]);
  if (gMLfilesMode == MLFILES_DING) {
    FrmShowObject(frm, FrmGetObjectIndex(frm, FILES_done));
    FrmShowObject(frm, FrmGetObjectIndex(frm, FILES_done_label));
  } else {
    FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_done_label));
  }

  // Network Name
  if (gMLfilesStrings[MLFILES_NET] != NULL) MemPtrFree(gMLfilesStrings[MLFILES_NET]);
  net = MLnetworkGetByID(file->net_num);
  if (! net) {
    FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_net_label));
    FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_net));
  } else {
    tmp = MemHandleLock(net->name);
    gMLfilesStrings[MLFILES_NET] = MemPtrNew(StrLen(tmp)+1);
    MemSet(gMLfilesStrings[MLFILES_NET], StrLen(tmp)+1, 0);
    StrNCopy(gMLfilesStrings[MLFILES_NET], tmp, StrLen(tmp));
    MemHandleUnlock(net->name);
    ctl = TNGetObjectPtr(FILES_net);
    CtlSetLabel(ctl, gMLfilesStrings[MLFILES_NET]);
    FrmShowObject(frm, FrmGetObjectIndex(frm, FILES_net));
    FrmShowObject(frm, FrmGetObjectIndex(frm, FILES_net_label));
  }

  
  FrmShowObject(frm, FrmGetObjectIndex(frm, FILES_size));
  FrmShowObject(frm, FrmGetObjectIndex(frm, FILES_size_label));

  // Draw the form to get data displayed
  FrmDrawForm(frm);

}


static void FilesFinished(void) {
  ListType *lst;
  ControlType *ctl;
  FormType *frm;

  PrgStopDialog(gMLfilesProgress, true);
  NetTrafficEnable();
  NetTrafficStop();

  lst = TNGetObjectPtr(FILES_list);
  ctl = TNGetObjectPtr(FILES_trigger);
  frm = FrmGetActiveForm();

  //if (! gMLfilesNumFiles) {
    // No files -> hide display items, show no data string


  LstSetListChoices(lst, NULL, gMLfilesNumFiles);
  LstSetDrawFunction(lst, FilesListDrawFunc);
  LstSetHeight(lst, min(gMLfilesNumFiles, MLFILES_MAX_LISTHEIGHT));
  FilesUpdate(0);
  
}


static Boolean FilesProgress(PrgCallbackDataPtr cbP) {
  Char *temp_string;
  ProgressStatus *p=(ProgressStatus *)cbP->userDataP;

  if (cbP->stage == 0)  return true;

  if (cbP->textP != NULL) MemPtrFree((MemPtr)cbP->textP);
  temp_string=(Char *) MemPtrNew(cbP->textLen);
  MemSet(temp_string, sizeof(temp_string), 0);

  if (cbP->stage == 1) {
    StrCopy(temp_string, "Requested file list from server\nWaiting for response");
  } else if ((p != NULL) && (p->current == p->max)) {
    StrPrintF(temp_string, "Download done.\n%u file specs received", p->max);
    cbP->delay=true;
  } else {
    if (p != NULL) StrPrintF(temp_string, "Downloading file list...\nWorking on %u of %u", p->current, p->max);
    else StrCopy(temp_string, "Downloading file list...");

    if (cbP->message != NULL) StrNCat(temp_string, cbP->message, cbP->textLen-1);
  }

  cbP->textP = temp_string;
  cbP->bitmapId = BITMAP_progress_start + (p->current % 3);

  return true;
}

static void FilesFormInit(FormType *frm) {
  MemHandle m;
  Char *tmp, *tmp2;
  
  FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_trigger));
  FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_size));
  FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_size_label));
  FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_dled));
  FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_dled_label));
  FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_rate));
  FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_rate_label));
  FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_done));
  FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_done_label));
  FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_net));
  FrmHideObject(frm, FrmGetObjectIndex(frm, FILES_net_label));

  FrmDrawForm(frm);
  FilesFree();

  m = DmGetResource(strRsc, FILES_string_ding+gMLfilesMode);
  tmp = MemHandleLock(m);
  tmp2 = MemPtrNew(StrLen(tmp)+1);
  MemSet(tmp2, StrLen(tmp)+1, 0);
  StrNCopy(tmp2, tmp, StrLen(tmp));
  MemHandleUnlock(m);
  FrmSetTitle(frm, tmp2);
  if (gMLfilesStrings[MLFILES_TITL] != NULL)  MemPtrFree(gMLfilesStrings[MLFILES_TITL]);
  gMLfilesStrings[MLFILES_TITL] = tmp2;
  FrmDrawForm(frm);

  gMLfilesProgStat.max=1;
  gMLfilesProgStat.current=0;

  NetTrafficStart();
  NetTrafficDisable();
  gMLfilesProgress = PrgStartDialog("Downloading file list", FilesProgress, &gMLfilesProgStat);
  PrgUpdateDialog (gMLfilesProgress, 0, 1, NULL, true);

  if (gMLfilesMode == MLFILES_DING) {
    MLrequest(GetDownloadFiles);
  } else {
    MLrequest(GetDownloadedFiles);
  }
}


void FilesSetMode(UInt8 newMode) {
  gMLfilesMode = newMode;
}


Boolean FilesFormHandleEvent(EventType *event) {
  Boolean handled=false;
  FormType *frm=FrmGetActiveForm();

  
  if (event->eType == ctlSelectEvent) {
    // button handling
    
    switch (event->data.ctlSelect.controlID) {
      case FILES_disco:
        MLdisconnect();
        FrmGotoForm(FORM_main);
        handled = true;
        break;

      case FILES_ding:
        gMLfilesMode = MLFILES_DING;
        FilesFormInit(frm);
        handled = true;
        break;

      case FILES_ded:
        gMLfilesMode = MLFILES_DLED;
        FilesFormInit(frm);
        handled = true;
        break;

      case FILES_stats:
        FrmGotoForm(FORM_stats);
        handled = true;
        break;
      
      default:
        break;
    }
  } else if (event->eType == popSelectEvent) {
    FilesUpdate(event->data.popSelect.selection);
    handled = true;
  } else if (event->eType == menuEvent) {
    // forwarding of menu events
    return HandleMenuEvent(event->data.menu.itemID);
  } else if (event->eType == frmUpdateEvent) {
    // redraws the form if needed
    FrmDrawForm (frm);
    handled = true;
  } else if (event->eType == frmOpenEvent) {
    // initializes and draws the form at program launch
    UInt8 i;
    gMLfilesChunkInfo.chunks = NULL;
    gMLfilesChunkInfo.availability = NULL;
    for (i=0; i < MLFILES_NUM_STRINGS; ++i) {
      gMLfilesStrings[i]=NULL;
    }
    FilesFormInit(frm);
    handled = true;
  } else if (event->eType == frmCloseEvent) {
    // this is done if program is closed
    FilesFree();
  }
  
  return handled;
}




void MLfilesCb(MLcoreCode opc, UInt32 dataSize) {
  Err err;
  MLcoreCode opcode=0;
  UInt32 size=0;
  UInt16 numFiles = 0, i=0;
  
  if ((err = MLreadHead(&size, &opcode)) != errNone) {
    return;
  }

  if ((err = MLread_UInt16(&numFiles)) != errNone) {
    return;
  }

  gMLfilesProgStat.max = numFiles;
  
  for (i=0; i < numFiles; ++i) {
    UInt32 tmp32=0;
    UInt16 tmp16=0, tmpNum=0, j=0;
    UInt8  tmp8=0;
    MLfileInfo *file = MemPtrNew(sizeof(MLfileInfo));
    MemSet(file, MemPtrSize(file), 0);
    
    gMLfilesProgStat.current = i+1;
    PrgUpdateDialog (gMLfilesProgress, 0, (i%2)+2, NULL, true);

    MLread_UInt32(&file->file_num);
    MLread_UInt32(&file->net_num);
    MLread_UInt16(&tmpNum);
    
    // We do not care about those names, we only want the primary name given later
    for (j=0; j < tmpNum; ++j) {
      MLread_UInt16(&tmp16);
      MLreadDiscard(tmp16);
    }

    MLreadBytesIntoBuffer(file->md4, 16);

    MLread_UInt32(&file->size);
    MLread_UInt32(&file->downloaded);
    MLread_UInt32(&file->num_locations);
    MLread_UInt32(&file->num_clients);
    MLread_UInt8(&file->state);

    file->chunks = MemHandleNew(10);
    MLread_String(&file->chunks);

    file->availability = MemHandleNew(10);
    MLread_String(&file->availability);

    file->download_rate = MemHandleNew(4);
    MLread_String(&file->download_rate);

    MLread_UInt16(&tmpNum);

    // We do not care about chunk ages
    for (j=0; j < tmpNum; ++j) {
      MLread_UInt16(&tmp16);
      MLreadDiscard(tmp16);      
    }

    file->file_age = MemHandleNew(11);
    MLread_String(&file->file_age);

    // This really sucks, read format and this may change in length, form and
    // everything :-/ Should be made optional in my opinion.
    // We do not care and just discard it
    MLread_UInt8(&tmp8);
    switch(tmp8) {
      case 0: break;
      case 1: {
        // Two strings, seems "generic".
        MLread_UInt16(&tmp16);
        MLreadDiscard(tmp16);      
        MLread_UInt16(&tmp16);
        MLreadDiscard(tmp16);      
      }; break;
      case 2: {
        // AVI
        // Codec string followed by width, height, fps and rate
        MLread_UInt16(&tmp16);
        MLreadDiscard(tmp16);      
        MLread_UInt32(&tmp32);
        MLread_UInt32(&tmp32);
        MLread_UInt32(&tmp32);
        MLread_UInt32(&tmp32);
      }; break;
      case 3: {
        // MP3
        // 5 Strings for title, artist, album, year, comment
        // two ints for tracknum and genre
        for (j=0; j < 5; ++j) {
          MLread_UInt16(&tmp16);
          MLreadDiscard(tmp16);      
        }
        MLread_UInt32(&tmp32);
        MLread_UInt32(&tmp32);
      }; break;
    }


    file->name = MemHandleNew(20);
    MLread_String(&file->name);

    MLread_UInt32(&file->last_seen);
    MLread_UInt32(&file->priority);

    gMLfilesList = TNlistAppend(gMLfilesList, file);
  }

  gMLfilesNumFiles = numFiles;
  FilesFinished();

}
