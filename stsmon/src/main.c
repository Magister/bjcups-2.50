/*  Canon Bubble Jet Print Filter.
 *  Copyright CANON INC. 2001-2005
 *  All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <config.h>

#include <gtk/gtk.h>

#include "interface.h"
#include "support.h"
#include "callbacks.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>

#include "lmsmsock.h"
#include "smstat.h"

#define  LM_COM_PORT	19999

extern	void print_sts_recv();
extern	int 	SM_ComCreate();
extern	void	sighand_term();
extern	void	sighand_usr1();
extern	gint stsmon_show_check();

char 	*check_model(int ac,char **argv);
int  	check_noparent(int ac,char **argv);
int  	check_model_s(char *);
void	SetGtkResourceDefault();
void	draw_init(GtkWidget *);

#ifdef DEBUG
void create_log();
void write_log(char *str);
#endif

/*-------------------------------------------
 * 	GLOBAL 
 *-------------------------------------------*/
int		sock_tag;	// socket tag
int		sock_path;	// socket path
lmsm_socket	lmsm_s;
int		enable_cancel;	// enable cancel   button	

GdkPixmap	*pixmap;	// drawing area
GdkPixmap	*g_out;		// Ink_Out Fill Pixmap
GdkPixmap	*g_low;		// Ink_low Fill Pixmap
GdkBitmap	*mask_low;	// Ink Out Pixmap Mask
GdkBitmap	*mask_out;	// Ink Out Pixmap Mask
GdkPixmap	*g_out_bk;	// Ink_Out Fill Pixmap
GdkPixmap	*g_low_bk;	// Ink_low Fill Pixmap
GdkBitmap	*mask_low_bk;	// Ink Out Pixmap Mask
GdkBitmap	*mask_out_bk;	// Ink Out Pixmap Mask
GdkPixmap	*g_I_el;	// cartridge edge left Pixmap
GdkPixmap	*g_I_er;	// cartridge edge right Pixmap
GdkPixmap	*g_I_pb;	// ink photo black Pixmap
GdkPixmap	*g_I_pc;	// ink photo cyan Pixmap
GdkPixmap	*g_I_pm;	// ink photo magenta Pixmap
GdkPixmap	*g_I_bk;	// ink black Pixmap
GdkPixmap	*g_I_bb;	// ink big black Pixmap
GdkPixmap	*g_I_cy;	// ink cyan Pixmap
GdkPixmap	*g_I_ma;	// ink magenta Pixmap
GdkPixmap	*g_I_ye;	// ink yellow Pixmap
GdkPixmap	*g_I_re;	// ink red Pixmap		990i
GdkPixmap	*g_I_gr;	// ink green Pixmap		ip8600
GdkPixmap	*g_I_sc;	// scanner Pixmap
GdkPixmap	*g_I_sp;	// ink space for big-black Pixmap
GdkPixmap	*g_I_24b;	// S300 
GdkPixmap	*g_I_24c;	// S300 
GdkPixmap	*g_I_24bf;	// S300 
GdkPixmap	*g_I_24cf;	// S300 
GdkPixmap	*g_I_24b1;	// S300 
GdkPixmap	*g_I_24b2;	// S300 
GdkPixmap	*g_I_24b3;	// S300 
GdkPixmap	*g_I_24c1;	// S300 
GdkPixmap	*g_I_24c2;	// S300 
GdkPixmap	*g_I_24c3;	// S300 
GdkPixmap	*g_low070;	// F900 
GdkPixmap	*g_low040;	// F900 
GdkPixmap	*g_low010;	// F900 
GdkBitmap	*mask_nal;	// Mask nallow
GdkBitmap	*mask_wid;	// Mask wide (big black)
GdkBitmap	*mask_el;	// Mask for edge
GdkBitmap	*mask_er;	// Mask for edge
GdkBitmap	*mask_sp;	// Mask for space
GdkBitmap	*mask_sc;	// Mask for scanner
GdkBitmap	*mask_24b;	// S300 Mask
GdkBitmap	*mask_24c;	// S300 Mask
GdkBitmap	*mask_24bf;	// S300 Mask
GdkBitmap	*mask_24cf;	// S300 Mask
GdkBitmap	*mask_24b3;	// S300 Mask
GdkBitmap	*mask_24c3;	// S300 Mask
GdkBitmap	*mask_24b1;	// S300 Mask
GdkBitmap	*mask_24c1;	// S300 Mask
GdkBitmap	*mask_low010;	// F900 Mask
GdkBitmap	*mask_low040;	// F900 Mask
GdkBitmap	*mask_low070;	// F900 Mask

