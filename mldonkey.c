/* $Id: mldonkey.c,v 1.10 2003/07/24 22:00:23 tim Exp $
 *
 * Functions to deal with MLdonkey
 * Created: March 13th 2003
 */

#include "mldonkey.h"
#include "mlstats.h"
#include "mlfiles.h"
#include "tnglue.h"
#include "progress.h"

extern UInt16 gNetReference;
Int32 gMLtimeout=5;
NetSocketRef gMLsocket=NULL;
MLconfig *gMLconfig=NULL;
Boolean gMLprocessLocked=true;
MLcallbackID gMLstatsFooterCbID, gMLdingCbID, gMLdledCbID, gMLnetworkCbID, gMLcoreProtoCbID, gMLbadPassCbID;
MLcoreCode gMLopcode=0;
UInt32 gMLsize = 0;
TNlist *gMLnetworks=NULL;


void MLreadDiscard(UInt32 size) {
  MemHandle m = MemHandleNew(4096);
  UInt32 toDiscard=size;
  Err err = errNone;
  Char *buffer = MemHandleLock(m);

  while (toDiscard > 0) {
    UInt16 toRead, recd=0;

    toRead = min(4096, toDiscard);
    
    while ((toRead > 0) && (err == errNone)) {
      recd = NetLibReceive(gNetReference, gMLsocket, buffer, toRead, 0, NULL, 0, gMLtimeout*SysTicksPerSecond(), &err);
      toRead -= recd;
      toDiscard -= recd;
    }
  }

  gMLsize -= size;

  MemHandleUnlock(m);
  MemHandleFree(m);
}

void MLreadPrepare(UInt32 size, MLcoreCode opcode) {
  gMLopcode = opcode;
  gMLsize = size;
}

UInt32 MLreadLeft(MLcoreCode *opc) {
  if (opc != NULL) *opc = gMLopcode;
  return gMLsize;
}

Err MLreadHead(UInt32 *size, MLcoreCode *opcode) {
  UInt16 recd=0;
  MLmsgHead head;
  Err err=errNone;

  recd = NetLibReceive(gNetReference, gMLsocket, &head, sizeof(head), netIOFlagPeek, NULL, 0, gMLtimeout*SysTicksPerSecond(), &err);
  if (recd < sizeof(head)) {
    // We could not read enough bytes for the head. Return InvHead and return for next read try
    return MLerrInvHead;
  }

  recd = NetLibReceive(gNetReference, gMLsocket, &head, sizeof(head), 0, NULL, 0, gMLtimeout*SysTicksPerSecond(), &err);

  *size = NetSwap32(head.size)-2; // -2 for opcode
  *opcode = NetSwap16(head.opcode);

  MLreadPrepare(*size, *opcode);
  
  return errNone;
}

/* NOTE: The data MemHandle must already be resized! */
Err MLreadBytes(MemHandle *data, UInt32 size) {
  UInt16 recd=0, toRead=0;
  Char *buffer;
  Err err;

  if ((size > 0)) {
    // If there is nothing to read this would be nonsense, right?!?
    if (size > ML_MSG_MAX_SIZE) {
      return MLerrMsgTooBig;
    }
    buffer = MemHandleLock(*data);
    MemSet(buffer, size, 0);
    
    toRead = size;
    err = errNone;
    while ((toRead > 0) && (err == errNone)) {
      recd = NetLibReceive(gNetReference, gMLsocket, buffer, toRead, 0, NULL, 0, gMLtimeout*SysTicksPerSecond(), &err);
      toRead -= recd;
      buffer += recd;
    }
    MemHandleUnlock(*data);
    gMLsize -= size;
  }
  
  return errNone;
}


