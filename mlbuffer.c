/* $Id: mlbuffer.c,v 1.2 2003/07/24 22:38:41 tim Exp $
 *
 * Write buffer convenience stuff
 * Created: July 24th 2003
 */

#include "mlbuffer.h"

MLbufferInfo *gMLbuffer=NULL;

void MLbuffer_create(void) {

  // Only one buffer at a time. If one exists it will get deleted
  if (gMLbuffer != NULL) {
    MemHandleUnlock(gMLbuffer->buffer_handle);
    MemHandleFree(gMLbuffer->buffer_handle);
    MemPtrFree(gMLbuffer);
  }
  gMLbuffer = MemPtrNew(sizeof(MLbufferInfo));
  MemSet(gMLbuffer, sizeof(MLbufferInfo), 0);

  // 1024 seems like a reasonable default value, should be big enough
  // for most commands that we will have to send
  gMLbuffer->handle_size = 1024;
  gMLbuffer->buffer_handle = MemHandleNew(gMLbuffer->handle_size);
  if (! gMLbuffer->buffer_handle) {
    MemPtrFree(gMLbuffer);
    gMLbuffer = NULL;
    return;
  }
  gMLbuffer->buffer_ptr = MemHandleLock(gMLbuffer->buffer_handle);
  gMLbuffer->buffer = (Char *)gMLbuffer->buffer_ptr;
}

static void MLbuffer_conditionalResize(UInt32 numBytes) {
  if ( (gMLbuffer->size + numBytes) > gMLbuffer->handle_size ) {
    MemHandleUnlock(gMLbuffer->buffer_handle);
    gMLbuffer->handle_size = (gMLbuffer->size + numBytes);
    MemHandleResize(gMLbuffer->buffer_handle, gMLbuffer->handle_size);
    gMLbuffer->buffer_ptr = MemHandleLock(gMLbuffer->buffer_handle);
    gMLbuffer->buffer = (Char *)gMLbuffer->buffer_ptr;
    gMLbuffer->buffer += gMLbuffer->size;
  }
}

void MLbuffer_append_Data(void *appData, UInt16 appLen) {
  if (gMLbuffer != NULL) {
    MLbuffer_conditionalResize(appLen);
    MemMove(gMLbuffer->buffer, appData, appLen);
    gMLbuffer->buffer += appLen;
    gMLbuffer->size += appLen;
  }
}

void MLbuffer_append_String(Char *appStr) {
  if (gMLbuffer != NULL) {
    UInt16 length = StrLen(appStr);
    MLbuffer_append_UInt16(length);
    MLbuffer_append_Data(appStr, length);
  }
}

void MLbuffer_append_UInt32(UInt32 ui32) {
  ui32 = NetSwap32(ui32);
  MLbuffer_append_Data(&ui32, sizeof(UInt32));
}


void MLbuffer_append_UInt16(UInt16 ui16) {
  ui16 = NetSwap16(ui16);
  MLbuffer_append_Data(&ui16, sizeof(UInt16));
}

void MLbuffer_append_UInt8(UInt8 ui8) {
  MLbuffer_append_Data(&ui8, sizeof(UInt8));
}


void MLbuffer_write(MLguiCode opcode) {
  if ( gMLbuffer != NULL ) {
    if (gMLbuffer->size > 0) {
      MemHandleUnlock(gMLbuffer->buffer_handle);
      MLsocketWrite(opcode, &gMLbuffer->buffer_handle, gMLbuffer->size);
      gMLbuffer->buffer_ptr = MemHandleLock(gMLbuffer->buffer_handle);
      MLbuffer_destroy();
    } else {
      MLsocketWrite(opcode, NULL, 0);
    }
  }
}

void MLbuffer_destroy(void) {
  if (gMLbuffer != NULL) {
    MemHandleUnlock(gMLbuffer->buffer_handle);
    MemHandleFree(gMLbuffer->buffer_handle);
    MemPtrFree(gMLbuffer);
    gMLbuffer = NULL;
  }
}

void MLrequest(MLguiCode opcode) {
  MLbuffer_create();
  MLbuffer_write(opcode);
  MLbuffer_destroy();
}
