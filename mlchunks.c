/* $Id: mlchunks.c,v 1.7 2003/07/29 20:29:14 tim Exp $
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
  
  if (! chunkInfo->chunks || ! chunkInfo->availability || (StrLen(chunkInfo->chunks) == 0)) {
    Char *nochunks = TNGetLockedString(CHUNKS_no);
    FontID oldFont;
    RectangleType stringRect;
    UInt16 offsetx, offsety;

    oldFont = FntSetFont(boldFont);

    offsetx = (bounds.extent.x - FntLineWidth(nochunks, StrLen(nochunks))) / 2;
    offsety = (bounds.extent.y - FntLineHeight()) / 2;
    
    RctSetRectangle(&stringRect, bounds.topLeft.x+offsetx, bounds.topLeft.y+offsety,
                                 bounds.extent.x, bounds.extent.y);
    TNDrawCharsToFitWidth(nochunks, &stringRect);

    FntSetFont(oldFont);
    MemPtrUnlock(nochunks);
    return;
  }

  len = StrLen(chunkInfo->chunks);
  FrmGetObjectBounds(frm, TNGetObjectIndexFromPtr(frm, gadget), &bounds);

  RctSetRectangle(&bounds, bounds.topLeft.x+1, bounds.topLeft.y+1,
                           bounds.extent.x-2, bounds.extent.y-2);
  WinSetForeColor(UIColorGetTableEntryIndex(UIObjectForeground));
  WinDrawRectangleFrame(simpleFrame, &bounds);

  /* NEW code
   * Assumptions: NumberOfChunks <= GadgetWidth
   * If this is not the case nothing gets drawn. First I had it that much longer
   * chunk strings could be drawn but that just seems waste of code and I needed
   * to rewrite it anyway to take care about the availability string, totally
   * forgot about that the first time...
   */

  if (len > bounds.extent.x) {
    Char *toomanychunks = TNGetLockedString(CHUNKS_toomany);
    FontID oldFont;
    RectangleType stringRect;
    UInt16 offsetx, offsety;

    oldFont = FntSetFont(boldFont);

    offsetx = (bounds.extent.x - FntLineWidth(toomanychunks, StrLen(toomanychunks))) / 2;
    offsety = (bounds.extent.y - FntLineHeight()) / 2;
    
    RctSetRectangle(&stringRect, bounds.topLeft.x+offsetx, bounds.topLeft.y+offsety,
                                 bounds.extent.x, bounds.extent.y);
    TNDrawCharsToFitWidth(toomanychunks, &stringRect);

    FntSetFont(oldFont);
    MemPtrUnlock(toomanychunks);
    return;
  }

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
    RctSetRectangle(&rect, bounds.topLeft.x + (chunkWidth * i) + min(i, chunksWithExtra),
                           bounds.topLeft.y,
                           chunkWidth + ((i < chunksWithExtra) ? 1 : 0),
                           bounds.extent.y);
    WinDrawRectangle(&rect, 0);
  }

  // Restore old color
  TNSetForeColorRGB(&old, NULL);
  
}
