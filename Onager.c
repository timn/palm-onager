/* $Id: Onager.c,v 1.3 2003/07/23 22:20:42 tim Exp $
 *
 * UniDonkey main, event handling
 * Created: March 12th 2003
 */

#include "UniDonkey.h"
#include "tnglue.h"
#include "net.h"
#include "mldonkey.h"
#include "mlstats.h"
#include "mlprefs.h"
#include "mlchunks.h"
#include "mlfiles.h"
#include "progress.h"


Char gCategoryName[dmCategoryLength];
UInt16 gMenuCurrentForm=FORM_main;
Boolean connected=false;
MemHandle gMainMLconf=NULL;
MLconfig *gMainMLconfig=NULL;

/***********************************************************************
 * function is called at program start
 * you can put your own initialization jobs there
 ***********************************************************************/
static UInt16 StartApplication (void) {
  Err err = 0;

	// Initialize the random number seed;
	SysRandom( TimGetSeconds() );

  // Initialize TNglue
  err = TNGlueColorInit();

  gMainMLconf = MemHandleNew(sizeof(MLconfig));
  gMainMLconfig = MemHandleLock(gMainMLconf);
  MLprefsLoad(gMainMLconfig);

  // Open Database
  //if (err == errNone) {
  //  err = OpenDatabase();
  //}

  return (err);
}

/***********************************************************************
 * function is checking ROM version of the palmos operating system
 * if ROM version is less then required the result of this function is
 * sysErrRomIncompatible
 ***********************************************************************/
