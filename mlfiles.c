/* $Id: mlfiles.c,v 1.1.1.1 2003/07/09 12:05:16 tim Exp $
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
TNlist *gMLfilesList;
UInt16 gMLfilesNumFiles=0;
Char *gMLfilesTrigger = NULL;
ProgressStatus gMLfilesProgStat;
ProgressType *gMLfilesProgress;
MLchunkInfo gMLfilesChunkInfo;

void MLfilesFree(void) {
  TNlist *tmpList = gMLfilesList;
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
  if (gMLfilesChunkInfo.chunks != NULL) {
    MemPtrFree(gMLfilesChunkInfo.chunks);
    gMLfilesChunkInfo.chunks = NULL;
  }
  if (gMLfilesChunkInfo.availability != NULL) {
    MemPtrFree(gMLfilesChunkInfo.availability);
    gMLfilesChunkInfo.availability = NULL;
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


static void FilesSetTriggerAndGadget(UInt16 n) {
  MLfileInfo *file = FilesGetFile(n);
  Char *name, *chunks, *availability;
  ControlType *ctl=TNGetObjectPtr(FILES_trigger);
  RectangleType list_bounds;
  FormType *frm=FrmGetActiveForm();
  UInt16 numChars = 0;

  if (! file)  return;

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

  gMLfilesChunkInfo.availability = MemPtrNew(StrLen(availability)+1);
  MemSet(gMLfilesChunkInfo.availability, StrLen(availability)+1, 0);
  StrNCopy(gMLfilesChunkInfo.availability, availability, StrLen(availability));

  MemHandleUnlock(file->chunks);
  MemHandleUnlock(file->availability);

  FrmSetGadgetData(frm, FrmGetObjectIndex(frm, FILES_GADGET_chunks), &gMLfilesChunkInfo);   
  FrmSetGadgetHandler(frm, FrmGetObjectIndex(frm, FILES_GADGET_chunks), MLchunkGadgetHandler);

  FrmDrawForm(frm);
  
}


static void FilesFinished(void) {
  ListType *lst;
  ControlType *ctl;
  PrgStopDialog(gMLfilesProgress, true);
  NetTrafficEnable();
  lst = TNGetObjectPtr(FILES_list);
  ctl = TNGetObjectPtr(FILES_trigger);
  LstSetListChoices(lst, NULL, gMLfilesNumFiles);
  LstSetDrawFunction(lst, FilesListDrawFunc);
  FilesSetTriggerAndGadget(0);
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
    StrCopy(temp_string, "Download done.");
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
      
      default:
        break;
    }
  } else if (event->eType == popSelectEvent) {
    FilesSetTriggerAndGadget(event->data.popSelect.selection);
    handled = true;
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

    gMLfilesProgStat.max=1;
    gMLfilesProgStat.current=0;

    gMLfilesProgress = PrgStartDialog("Downloading file list", FilesProgress, &gMLfilesProgStat);
    PrgUpdateDialog (gMLfilesProgress, 0, 1, NULL, true);
    NetTrafficDisable();
    MLrequest(GetDownloadFiles);

    /*
    FrmSetGadgetData(frm, FrmGetObjectIndex(frm, STATS_GADGET_chunks), gChunkString);   
    FrmSetGadgetHandler(frm, FrmGetObjectIndex(frm, STATS_GADGET_chunks), MLchunkGadgetHandler);
    */

    handled = true;
  } else if (event->eType == frmCloseEvent) {
    // this is done if program is closed
    if (gMLfilesTrigger != NULL) {
      MemPtrFree(gMLfilesTrigger);
      gMLfilesTrigger = NULL;
    }
  }
  
  return handled;
}




void MLfilesCb(MLcoreCode opc, UInt32 dataSize) {
  Err err;
  MLcoreCode opcode=0;
  UInt32 size=0;
  UInt16 numFiles = 0, i=0;
  
  NetTrafficStart();
  if ((err = MLreadHead(&size, &opcode)) != errNone) {
    NetTrafficStop();
    return;
  }

  if ((err = MLread_UInt16(&numFiles)) != errNone) {
    NetTrafficStop();
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