GdkPixmap	**ink_p[INK_CNT];
GdkBitmap	**mask_p[INK_CNT];

gchar		*sm_pixmaps_dir;
		
int			gDw,gDh;				// Pixmap Default dx,dy
int			area_width;
volatile 	int	flag_term = 0;	
volatile 	int	flag_redisp = 0;
int			flag_noparent;		// 
gint		to_tag;				// timeout check tag
char		*n_ptr_sub;			//
char		*disp_status;
char		sm_gap;				// Paper thickness lever 
short		chd_kind = 0;		//
short		model_id = 0;		// model id for table
short		dev_type = 0;		// Device type LP, USB
short		cil_kind = 0;		// CIL ink fill check
char		outputdata_type	=0;	// Standard-Print-Data or Utility-Command-Data (i250/i255)
char		endcheck_retry	=0;	//  (i250/i255)
char		printing_stat;		// 1: Data Out OK
char		fault_stat;			// 1: Fault
char		wait_end;    		// 1: waiting for compelete Operator Call

struct pr_t	pr_tbl[MODEL_CNT] = {
		{"BJF850", 		BACK_GREEN,	LVR_UD,		"BJF850"},
		{"BJF860", 		BACK_GREEN,	LVR_UD,		"BJF860"},
		{"BJF870", 		BACK_GREEN,	LVR_UD,		"BJF870"},
		{"BJF360", 		BACK_GREEN,	LVR_UD,		"BJF360"},
		{"BJS600", 		BACK_BLACK,	LVR_RL,		"BJS600"},
		{"BJS630", 		BACK_BLACK,	LVR_RL,		"BJS630"},
		{"BJS6300",		BACK_BLACK,	LVR_RL,		"BJS6300"},
		{"BJF900", 		BACK_BLACK,	LVR_RL,		"BJF900"},
		{"BJF9000",		BACK_BLACK,	LVR_RL,		"BJF9000"},
		{"BJS500", 		BACK_BLACK,	LVR_RL,		"BJS500"},
		{"BJS300", 		BACK_BLACK,	LVR_RL,		"BJS300"},
		{"PIXUS550I", 	BACK_BLACK,	LVR_NONE,	"PIXUS 550i"},
		{"PIXUS850I", 	BACK_BLACK,	LVR_NONE,	"PIXUS 850i"},
		{"PIXUS950I", 	BACK_BLACK,	LVR_NONE,	"PIXUS 950i"},
		{"I250", 		BACK_BLACK,	LVR_NONE,	"i250"},
		{"I255", 		BACK_BLACK,	LVR_NONE,	"i255"},
		{"PIXUS560I", 	BACK_BLACK,	LVR_NONE,	"PIXUS 560i"},
		{"PIXUS860I", 	BACK_BLACK,	LVR_NONE,	"PIXUS 860i"},
		{"PIXUS990I", 	BACK_BLACK,	LVR_NONE,	"PIXUS 990i"},
		{"PIXMAIP1000", 	BACK_BLACK,	LVR_NONE,	"PIXMA iP1000"},
		{"PIXMAIP1500", 	BACK_BLACK,	LVR_NONE,	"PIXMA iP1500"},
		{"PIXUSIP3100", 	BACK_BLACK,	LVR_NONE,	"PIXUS iP3100"},
		{"PIXUSIP4100", 	BACK_BLACK,	LVR_NONE,	"PIXUS iP4100"},
		{"PIXUSIP8600", 	BACK_BLACK,	LVR_NONE,	"PIXUS iP8600"},
};
struct pat_t 	pat_tbl[CHD_CNT2+1][CHD_AREA_MAX+1] = {
		{{I_el,  0},	// BC-50
		 {I_bk,  3},
		 {I_pc, 20},
		 {I_pm, 37},
		 {I_cy, 54},
		 {I_ma, 71},
		 {I_ye, 88},
		 {I_er,105},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0}},
		{{I_el,  0},	// BC-33
		 {I_bb,  3},
		 {I_cy, 28},
		 {I_ma, 45},
		 {I_ye, 62},
		 {I_er, 79},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0}},
		{{I_el,  0},	// BC-30
		 {I_sp,  3},
		 {I_bb, 16},
		 {I_sp, 51},
		 {I_er, 54},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0}},
		{{I_el,  0},	// BC-34
		 {I_pb,  3},
		 {I_pc, 20},
		 {I_pm, 37},
		 {I_ye, 54},
		 {I_er, 71},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0}},
		{{I_24cf, 0},	// BC-24
		 {I_24bf, 20},
		 {   0,  40},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0}},
		{{I_24c, 0},	// BC-24 no ink check
		 {I_24b, 20},
		 {   0,  40},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0}},
		{{I_el,  0},	// BC-30+BC-31
		 {I_sp,  3},
		 {I_bb, 16},
		 {I_sp, 41},
		 {I_er, 54},
		 {I_el, 63},
		 {I_cy, 66},
		 {I_ma, 83},
		 {I_ye,100},
		 {I_er,117},
		 {   0,  0}},
		{{I_el,  0},	// BC-32+BC-31
		 {I_pb,  3},
		 {I_pc, 20},
		 {I_pm, 37},
		 {I_er, 54},
		 {I_el, 63},
		 {I_cy, 66},
		 {I_ma, 83},
		 {I_ye,100},
	 	 {I_er,117}, 
		 {   0,  0}},
		{{   0, 13},	// Scanner+BC-31
		 {I_el, 63},
		 {I_cy, 66},
		 {I_ma, 83},
		 {I_ye,100},
		 {I_er,117},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0}},
		{{   0, 29},	// Scanner
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0}},
		{{I_el,  0},	// 950i
		 {I_cy,  3},
		 {I_pc, 20},
		 {I_bk, 37},
		 {I_ye, 54},
		 {I_pm, 61},
		 {I_ma, 88},
		 {I_er,105},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0}},
		{{I_el,  0},	// 860i
		 {I_pb,  3},
		 {I_bb, 20},
		 {I_ye, 45},
		 {I_ma, 62},
		 {I_cy, 79},
		 {I_er, 96},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0}},
		{{I_el,  0},	// 990i
		 {I_pm,  3},
		 {I_re, 20},
		 {I_bk, 37},
		 {I_pc, 54},
		 {I_cy, 71},
		 {I_ma, 88},
		 {I_ye,105},
		 {I_er,122},
		 {   0,  0},
		 {   0,  0}},
		{{I_el,  0},	// ip8600
		 {I_gr,  3},
		 {I_re, 20},
		 {I_pm, 37},
		 {I_bk, 54},
		 {I_pc, 71},
		 {I_cy, 88},
		 {I_ma,105},
		 {I_ye,122},
		 {I_er,139},
		 {   0,  0}},
		{{   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0},
		 {   0,  0}}
};

