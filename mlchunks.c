/* $Id: mlchunks.c,v 1.1 2003/07/09 12:05:16 tim Exp $
 *
 * ML chunk gadget code
 * Created: April 10th 2003
 */

#include "mlchunks.h"
#include "tnglue.h"

Boolean MLchunkGadgetHandler(FormGadgetTypeInCallback *gadget, UInt16 cmd, void *param) {
  Boolean handled = false;
  EventType *event = (EventType *)param;

  switch (cmd) {
    case formGadgetDrawCmd:
      //Sent to active gadgets any time form is
      //drawn or redrawn.
      MLchunkGadgetDraw(gadget);
      gadget->attr.visible = true;
      handled = true;
      break;

    case formGadgetHandleEventCmd:
      //Sent when form receives a gadget event.
      //paramP points to EventType structure.

      if (event->eType == frmGadgetEnterEvent) {
        // penDown in gadget's bounds.
        // GadgetTap((FormGadgetType *)gadgetP, event);
        //handled = true;
      }
      if (event->eType == frmGadgetMiscEvent) {
        //This event is sent by your application
        //when it needs to send info to the gadget
      }
      break;

    case formGadgetDeleteCmd:
      //Perform any cleanup prior to deletion.
      break;

    case formGadgetEraseCmd:
      //FrmHideObject takes care of this if you
      //return false.
       handled = false;
       break;

    default:
      break;

  }

  return handled;
}