Err MLreadBytesIntoBuffer(Char *buffer, UInt32 size) {
  Err err;
  if (size <= ML_MSG_MAX_SIZE) {
    Char *tmp;
    MemHandle m=MemHandleNew(size);
    if ((err = MLreadBytes(&m, size)) == errNone) {
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
      err = MLreadBytes(content, *size);
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
     err = MLreadBytes(stringHandle, length);
  }

  return err;
}


Boolean MLdataWaiting(UInt32 *size, MLcoreCode *opcode) {
  NetFDSetType readFDs, writeFDs, exceptFDs;
  Err err;
  
  netFDZero(&readFDs);
  netFDZero(&writeFDs);
  netFDZero(&exceptFDs);
  
  netFDSet(gMLsocket, &readFDs);
  if (NetLibSelect(gNetReference, gMLsocket+1, &readFDs, &writeFDs, &exceptFDs, 0, &err) > 0) {

    if (netFDIsSet(gMLsocket, &readFDs)) {
      // Now check if there are really bytes to read.
      // If the bit is set that only quarantees that the following call will succeed
      // quickly, either fail quicky or deliver data quickly
      MLmsgHead head;
      UInt16 recd = NetLibReceive(gNetReference, gMLsocket, &head, sizeof(head), netIOFlagPeek, NULL, 0, gMLtimeout*SysTicksPerSecond(), &err);
      if (recd == sizeof(head)) {
        if (size != NULL)   *size   = NetSwap32(head.size)-2; // -2 for opcode
        if (opcode != NULL) *opcode = NetSwap16(head.opcode);
        return true;
      }
    }
  }

  return false;
}


Err MLsocketWrite(MLguiCode opcode, MemHandle *content) {
  Char *buffer;
  UInt32 toSent;
  UInt16 sent;
  Err err;
  MLmsg *msg;
  MemHandle message;

  NetTrafficStart();

  if (content != NULL) {
    toSent = MemHandleSize(*content);
  } else {
    toSent = 0;
  }
  message = MemHandleNew(toSent + sizeof(MLmsgHead));

  msg = (MLmsg *)MemHandleLock(message);
  msg->head.size = NetSwap32(toSent+2); // +2 for opcode
  msg->head.opcode = NetSwap16(opcode);

  buffer = (Char *)msg;
  buffer += sizeof(MLmsgHead);

  if (content != NULL) {
    MemMove(buffer, MemHandleLock(*content), toSent);
    MemHandleUnlock(*content);
  }
  toSent += sizeof(MLmsgHead);

  buffer = (Char *)msg;
  err = errNone;

  if (MLsocketIsOpen(gMLsocket, &err)) {
    while ((toSent > 0) && (err == errNone)) {
      sent = NetLibSend(gNetReference, gMLsocket, buffer, toSent, 0, NULL, 0, gMLtimeout*SysTicksPerSecond(), &err);
      toSent -= sent;
      buffer += sent;
    }
  } else {
    FrmCustomAlert(ALERT_debug, "Write attempt on closed socket", "", "");
    MLdisconnect();
    FrmGotoForm(FORM_main);
  }

  MemHandleUnlock(message);
  MemHandleFree(message);

  NetTrafficStop();

  return err;
}


MLnetInfo* MLnetworkGetByID(UInt32 id) {
  TNlist *tmpList = gMLnetworks;

  while (tmpList) {
    MLnetInfo *net=(MLnetInfo *)tmpList->data;
    if (net->id == id) {
      break;
    }
    tmpList = tmpList->next;
  }

  if (tmpList) {
    return (MLnetInfo *)tmpList->data;
  } else {
    return NULL;
  }
}



Boolean MLsocketIsOpen(NetSocketRef socket, Err *err) {
  Char junk;
  UInt16 recd;
  recd = NetLibReceive(gNetReference, socket, &junk, 1, netIOFlagPeek, NULL, 0, SysTicksPerSecond(), err);

  return ((recd > 0) && (*err != netErrSocketNotOpen));
}



Err MLsocket(MLconfig *config, NetSocketRef *socket) {
  Err err=errNone;
  NetHostInfoBufType *hostInfoBuf = NULL;
  NetHostInfoType *hostInfo = NULL;
  NetIPAddr addr = 0, *addrP=NULL;

  if (StrLen(gMLconfig->hostname) == 0) return -1;
  if (gMLconfig->port > UInt16_MAX) return -1;

  if (StrLen(gMLconfig->login) == 0) StrCopy(gMLconfig->login, "admin");
  if (gMLconfig->port == 0) gMLconfig->port = 4001;

  hostInfoBuf = (NetHostInfoBufType *)MemPtrNew(sizeof(NetHostInfoBufType));

  hostInfo = NetLibGetHostByName(gNetReference, gMLconfig->hostname, hostInfoBuf,
                                 5 * SysTicksPerSecond(), &err);


  if (! hostInfo || (err != errNone)) {
    ErrAlert(err);
    MemPtrFree(hostInfoBuf);
    return err;
  } else {
    NetSocketRef socketLoc;
    NetSocketTypeEnum socketType;
    NetSocketAddrType socketAddr;
    NetSocketAddrINType *socketINaddr;
    UInt16 resCode=0;
  
    addrP = (NetIPAddr *)hostInfo->addrListP[0];
    addr = NetNToHL(*addrP);
  
    MemPtrFree(hostInfoBuf);
  
    // Connect and get peers proto ver
    socketType = netSocketTypeStream;
    socketINaddr = (NetSocketAddrINType *)&socketAddr;
    socketINaddr->family = netSocketAddrINET;
    socketINaddr->port = NetHToNS(config->port);
    socketINaddr->addr = NetHToNL(addr);

    socketLoc = NetLibSocketOpen(gNetReference, netSocketAddrINET,
                                 socketType, 0, gMLtimeout*SysTicksPerSecond(), &err);

    if (socketLoc == NET_INVSOCK) {
      ErrAlert(err);
    } else {
      resCode = NetLibSocketConnect(gNetReference, socketLoc, &socketAddr,
                                    sizeof(socketAddr), gMLtimeout*SysTicksPerSecond(), &err);
      if (resCode != 0) {
        FrmCustomAlert(ALERT_debug, "Could not open connection", "", "");
        NetLibSocketClose(gNetReference, socketLoc, gMLtimeout*SysTicksPerSecond(), &err);
        return netErrSocketNotOpen;
      } else {
        *socket = socketLoc;
        err = errNone;
      }
    }
  }

  return err;
}

Err MLdisconnect(void) {
  Err err;
  TNlist *tmpList;

  if (gMLconfig->connected && gNetReference)
    NetLibSocketClose(gNetReference, gMLsocket, gMLtimeout, &err);

  gMLconfig->connected = false;

  MLcallbackUnregister(gMLstatsFooterCbID);
  MLcallbackUnregister(gMLdingCbID);
  MLcallbackUnregister(gMLdledCbID);
  MLcallbackUnregister(gMLnetworkCbID);
  MLcallbackUnregister(gMLcoreProtoCbID);
  MLcallbackUnregister(gMLbadPassCbID);

  gMLprocessLocked=true;

  tmpList = gMLnetworks;
  while (tmpList) {
    MLnetInfo *net = (MLnetInfo *)tmpList->data;
    MemHandleFree(net->name);
    tmpList = tmpList->next;
  }
  TNlistFree(gMLnetworks);
  gMLnetworks = NULL;

  NetTerm();

  return err;
}

static void MLnetworkCb(MLcoreCode opc, UInt32 dataSize) {
  Err err;
  MLcoreCode opcode=0;
  UInt32 size=0;
  MLnetInfo *net = MemPtrNew(sizeof(MLnetInfo));
  UInt16 tmp16;
  
  NetTrafficStart();
  if ((err = MLreadHead(&size, &opcode)) != errNone) {
    NetTrafficStop();
    return;
  }

  MLread_UInt32(&net->id);
  net->name = MemHandleNew(20);
  MLread_String(&net->name);
  MLread_Bool(&net->enabled);

  MLread_UInt16(&tmp16);
  MLreadDiscard(tmp16);

  MLread_UInt64(&net->uploaded);
  MLread_UInt64(&net->downloaded);

  gMLnetworks = TNlistAppend(gMLnetworks, net);

  NetTrafficStop();
}

static void MLcoreProtoCb(MLcoreCode opc, UInt32 dataSize) {
  Err err;
  MLcoreCode opcode=0;
  UInt32 size=0, ui32=0;

  gMLconfig->connected = true;

  NetTrafficStart();
  if ((err = MLreadHead(&size, &opcode)) != errNone) {
    NetTrafficStop();
    return;
  }

  MLread_UInt32(&ui32);
  gMLconfig->CoreProto=ui32;
  ProgressUpdate(errNone, 2, NULL, false);

  // Our supported GUI Extensions
  MLbuffer_create();
  MLbuffer_append_UInt16(1);
  MLbuffer_append_UInt32(1);
  MLbuffer_append_UInt8(1);
  MLbuffer_write(GuiExtensions);
  
  // Send user and password
  MLbuffer_create();
  MLbuffer_append_String(gMLconfig->password);
  MLbuffer_append_String(gMLconfig->login);
  MLbuffer_write(AuthUserPass);

  NetTrafficStop();
}

static void MLgoodPassCb(MLcoreCode opc, UInt32 dataSize) {
  ProgressUpdate(errNone, 3, NULL, true);
  MLcallbackUnregister(gMLstatsFooterCbID);
  MLcallbackRegister(Client_stats_v4, &gMLstatsFooterCbID, MLstatsFooterCb);
  ProgressStop();
  NetTrafficEnable();
  NetTrafficStop();
  FrmGotoForm(FORM_stats);
}

static void MLbadPassCb(MLcoreCode opc, UInt32 dataSize) {
  ProgressUpdate(-1, 4, NULL, true);
  gMLprocessLocked = true;
  MLdisconnect();
}

Err MLconnect(MLconfig *config) {
  Err err=errNone;

  gMLprocessLocked=true;

  NetInit();
  NetTrafficStart();
  NetTrafficDisable();

  if (config != NULL) gMLconfig = config;

  ProgressStart(PROGRESS_conn_title, sysNetworkProgress01Bitmap, 6, NULL);

  if ( ((err = MLsocket(gMLconfig, &gMLsocket)) == errNone) &&
       MLsocketIsOpen(gMLsocket, &err) ) {

    MLbuffer_create();
    MLbuffer_append_UInt32(MLDONKEY_PROTO_VER);
    MLbuffer_write(GuiProto);

    MLcallbackRegister(Client_stats_v4, &gMLstatsFooterCbID, MLgoodPassCb);
    MLcallbackRegister(DownloadFiles_v4, &gMLdingCbID, MLfilesCb);
    MLcallbackRegister(DownloadedFiles_v4, &gMLdledCbID, MLfilesCb);
    MLcallbackRegister(Network_info, &gMLnetworkCbID, MLnetworkCb);
    MLcallbackRegister(CoreProtocol, &gMLcoreProtoCbID, MLcoreProtoCb);
    MLcallbackRegister(BadPassword, &gMLbadPassCbID, MLbadPassCb);

  } else {
    ProgressStop();
    FrmAlert(ALERT_cantconnect);
  }

  gMLprocessLocked=false;

return err;
}



Err MLprocess(void) {
  Err err=errNone;

  // VERY primitive "locking".
  // Yeah, we learned better stuff, whatever...
  if (gMLprocessLocked) return MLerrProcessLocked;
  gMLprocessLocked=true;

  if ( (gMLconfig != NULL) ) {
    MLcoreCode opcode;
    UInt32 bytes;

    while (MLdataWaiting(&bytes, &opcode)) {
      MLcallbackFindAndRun(opcode, bytes);
    }
  }

  gMLprocessLocked=false;
  return err;
}
