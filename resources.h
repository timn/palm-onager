/* $Id: resources.h,v 1.7 2003/07/23 22:20:42 tim Exp $
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
#define FILES_size 1037
#define FILES_size_label 1038
#define FILES_dled 1039
#define FILES_dled_label 1040
#define FILES_rate 1041
#define FILES_rate_label 1042
#define FILES_done 1043
#define FILES_done_label 1044
#define FILES_net 1045
#define FILES_net_label 1046
#define FILES_stats 1047
#define FILES_string_ding 1048
#define FILES_string_dled 1049
#define FILES_nofiles 1050


#define MENU_main 2000
#define MENUITEM_about 2001


#define ALERT_ROMIncompatible 4001
#define ALERT_debug 4002
#define ALERT_noNet 4003
#define ALERT_iff 4004
#define ALERT_pass 4005
#define ALERT_cantconnect 4006


#define PROGRESS_conn_title 4500
#define PROGRESS_conn_est 4501
#define PROGRESS_conn_auth 4502
#define PROGRESS_conn_succ 4503
#define PROGRESS_conn_bad 4504

#define PROGRESS_files_title 4510
#define PROGRESS_files_req 4511
#define PROGRESS_files_work 4512
#define PROGRESS_files_done 4513
#define PROGRESS_files_fallback 4514


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
#define BITMAP_disconnect 5013
#define BITMAP_disconnect_clicked 5014
#define BITMAP_stats 5015
#define BITMAP_stats_clicked 5016
