/* $Id: mldonkey.h,v 1.6 2003/07/24 22:17:01 tim Exp $
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
#include "mlread.h"

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
extern Err MLsocketRead(MemHandle *data, UInt32 size);

extern Err MLconnect(MLconfig *config);
extern Err MLdisconnect(void);

extern Err MLprocess(void);
extern Boolean MLdataWaiting(UInt32 *size, MLcoreCode *opcode);


extern MLnetInfo* MLnetworkGetByID(UInt32 id);

#endif // MLDONKEY_H
