/* $Id: resources.h,v 1.1 2003/07/09 12:05:16 tim Exp $
 *
 * ID's for use in ressource definitions
 *
 * Definitions:
 * 1:         Version
 * 100-199:   app specific values
 * 1001-1999: Forms, form items
 * 2000-2999: Menus and menu items
 * 4000-4999: Help
 * 8000-8999: Special stuff, bitmaps etc.
 */

#define APP_ID 100
#define APP_NAMEID 101
#define APP_CATEGORIES 102


#define FORM_main 1001
#define FIELD_host 1002
#define FIELD_port 1003
#define FIELD_user 1004
#define FIELD_pass 1005
#define BUTTON_connect 1006
#define BUTTON_net 1007

#define FORM_stats 1010
#define STATS_disco 1011
#define STATS_LABEL_down 1012
#define STATS_LABEL_up 1013
#define STATS_ding 1014
#define STATS_ded 1015

#define FORM_about 1020
#define BITMAP_logo 1021
#define BUTTON_about_ok 1022

#define FILES_form 1030
#define FILES_disco 1031
#define FILES_list 1032
#define FILES_trigger 1033
#define FILES_ding 1034
#define FILES_ded 1035
#define FILES_GADGET_chunks 1036


#define MENU_main 2000
#define MENUITEM_about 2001


#define ALERT_ROMIncompatible 4001
#define ALERT_debug 4002
#define ALERT_noNet 4003
#define ALERT_iff 4004
#define ALERT_pass 4005


#define BITMAP_net 5001
#define BITMAP_ok 5002
#define BITMAP_ok_clicked 5003
#define BITMAP_cancel 5004
#define BITMAP_cancel_clicked 5005
#define BITMAP_downloading 5006
#define BITMAP_downloading_clicked 5007
#define BITMAP_downloaded 5008
#define BITMAP_downloaded_clicked 5009
#define BITMAP_progress_start 5010
#define BITMAP_progress_2 5011
#define BITMAP_progress_3 5012
