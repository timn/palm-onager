/* $Id: mlread.h,v 1.1 2003/07/24 22:17:01 tim Exp $
 *
 * All da read funcs
 * Created: July 24th 2003
 */

#ifndef MLREAD_H
#define MLREAD_H

#include "mldonkey.h"
#include "tnglue.h"

extern void MLreadDiscard(UInt32 size);

extern Err MLreadBytesIntoBuffer(Char *buffer, UInt32 size);

extern Err MLreadHead(UInt32 *size, MLcoreCode *opcode);
extern Err MLread(UInt32 *size, MLcoreCode *opcode, MemHandle *content);

extern Err MLread_Bool(Boolean *boole);
extern Err MLread_UInt8(UInt8 *ui8);
extern Err MLread_UInt16(UInt16 *ui16);
extern Err MLread_UInt32(UInt32 *ui32);
extern Err MLread_UInt64(UInt64 *ui64);
extern Err MLread_String(MemHandle *stringHandle);

#endif // MLREAD_H
