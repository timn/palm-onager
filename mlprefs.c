/* $Id: mlprefs.c,v 1.1 2003/07/09 12:05:16 tim Exp $
 *
 * Preferences
 */

#include "mlprefs.h"

void MLprefsLoad(MLconfig *config) {
  UInt16 prefsSize=0;
  Int16 version;
  MLprefs prefs;
  
  version = PrefGetAppPreferences(APP_CREATOR, ML_PREFS_ID, NULL, &prefsSize, false);
  
  if (version == noPreferenceFound) {
    MemSet(&prefs, sizeof(MLprefs), 0);
  } else if (version != ML_PREFS_VERSION) {
    // Attempt import if old was smaller
    if (prefsSize < sizeof(MLprefs)) {

      MemHandle m=MemHandleNew(prefsSize);
      MemPtr mp = MemHandleLock(m);
  
      PrefGetAppPreferences(APP_CREATOR, ML_PREFS_ID, (Char *)mp, &prefsSize, false);
  
      MemSet(&prefs, sizeof(MLprefs), 0);
      MemMove(&prefs, mp, prefsSize);

      PrefSetAppPreferences(APP_CREATOR, ML_PREFS_ID, version, NULL, 0, false);

      MemHandleUnlock(m);
      MemHandleFree(m);
    }
  } else {
    // Load
    PrefGetAppPreferences(APP_CREATOR, ML_PREFS_ID, &prefs, &prefsSize, false);
  }

  MLzeroConfig(config);
  MemMove(config, &prefs, sizeof(MLprefs));
}

void MLprefsSave(MLconfig *config) {
  MLprefs prefs;
  MemMove(&prefs, config, sizeof(MLprefs));
  PrefSetAppPreferences(APP_CREATOR, ML_PREFS_ID, ML_PREFS_VERSION, &prefs, sizeof(MLprefs), false);
}