#ifdef DEBUG
FILE *log_path;
#endif

/*-------------------------------------------
 * 	MAIN   
 *-------------------------------------------*/
int
main (int argc, char *argv[])
{
  GtkWidget 	*window1;
  GtkWidget 	*print_continue;
  GtkWidget 	*print_cancel;
  struct 	sockaddr_in address;
  struct	hostent	*host;
  struct	sigaction	sa;

  int	 	ret,	retry;
  char		*model_s,model_name[256],model_name2[256];
  int		i,n;

#ifdef DEBUG
  create_log();
#endif

  bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
  textdomain (PACKAGE);

  /*-------------------------
     Initialize
  ---------------------------*/
  SetGtkResourceDefault();

  gtk_set_locale ();
  gtk_init (&argc, &argv);

  if( (model_s = check_model( argc,argv )) != NULL )
  	if( (model_id = check_model_s( model_s )) >= MODEL_CNT) 
		model_s = NULL;

  flag_noparent = check_noparent( argc, argv );	
 
  enable_cancel = printing_stat = fault_stat = 0;
  wait_end = sm_gap = 0;

	ink_p[I_el-1] = &g_I_el;
	ink_p[I_er-1] = &g_I_er;
  	ink_p[I_pb-1] = &g_I_pb;
  	ink_p[I_bk-1] = &g_I_bk;
  	ink_p[I_bb-1] = &g_I_bb;
  	ink_p[I_pm-1] = &g_I_pm;
  	ink_p[I_pc-1] = &g_I_pc;
  	ink_p[I_cy-1] = &g_I_cy;
  	ink_p[I_ma-1] = &g_I_ma;
  	ink_p[I_ye-1] = &g_I_ye;
  	ink_p[I_re-1] = &g_I_re;	// 990i
  	ink_p[I_gr-1] = &g_I_gr;	// ip8600
  	ink_p[I_sp-1] = &g_I_sp;
  	ink_p[I_24b-1] = &g_I_24b;
  	ink_p[I_24c-1] = &g_I_24c;
  	ink_p[I_24bf-1] = &g_I_24bf;
  	ink_p[I_24cf-1] = &g_I_24cf;
  	ink_p[I_sc-1] = &g_I_sc;

  	mask_p[I_el-1] = &mask_el;
  	mask_p[I_er-1] = &mask_er;
  	mask_p[I_pb-1] = &mask_nal;
  	mask_p[I_bk-1] = &mask_nal;
  	mask_p[I_bb-1] = &mask_wid;
  	mask_p[I_pm-1] = &mask_nal;
  	mask_p[I_pc-1] = &mask_nal;
  	mask_p[I_cy-1] = &mask_nal;
  	mask_p[I_ma-1] = &mask_nal;
  	mask_p[I_ye-1] = &mask_nal;
  	mask_p[I_re-1] = &mask_nal;		// 990i
  	mask_p[I_gr-1] = &mask_nal;		// ip8600
  	mask_p[I_sp-1] = &mask_sp;
  	mask_p[I_24b-1] = &mask_24b;
  	mask_p[I_24c-1] = &mask_24c;
  	mask_p[I_24bf-1] = &mask_24bf;
  	mask_p[I_24cf-1] = &mask_24cf;
  	mask_p[I_sc-1] = &mask_sc;

  /*-----------------------
  // Create Socket
  -------------------------*/
  sock_path = socket( AF_INET, SOCK_STREAM, 0);
  if( sock_path < 0 )	exit(1);

  address.sin_family = AF_INET;
  address.sin_port = htons(LM_COM_PORT);

  host = gethostbyname("localhost");
  if( !host )	close( sock_path );
  memcpy( &address.sin_addr, host->h_addr_list[0], sizeof(address.sin_addr));

  retry = 5;
  while( retry > 0)
  {
#ifdef DEBUG
	write_log("<SM> connect...\n");
#endif
	//Connect to LM
	ret = connect( sock_path, (struct sockaddr *)&address, sizeof(address));
	if( ret == -1 ){
		sleep(1);
//		retry--;
	}else	break;
  }
  if( retry == 0)
  {
	perror(" Can't connect!\n");
	exit(1);
  }

  /*---------------------------------
  // Printer Status Initial Setting
  ----------------------------------*/
  lmsm_s.socket_type = SMtoLM;
  lmsm_s.command     = LM_PRN_COM;
  lmsm_s.prn_data.com.com_size = SM_ComCreate( lmsm_s.prn_data.com.com_buffer,
				SM_INIT_BJF850 );
#ifdef DEBUG
	write_log("write INIT_COM\n");
#endif

  ret = write( sock_path, &lmsm_s, sizeof(lmsm_s));
  if( ret == -1 )	exit(1);

#ifdef UNUSE
  add_pixmap_directory (PACKAGE_DATA_DIR "/pixmaps");
  add_pixmap_directory (PACKAGE_SOURCE_DIR "/../pixmaps");
#endif

  sm_pixmaps_dir = g_strdup( PACKAGE_DATA_DIR "/pixmaps" );

  /*
   * The following code was added by Glade to create one of each component
   * (except popup menus), just so that you see something after building
   * the project. Delete any components that you don't want shown initially.
   */

  /*-------------------------------
     Create Window1
  -------------------------------*/

  if( (n_ptr_sub = malloc( MAX_STATBUF))==NULL) goto ret;
  if( (disp_status = malloc( MAX_STATBUF))==NULL) goto ret;
  memset( disp_status,0,MAX_STATBUF );
  
  if( model_s == NULL ){
	memset( &model_name2[0],0,256 );
	strncpy( &model_name[0], PACKAGE_PRINTER_MODEL, 255 );
	n=strlen( model_name );
	for(i=0; i<n; i++ )
		model_name2[i] = toupper( model_name[i] );
	model_s = &model_name2[0] ;
  }
  if( (model_id = check_model_s( model_s )) >= MODEL_CNT) 
	model_s = NULL;

  window1 = create_window1 ();
  
  // Set Title 
  	memset (model_name, 0, 256);
	strncpy( model_name, "Canon ", 255);
	if( model_s != NULL )
		strncat( model_name, &pr_tbl[model_id].disp_name[0], strlen(&pr_tbl[model_id].disp_name[0]) );
	else
		strncat( model_name, "BJ-Printer", strlen("BJ-Printer") );
	strncat( model_name, " Status Monitor", strlen(" Status Monitor") );
  	gtk_window_set_title( GTK_WINDOW (window1), model_name );

  // hide button
  print_continue = lookup_widget( window1, "print_continue" );
  print_cancel = lookup_widget( window1, "print_cancel"   );
  gtk_widget_set_sensitive( print_continue, FALSE );
  gtk_widget_set_sensitive( print_cancel, FALSE  );
  gtk_widget_show (window1);

/*-------------
 * Socket Check
 -------------*/
  sock_tag = gdk_input_add( sock_path,
				GDK_INPUT_READ,
				print_sts_recv,
				window1   );
  to_tag  = gtk_timeout_add( SM_SHOW_TIMEOUT_ms,
				stsmon_show_check,
				window1 );

  memset( &sa, 0, sizeof(sa));
  sa.sa_handler = sighand_term;
  if( sigaction( SIGTERM, &sa, NULL) ) perror("sigaction");
  sa.sa_handler = sighand_usr1;
  if( sigaction( SIGUSR1, &sa, NULL) ) perror("sigaction");
  
  /*-----------------
  // Printing Start
  ------------------*/
  lmsm_s.command     = LM_PRN_START;
  ret = write( sock_path, &lmsm_s, sizeof(lmsm_s));

  /*---------------------
    GTK_MAIN()
  ---------------------*/
  gtk_main ();

ret:
  if( n_ptr_sub ) free( n_ptr_sub );
  if( disp_status ) free( disp_status );
  
  gtk_timeout_remove( to_tag );
  gdk_input_remove( sock_tag );
  if( sock_path != -1 )  close( sock_path );

  if (sm_pixmaps_dir)	free (sm_pixmaps_dir);
  
  exit (0);
}