void MLchunkGadgetDraw(FormGadgetTypeInCallback *gadget) {
  RectangleType bounds, rect;
  FormType *frm=FrmGetActiveForm();
  RGBColorType red={0x00, 0xFF, 0x00, 0x00},
               green={0x00, 0x00, 0xFF, 0x00},
               blue={0x00, 0x00, 0x00, 0xFF}, old;
  MLchunkInfo *chunkInfo=(MLchunkInfo *)gadget->data;
  UInt16 len, i;
  UInt16 chunksWithExtra = 0, chunkWidth=0;
  
  if (! chunkInfo->chunks || ! chunkInfo->availability)  return;

  len = StrLen(chunkInfo->chunks);
  FrmGetObjectBounds(frm, TNGetObjectIndexFromPtr(frm, gadget), &bounds);

  /* NEW code
   * Assumptions: NumberOfChunks <= GadgetWidth
   * If this is not the case nothing gets drawn. First I had it that much longer
   * chunk strings could be drawn but that just seems waste of code and I needed
   * to rewrite it anyway to take care about the availability string, totally
   * forgot about that the first time...
   */

  if (len > bounds.extent.x)  return;

  // Save old color
  TNSetForeColorRGB(&red, &old);

  // Calculate width of chunks
  chunkWidth = (UInt16)(bounds.extent.x / len);
  chunksWithExtra = bounds.extent.x - (len * chunkWidth);

  
  for (i=0; i < len; ++i) {

    // Determine color of chunk
    if (chunkInfo->chunks[i] == '2') {
      TNSetForeColorRGB(&green, NULL);
    } else if (chunkInfo->availability[i] >= 1) {
      TNSetForeColorRGB(&blue, NULL);
    } else {
      TNSetForeColorRGB(&red, NULL);
    }

    // Draw the chunk
    RctSetRectangle(&rect, bounds.topLeft.x + (chunkWidth * i) + ((i < chunksWithExtra) ? 1 : 0) + min(i, chunksWithExtra),
                           bounds.topLeft.y,
                           chunkWidth + ((i < chunksWithExtra) ? 1 : 0),
                           bounds.extent.y);
    WinDrawRectangle(&rect, 0);
  }
  
  
/* OLD Code
  
    Char temp[100];

  
  while (len > bounds.extent.x) {
    len /= 2;
    chunksPerUnit *= 2;
  }


  c.r=255;
  c.g=0;
  c.b=0;

  TNSetForeColorRGB(&c, &old);
  WinDrawRectangle(&bounds, 0);
  c.r = 0;
  TNSetForeColorRGB(&c, NULL);
  WinDrawRectangleFrame(simpleFrame, &bounds);
  TNSetForeColorRGB(&old, NULL);

  c.r=0;
  c.g=0;
  c.b=255;

  TNSetForeColorRGB(&c, &old);

  chunkWidth = (UInt16)(bounds.extent.x / len);
  chunksWithExtra = bounds.extent.x - (len * chunkWidth);

  StrPrintF(temp, "Len: %u, Width: %u, withExtra: %u, ex.x: %u", len, chunkWidth, chunksWithExtra, bounds.extent.x);
  //FrmCustomAlert(ALERT_debug, temp, "", "");
  
  for (i=0; i < len; ++i) {
    UInt8 numChunksDone=0;

    for (j=0; j < chunksPerUnit; ++j) {
      if (chunkInfo->chunks[(chunksPerUnit*i)+j] >= '1') {
        numChunksDone += 1;
      }
    }

    if (numChunksDone == chunksPerUnit) {
      // Draw a "fully downloaded" chunk
      RctSetRectangle(&rect, bounds.topLeft.x + (chunkWidth * i) + ((i < chunksWithExtra) ? 1 : 0) + min(i, chunksWithExtra),
                             bounds.topLeft.y,
                             chunkWidth + ((i < chunksWithExtra) ? 1 : 0),
                             bounds.extent.y);
      WinDrawRectangle(&rect, 0);
      //StrPrintF(temp, "i: %u, X: %u, Y: %u, W: %u, H: %u", i, rect.topLeft.x, rect.topLeft.y, rect.extent.x, rect.extent.y);
      //FrmCustomAlert(ALERT_debug, temp, "", "");
    } else if ((chunksPerUnit == 2) && (i < chunksWithExtra)) {
      // Special case.
      // We had a size between extent.x and 2*extent.x. Now the chunks with extra line
      // do not need horizontal splitting, they can be split vertically
      //FrmCustomAlert(ALERT_debug, "Case 2", "", "");
      RctSetRectangle(&rect, bounds.topLeft.x + (chunkWidth * i) + ((chunkInfo->chunks[(chunksPerUnit*i)] >= '1') ? 0 : 1) + min(i, chunksWithExtra),
                             bounds.topLeft.y,
                             1,
                             bounds.extent.y);
      WinDrawRectangle(&rect, 0);
    } else if (numChunksDone > 0) {
      // We have more chunks than two chunks per line and
      // FrmCustomAlert(ALERT_debug, "Case 3", "", "");
      if (((bounds.extent.y % chunksPerUnit) == 0) && (chunksPerUnit <= bounds.extent.y)) {
        // we can easily divide
        UInt16 vertChunkHeight, v;

        vertChunkHeight = (UInt16)(bounds.extent.y / chunksPerUnit);

        for (v=0; v < chunksPerUnit; ++v) {
          if (chunkString[(chunksPerUnit*i)+v] == '1') {
            RctSetRectangle(&rect, bounds.topLeft.x + (chunkWidth * i) + ((i < chunksWithExtra) ? 1 : 0) + min(i, chunksWithExtra),
                                   bounds.topLeft.y + (vertChunkHeight * v),
                                   chunkWidth + ((i < chunksWithExtra) ? 1 : 0),
                                   vertChunkHeight);
            WinDrawRectangle(&rect, 0);
          }
        }
      } else {
        RctSetRectangle(&rect, bounds.topLeft.x + (chunkWidth * i) + ((i < chunksWithExtra) ? 1 : 0) + min(i, chunksWithExtra),
                               bounds.topLeft.y,
                               chunkWidth + ((i < chunksWithExtra) ? 1 : 0),
                               (UInt16)(bounds.extent.y / 2));
        WinDrawRectangle(&rect, 0);
      }
    }
  }
*/

  // Restore old color
  TNSetForeColorRGB(&old, NULL);
  
}
