/* $Id: mlbuffer.h,v 1.1 2003/07/24 21:31:41 tim Exp $
 *
 * Write buffer convenience stuff
 * Created: July 24th 2003
 */

#ifndef MLBUFFER_H
#define MLBUFFER_H

#include "mldonkey.h"

extern void MLbuffer_create(void);
extern void MLbuffer_append_Data(void *appData, UInt16 appLen);
extern void MLbuffer_append_String(Char *appStr);
extern void MLbuffer_append_UInt32(UInt32 ui32);
extern void MLbuffer_append_UInt16(UInt16 ui16);
extern void MLbuffer_append_UInt8(UInt8 ui8);
extern void MLbuffer_write(MLguiCode opcode);
extern void MLbuffer_destroy(void);
extern void MLrequest(MLguiCode opcode);

#endif // MLBUFFER_H