char *check_model( int	ac, char **argv )
{
  int	i;

  if( ac <= 1 ) return NULL;
  for( i=1; i<ac; i++){
	if(!(strcmp(argv[i],"--model")) )
		return argv[i+1];
  }
  return NULL;
}

int check_noparent( int	ac, char **argv )
{
  int	i;

  if( ac <= 1 ) return 0;
  for( i=1;i<ac;i++){
	if(!(strcmp(&(argv[i][0]),"--noparent")) )
		return 1;
  }
  return 0;
}

int check_model_s( char *model_s )
{
	int	i=0;

	for( i=0; i<MODEL_CNT; i++ )
	{
		if( !(strcmp( model_s, pr_tbl[i].pr_name) ))
			break;
	}
	return i;
}

void SetGtkResourceDefault()
{
	gchar* home_dir = NULL;
	gchar* rc_path = NULL;

	home_dir = g_get_home_dir();
	rc_path = g_strdup_printf("%s/.gtkrc",home_dir);
	gtk_rc_add_default_file(rc_path);
	g_free(rc_path);
}

#ifdef DEBUG
void
create_log()
{
	log_path = fopen( "/var/log/smlog.txt","a+");
//	if( log_path == 0 )	exit(1);
	
	write_log( "*** SM log *** \n");
}
void write_log( char *str )
{
	if( log_path == 0 )	return;
	fwrite( str, 1, strlen(str),log_path);
	fflush( log_path );
}
#endif
