/* $Id: mldonkey.h,v 1.1 2003/07/09 12:05:16 tim Exp $
 *
 * Functions to deal with MLdonkey
 * Created: March 13th 2003
 */

#ifndef MLDONKEY_H
#define MLDONKEY_H

#include "UniDonkey.h"
#include "mltypes.h"
#include "mlcallback.h"

// Proto: 14 (BIG Endian hex defined)
#define MLDONKEY_PROTO_VER 0x0000000e

#define MLDEBUG
#define UInt16_MAX 65535
#define ML_MSG_MAX_SIZE 32768

#define ML_MAX_CALLBACKS 5

#define sleep(x) SysTaskDelay((x)*sysTicksPerSecond)

#define MLzeroConfig(c) MemSet(c, sizeof(MLconfig), 0);


extern Err MLsocket(MLconfig *config, NetSocketRef *socket);
extern Boolean MLsocketOpen(NetSocketRef socket, Err *err);
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

extern Err MLwriteSocket(MLguiCode opcode, MemHandle *content, NetSocketRef socket);
extern Err MLwrite(MLguiCode opcode, MemHandle *content);

extern void MLbuffer_create(UInt16 size);
extern void MLbuffer_append_Data(void *appData, UInt16 appLen);
extern void MLbuffer_append_String(Char *appStr);
extern void MLbuffer_append_UInt32(UInt32 ui32);
extern void MLbuffer_append_UInt16(UInt16 ui16);
extern void MLbuffer_append_UInt8(UInt8 ui8);
extern void MLbuffer_write(MLguiCode opcode);
extern void MLbuffer_destroy(void);
extern void MLrequest(MLguiCode opcode);

#endif // MLDONKEY_H
