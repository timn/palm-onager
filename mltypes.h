/* $Id: mltypes.h,v 1.3 2003/07/22 18:08:30 tim Exp $
 *
 * Types for mldonkey conversation
 * Created: March 22th 2003
 */

#include "net.h"
#include "tnlist.h"

#define MLerrInvHead        0x8000
#define MLerrMsgTooBig      0x8001
#define MLerrNoCBleft       0x8002
#define MLerrInvCbID        0x8003
#define MLerrProcessLocked  0x8004
#define MLerrNotConnected   0x8005


typedef enum {
  CoreProtocol          =  0, /* 0 */
  Options_info          =  1,
  RESERVED2             =  2,
  DefineSearches        =  3,
  Result_info           =  4,
  Search_result         =  5,
  Search_waiting        =  6,
  File_info             =  7,
  File_downloaded       =  8,
  File_availability     =  9,
  File_source           = 10, /* 10 */
  Server_busy           = 11,
  Server_user           = 12,
  Server_state          = 13,
  Server_info           = 14,
  Client_info           = 15,
  Client_state          = 16,
  Client_friend         = 17,
  Client_file           = 18,
  Console               = 19,
  Network_info          = 20, /* 20 */
  User_info             = 21,
  Room_info             = 22,
  Room_message          = 23,
  Room_add_user         = 24,
  Client_stats          = 25,
  Server_info_v2        = 26,
  MessageFromClient     = 27,
  ConnectedServers      = 28,
  DownloadFiles         = 29,
  DownloadedFiles       = 30, /* 30 */
  Room_info_v2          = 31,
  Room_remove_user      = 32,
  Shared_file_info      = 33,
  Shared_file_upload    = 34,
  Shared_file_unshared  = 35,
  Add_section_option    = 36,
  Client_stats_v2       = 37,
  Add_plugin_option     = 38,
  Client_stats_v3       = 39,
  File_info_v2          = 40, /* 40 */
  DownloadFiles_v2      = 41,
  DownloadedFiles_v2    = 42,
  File_info_v3          = 43,
  DownloadFiles_v3      = 44,
  DownloadedFiles_v3    = 45,
  File_downloaded_v2    = 46,
  BadPassword           = 47,
  Shared_file_info_v2   = 48,
  Client_stats_v4       = 49,
  File_remove_source    = 50,
  CleanTables           = 51,
  File_info_v4          = 52,
  DownloadFiles_v4      = 53,
  DownloadedFiles_v4    = 54,
} MLcoreCode;

typedef enum {
  GuiProto              =  0,
  ConnectMore           =  1,
  CleanOldServers       =  2,
  KillServer            =  3,
  ExtendedSearch        =  4,
  Password              =  5,
  Search                =  6,
  Download              =  7,
  Url                   =  8,
  RemoveServer          =  9,
  SaveOptions           = 10,
  RemoveDownload        = 11,
  ServerUsers           = 12,
  SaveFile              = 13,
  AddClientFriend       = 14,
  AddUserFriend         = 15,
  RemoveFriend          = 16,
  RemoveAllFriends      = 17,
  FindFriend            = 18,
  ViewUsers             = 19,
  ConnectAll            = 20,
  ConnectServer         = 21,
  DisconnectServer      = 22,
  SwitchDownload        = 23,
  VerifyAllChunks       = 24,
  QueryFormat           = 25,
  ModifyMp3Tags         = 26,
  ForgetSearch          = 27,
  SetOption             = 28,
  Command               = 29,
  Preview               = 30,
  ConnectFriend         = 31,
  GetServer_users       = 32,
  GetClient_files       = 33,
  GetFile_locations     = 34,
  GetServer_info        = 35,
  GetClient_info        = 36,
  GetFile_info          = 37,
  GetUser_info          = 38,
  PrivMessageToClient   = 39,
  EnableNetwork         = 40,
  BrowseUser            = 41,
  Search_query_v2       = 42,
  MessageToClient       = 43,
  GetConnectedServers   = 44,
  GetDownloadFiles      = 45,
  GetDownloadedFiles    = 46,
  GuiExtensions         = 47,
  SetRoomState          = 48,
  RefreshUploadStats    = 49,
  Download_query        = 50,
  SetFilePriority       = 51,
  AuthUserPass          = 52,
} MLguiCode;

typedef enum {
  Downloading           = 0,
  Paused                = 1,
  Downloaded            = 2,
  Shared                = 3,
  Cancelled             = 4,
  New                   = 5,
  Aborted               = 6,
  Queued                = 7
} MLfileState;


// Used by MLreadSizeOp. Note: When you read the directly from the stream you
// need to swap those values on a Big Endian machine like Palm!
typedef struct {
  UInt32 size;
  UInt16 opcode;
} MLmsgHead;

typedef struct {
  MLmsgHead  head;
  Char      *content;
} MLmsg;

typedef UInt16 MLlistHead;
typedef struct {
  UInt32 name;
  UInt8  value;
} MLlistIntBool;

// The info necessary to connect to mldonkey. login and password can be NULL.
// Make SURE that the first fields are completely identical to the stuff from
// MLprefs type!
#define ML_CONFIG_FIELD_MAXLEN 31
typedef struct {
  Char hostname[ML_CONFIG_FIELD_MAXLEN+1];
  Char login[ML_CONFIG_FIELD_MAXLEN+1];
  Char password[ML_CONFIG_FIELD_MAXLEN+1];
  UInt16 port;
  UInt32    CoreProto;
  UInt8     connected     :1    ;
  UInt8     loggedIn      :1    ;
  UInt8     flags         :6    ;
  
} MLconfig;

// The MLDonkey status returned
typedef struct {
  UInt64 upload_counter;
  UInt64 download_counter;
  UInt64 shared_counter;
  UInt32 nshared_files;
  UInt32 tcp_upload_rate;
  UInt32 tcp_download_rate;
  UInt32 udp_upload_rate;
  UInt32 udp_download_rate;
  UInt32 ndownloading_files;
  UInt32 ndownloaded_files;
  UInt32 nconnected_networks;
  UInt16 connected_networks;
} MLinfo;

typedef struct {
  UInt32      file_num;
  UInt32      net_num;
  TNlist     *names;
  Char        md4[16];
  UInt8       state;
  UInt32      size;
  UInt32      downloaded;
  UInt32      num_locations;
  UInt32      num_clients;
  MemHandle   chunks;
  MemHandle   availability;
  MemHandle   download_rate;
  TNlist     *chunks_age;
  MemHandle   file_age;
  UInt32      last_seen;
  MemHandle   name;
  UInt32      priority;
} MLfileInfo;

typedef struct {
  Char *chunks;
  Char *availability;
} MLchunkInfo;

typedef struct {
  UInt32      id;
  Boolean     enabled;
  MemHandle   name;
  UInt64      uploaded;
  UInt64      downloaded;
} MLnetInfo;




typedef UInt8 MLcallbackID;
typedef void MLcallbackFunc (MLcoreCode coreCode, UInt32 dataSize);

typedef struct {
  MLcallbackID      id;
  MLcoreCode        code;
  MLcallbackFunc   *func;
} MLcoreCallback;

typedef struct {
  UInt16 current;
  UInt16 max;
} ProgressStatus;
