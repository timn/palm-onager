/* $Id: mlprefs.h,v 1.1 2003/07/09 12:05:16 tim Exp $
 *
 * Preferences code
 */

#ifndef MLPREFS_H
#define MLPREFS_H

#include "mldonkey.h"

#define ML_PREFS_VERSION 1
#define ML_PREFS_ID 1


typedef struct {
  Char hostname[ML_CONFIG_FIELD_MAXLEN+1];
  Char login[ML_CONFIG_FIELD_MAXLEN+1];
  Char password[ML_CONFIG_FIELD_MAXLEN+1];
  UInt16 port;
} MLprefs;

extern void MLprefsLoad(MLconfig *config);
extern void MLprefsSave(MLconfig *config);

#endif //MLPREFS_H
