/* $Id: mlread.c,v 1.1 2003/07/24 22:17:01 tim Exp $
 *
 * All da read funcs
 * Created: July 24th 2003
 */

#include "mlread.h"


void MLreadDiscard(UInt32 size) {
  MemHandle m = MemHandleNew(4096);
  UInt32 toDiscard=size;
  Char *buffer = MemHandleLock(m);
  UInt16 toRead=0;

  while (toDiscard > 0) {

    toRead = min(4096, toDiscard);

    MLreadBytesIntoBuffer(buffer, toRead);
    toDiscard -= toRead;
  }

  MemHandleUnlock(m);
  MemHandleFree(m);
}


Err MLreadHead(UInt32 *size, MLcoreCode *opcode) {
  MLmsgHead head;

  MLreadBytesIntoBuffer((Char *)&head, sizeof(MLmsgHead));

  *size = NetSwap32(head.size)-2; // -2 for opcode
  *opcode = NetSwap16(head.opcode);

  return errNone;
}


Err MLreadBytesIntoBuffer(Char *buffer, UInt32 size) {
  Err err;
  if (size <= ML_MSG_MAX_SIZE) {
    Char *tmp;
    MemHandle m=MemHandleNew(size);
    if ((err = MLsocketRead(&m, size)) == errNone) {
      tmp = MemHandleLock(m);
      MemMove(buffer, tmp, size);
      MemHandleUnlock(m);
    }
    MemHandleFree(m);
  } else {
    return MLerrMsgTooBig;
  }

  return err;
}

  
Err MLread(UInt32 *size, MLcoreCode *opcode, MemHandle *content) {
  Err err=errNone;

  NetTrafficStart();

  if ((err = MLreadHead(size, opcode)) == errNone) {
    if ((err = MemHandleResize(*content, *size)) == errNone) {
      err = MLsocketRead(content, *size);
    }
  }
  NetTrafficStop();

return err;
}



Err MLread_UInt8(UInt8 *ui8) {
  return MLreadBytesIntoBuffer((Char *)ui8, sizeof(UInt8));
}

Err MLread_Bool(Boolean *boole) {
  UInt8 ui8;
  Err err;
  if ((err = MLreadBytesIntoBuffer((Char *)&ui8, sizeof(UInt8))) == errNone) {
    if (ui8) {
    *boole = 1;
    } else {
      *boole = 0;
    }
  }

  return err;
}

Err MLread_UInt16(UInt16 *ui16) {
  Err err = MLreadBytesIntoBuffer((Char *)ui16, sizeof(UInt16));
  *ui16 = NetSwap16(*ui16);
  return err;
}

Err MLread_UInt32(UInt32 *ui32) {
  Err err = MLreadBytesIntoBuffer((Char *)ui32, sizeof(UInt32));
  *ui32 = NetSwap32(*ui32);
  return err;
}

Err MLread_UInt64(UInt64 *ui64) {
  Err err = MLreadBytesIntoBuffer((Char *)ui64, sizeof(UInt64));
  UInt32 tmp = NetSwap32(ui64->high);
  ui64->high = NetSwap32(ui64->low);
  ui64->low = tmp;
  return err;
}

Err MLread_String(MemHandle *stringHandle) {
  UInt16 length=0;
  Err err;
  if ( ((err = MLread_UInt16(&length)) == errNone) &&
       ((err = MemHandleResize(*stringHandle, length+1) ) == errNone) ) {
     MemSet(MemHandleLock(*stringHandle), length+1, 0);
     MemHandleUnlock(*stringHandle);
     err = MLsocketRead(stringHandle, length);
  }

  return err;
}
