/* $Id: mldonkey.h,v 1.5 2003/07/24 21:31:41 tim Exp $
 *
 * Functions to deal with MLdonkey
 * Created: March 13th 2003
 */

#ifndef MLDONKEY_H
#define MLDONKEY_H

#include "UniDonkey.h"
#include "mltypes.h"
#include "mlcallback.h"
#include "mlbuffer.h"

// Proto: 14 (BIG Endian hex defined)
#define MLDONKEY_PROTO_VER 0x0000000e

#define MLDEBUG
#define UInt16_MAX 65535
#define ML_MSG_MAX_SIZE 32768

#define ML_MAX_CALLBACKS 5

#define sleep(x) SysTaskDelay((x)*sysTicksPerSecond)

#define MLzeroConfig(c) MemSet(c, sizeof(MLconfig), 0);


extern Err MLsocket(MLconfig *config, NetSocketRef *socket);
extern Boolean MLsocketIsOpen(NetSocketRef socket, Err *err);
extern Err MLsocketWrite(MLguiCode opcode, MemHandle *content);

extern Err MLconnect(MLconfig *config);
extern Err MLdisconnect(void);

extern Err MLprocess(void);
extern Boolean MLdataWaiting(UInt32 *size, MLcoreCode *opcode);

extern void MLreadPrepare(UInt32 size, MLcoreCode opcode);
extern UInt32 MLreadLeft(MLcoreCode *opc);
extern Err MLreadHead(UInt32 *size, MLcoreCode *opcode);
extern Err MLreadBytes(MemHandle *data, UInt32 size);
extern Err MLreadBytesIntoBuffer(Char *buffer, UInt32 size);
extern void MLreadDiscard(UInt32 size);
extern Err MLread(UInt32 *size, MLcoreCode *opcode, MemHandle *content);
extern Err MLread_UInt8(UInt8 *ui8);
extern Err MLread_UInt16(UInt16 *ui16);
extern Err MLread_UInt32(UInt32 *ui32);
extern Err MLread_UInt64(UInt64 *ui64);
extern Err MLread_String(MemHandle *stringHandle);

extern MLnetInfo* MLnetworkGetByID(UInt32 id);

#endif // MLDONKEY_H
