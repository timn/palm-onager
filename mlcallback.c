/* $Id: mlcallback.c,v 1.1 2003/07/09 12:05:16 tim Exp $
 *
 * Functions for mldonkey callback stuff
 * Created: April 7th 2003
 */

#include "mlcallback.h"
#include "tnlist.h"

MLcallbackID gMLcbLastID=0;
TNlist *gMLcbList=NULL;


Err MLcallbackRegister(MLcoreCode coreCode, MLcallbackID *callbackID, MLcallbackFunc *func) {
  MLcoreCallback *cb;
  cb = MemPtrNew(sizeof(MLcoreCallback));

  cb->id = gMLcbLastID;
  *callbackID = cb->id;
  gMLcbLastID += 1;

  cb->code = coreCode;
  cb->func = func;

  gMLcbList = TNlistAppend(gMLcbList, cb);

  return errNone;
}


Int16 MLcallbackCompareID(void *a, void *b) {
  MLcoreCallback *cba = (MLcoreCallback *)a;
  MLcoreCallback *cbb = (MLcoreCallback *)b;

  if (cba->id < cbb->id) {
    return -1;
  } else if (cba->id > cbb->id) {
    return 1;
  } else {
    return 0;
  }
}


Int16 MLcallbackCompareCC(void *a, void *b) {
  MLcoreCallback *cba = (MLcoreCallback *)a;
  MLcoreCallback *cbb = (MLcoreCallback *)b;
  
  if (cba->code < cbb->code) {
    return -1;
  } else if (cba->code > cbb->code) {
    return 1;
  } else {
    return 0;
  }
}


Err MLcallbackUnregister(MLcallbackID callbackID) {
  MLcoreCallback cb;
  TNlist *node;

  cb.id = callbackID;
  node = TNlistSearch(gMLcbList, &cb, MLcallbackCompareID);

  if (node) {
    gMLcbList = TNlistRemove(gMLcbList, node->data);
  } else {
    return MLerrInvCbID;
  }

  return errNone;
}


void MLcallbackFindAndRun(MLcoreCode opcode, UInt32 bytes) {
  TNlist *cblist;
  MLcoreCallback cbS;

  cbS.code = opcode;

  cblist = TNlistSearch(gMLcbList, &cbS, MLcallbackCompareCC);
  if (cblist) {
    MLcoreCallback *cb = (MLcoreCallback *)cblist->data;
    cb->func(opcode, bytes);
  } else {
    MLreadDiscard(bytes+sizeof(MLmsgHead));
  }
}