static Err RomVersionCompatible (UInt32 requiredVersion, UInt16 launchFlags) {
	UInt32 romVersion;

	// See if we're on in minimum required version of the ROM or later.
	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
	if (romVersion < requiredVersion) {
		if ((launchFlags & (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) ==
  			(sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) {
			FrmAlert (ALERT_ROMIncompatible);

			// PalmOS before 3.5 will continuously relaunch this app unless we switch to
			// another safe one.
			AppLaunchWithCommand(sysFileCDefaultApp, sysAppLaunchCmdNormalLaunch, NULL);
    }

    return (sysErrRomIncompatible);
  }

  return (0);
}


/***********************************************************************
 * handling for the main drop down menu actions
 ***********************************************************************/
Boolean HandleMenuEvent (UInt16 command){
  Boolean handled = false;
  MenuEraseStatus(0);
  gMenuCurrentForm=FrmGetFormId(FrmGetActiveForm());

  switch (command) {
    case MENUITEM_about:
      FrmDoDialog(FrmInitForm(FORM_about));
      handled=true;
      break;

    default:
      break;
  }

  return handled;
}


/***********************************************************************
 * handling for form and control actions
 * menu actions are forwarded to MainFormDoCommand
 ***********************************************************************/
static Boolean MainFormHandleEvent (EventPtr event){
  FormType *frm;
  Boolean handled = false;
  Char *tmp;
  
  if (event->eType == ctlSelectEvent) {
    // button handling
    handled = true;
    switch (event->data.ctlSelect.controlID) {

      case BUTTON_connect:

        // FrmCustomAlert(ALERT_debug, "HERE 0", "", "");
        tmp = FldGetTextPtr(TNGetObjectPtr(FIELD_port));
        if (tmp != NULL) gMainMLconfig->port = StrAToI(tmp);

        tmp = FldGetTextPtr(TNGetObjectPtr(FIELD_host));
        if (tmp != NULL) StrNCopy(gMainMLconfig->hostname, tmp, ML_CONFIG_FIELD_MAXLEN);

        tmp = FldGetTextPtr(TNGetObjectPtr(FIELD_user));
        if (tmp != NULL) StrNCopy(gMainMLconfig->login, tmp, ML_CONFIG_FIELD_MAXLEN);

        tmp = FldGetTextPtr(TNGetObjectPtr(FIELD_pass));
        if (tmp != NULL) StrNCopy(gMainMLconfig->password, tmp, ML_CONFIG_FIELD_MAXLEN);

        MLconnect(gMainMLconfig);
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
      MemHandle old, new;
      Char *tmp;
      FieldType *fld;

      frm = FrmGetActiveForm();
      
      if (StrLen(gMainMLconfig->hostname) > 0) {
        new = MemHandleNew(StrLen(gMainMLconfig->hostname)+1);
        fld = TNGetObjectPtr(FIELD_host);
        old = FldGetTextHandle(fld);
        tmp = MemHandleLock(new);
        MemSet(tmp, StrLen(gMainMLconfig->hostname)+1, 0);
        StrNCopy(tmp, gMainMLconfig->hostname, ML_CONFIG_FIELD_MAXLEN);
        MemHandleUnlock(new);
        FldSetTextHandle(fld, new);
        if (old != NULL) MemHandleFree(old);
      }

      if (StrLen(gMainMLconfig->login) > 0) {
        new = MemHandleNew(StrLen(gMainMLconfig->login)+1);
        fld = TNGetObjectPtr(FIELD_user);
        old = FldGetTextHandle(fld);
        tmp = MemHandleLock(new);
        MemSet(tmp, StrLen(gMainMLconfig->login)+1, 0);
        StrNCopy(tmp, gMainMLconfig->login, ML_CONFIG_FIELD_MAXLEN);
        MemHandleUnlock(new);
        FldSetTextHandle(fld, new);
        if (old != NULL) MemHandleFree(old);
      }

      if (StrLen(gMainMLconfig->password) > 0) {
        new = MemHandleNew(StrLen(gMainMLconfig->password)+1);
        fld = TNGetObjectPtr(FIELD_pass);
        old = FldGetTextHandle(fld);
        tmp = MemHandleLock(new);
        MemSet(tmp, StrLen(gMainMLconfig->password)+1, 0);
        StrNCopy(tmp, gMainMLconfig->password, ML_CONFIG_FIELD_MAXLEN);
        MemHandleUnlock(new);
        FldSetTextHandle(fld, new);
        if (old != NULL) MemHandleFree(old);
      }

      if (gMainMLconfig->port != 0) {
        new = MemHandleNew(maxStrIToALen+1);
        fld = TNGetObjectPtr(FIELD_port);
        old = FldGetTextHandle(fld);
        tmp = MemHandleLock(new);
        StrIToA(tmp, gMainMLconfig->port);
        MemHandleUnlock(new);
        FldSetTextHandle(fld, new);
        if (old != NULL) MemHandleFree(old);
      }

      FrmDrawForm(frm);
      handled = true;
    } else if (event->eType == frmCloseEvent) {
      // this is done if program is closed
    }

  return (handled);
}


/***********************************************************************
 * chooses an displays the requested form
 ***********************************************************************/
static Boolean AppHandleEvent( EventPtr eventP) {
	UInt16 formId;
	FormPtr frmP;

	switch (eventP->eType) {
    case nilEvent:
      MLprocess();
      break;

  	case frmLoadEvent:
      // Load the form resource.
		  formId = eventP->data.frmLoad.formID;
		  frmP = FrmInitForm(formId);
		  FrmSetActiveForm(frmP);

      // Set the event handler for the form.  The handler of the currently
      // active form is called by FrmHandleEvent each time is receives an
      // event.
		  switch (formId) {

        case FORM_main: FrmSetEventHandler(frmP, MainFormHandleEvent); break;
        case FORM_stats: FrmSetEventHandler(frmP, StatsFormHandleEvent); break;
        case FILES_form: FrmSetEventHandler(frmP, FilesFormHandleEvent); break;
        
        default: ErrNonFatalDisplay("Invalid Form Load Event"); break;
			}
		  break;

	  default:
		  return false;
	}
	return true;
}


/***********************************************************************
 * main event loop; loops until appStopEvent is caught or
 * QuitApp is set
 ***********************************************************************/
static void AppEventLoop(void){
	UInt16 error;
	EventType event;
  UInt16 evtTimeout = sysTicksPerSecond / 2;

	do {
		EvtGetEvent(&event, evtTimeout);

    if (! ProgressHandleEvent(&event))
      if (! SysHandleEvent(&event))
        if (! MenuHandleEvent(0, &event, &error))
          if (! AppHandleEvent(&event))
            FrmDispatchEvent(&event);

    // Check the heaps after each event
		#if EMULATION_LEVEL != EMULATION_NONE
			MemHeapCheck(0);
			MemHeapCheck(1);
		#endif

	} while (event.eType != appStopEvent);

}

/***********************************************************************
 * application is finished, so we have to clean the desktop behind us
 ***********************************************************************/
static void StopApplication (void){
  FrmCloseAllForms();

  // Handle saving and freeing of config handle
  MLprefsSave(gMainMLconfig);
  MemHandleUnlock(gMainMLconf);
  MemHandleFree(gMainMLconf);

  // Just to be sure...
  MLdisconnect();
	// CloseDatabase();
}


/***********************************************************************
 * main function
 ***********************************************************************/
UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags){
	UInt16 error = RomVersionCompatible (MINVERSION, launchFlags);
	if (error) return (error);

  /***
  * NORMAL STARTUP
  ****/
	if ( cmd == sysAppLaunchCmdNormalLaunch ) {
    error = StartApplication ();
    if (error) {
			// PalmOS before 3.5 will continuously relaunch this app unless we switch to
			// another safe one.
      FrmCustomAlert(ALERT_debug, "Please reports this bug! Give your Palm device and PalmOS version, this BadBug(TM) should not happen...", "", "");
			AppLaunchWithCommand(sysFileCDefaultApp, sysAppLaunchCmdNormalLaunch, NULL);
      return error;
    }

    FrmGotoForm(FORM_main);

		AppEventLoop ();
		StopApplication ();

  }

	return 0;
}
