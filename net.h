/* $Id: net.h,v 1.3 2003/07/17 00:21:45 tim Exp $
 *
 * Network related functions
 * Created: March 12th 2003
 */

#ifndef NET_H
#define NET_H

#include <PalmOS.h>
#include "resources.h"


#define NET_MINVERSION 0x03000000

#define NET_INVSOCK ((NetSocketRef) -1)

#define NET_BITMAP_X 76
#define NET_BITMAP_Y 1
#define NET_BITMAP_WIDTH 11
#define NET_BITMAP_HEIGHT 11

#define NET_TRAFFICUNIT_MAXLEN 3
#define NET_TRAFFIC_MAXLEN 10

// define macros to convert numbers since dumb devers did not know what network
// byte order looks like :-( That's pretty ugly since now you need to know which
// platform you are coding for and then swap bytes if needed instead of just
// using the almost always available ntohl and alike :-(
#define NetSwap32(x) \
        ((((x) >> 24) & 0x00FF) | \
         (((x) >>  8) & 0xFF00) | \
         (((x) & 0xFF00) <<  8) | \
         (((x) & 0x00FF) << 24))

#define NetSwap16(x) \
        ((((x) >> 8) & 0xFF) | \
         (((x) & 0xFF) << 8))


// Little endian high/low order
typedef struct {
  UInt32 high;
  UInt32 low;
} UInt64;


extern Err NetInit(void);
extern Err NetTerm(void);
extern void NetTrafficStart(void);
extern void NetTrafficStop(void);
extern void NetTrafficEnable(void);
extern void NetTrafficDisable(void);

extern UInt8 NetUInt64ToBytes(UInt64 ui64, UInt32 *low, UInt32 *veryLow, Boolean swap);
extern void NetTrafficUnit(Char *dst, UInt8 n);
extern void NetTrafficStringFromBytes(UInt32 bytes, Char *dst);

#endif
