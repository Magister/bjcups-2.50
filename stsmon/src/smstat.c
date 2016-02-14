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
#include	<gtk/gtk.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<stdio.h>

#include	"support.h"
#include	"lmsmsock.h"
#include	"lm.h"
#include	"smstat.h"

#define		SER_S300	(model_id==MODEL_S300 || model_id==MODEL_i250  || model_id==MODEL_i255 || model_id==MODEL_ip1000 || model_id==MODEL_ip1500)
#define		SER_F900	(model_id==MODEL_F900 || model_id==MODEL_F9000 || model_id==MODEL_950i || model_id==MODEL_990i || model_id==MODEL_ip8600)

static char *SM_GetStatVal( char *,char *, char *);
static char *SM_GetStatValC( char *,char *, char *);
int  	SM_ComCreate( char *, char * );
void 	print_sts_read( GtkWidget * );
void 	print_err_check( GtkWidget *, char );
void 	DOC_InkOut_display(GtkWidget *, char * );
void 	DWS_InkLow_display(GtkWidget *, char * );
static void ink_pixmap_display(GtkWidget *, short , short  );
void 	cartridge_pixmap_draw( GtkWidget * );
static void cartridge_pixmap_clear( GtkWidget * );
static void socket_send_LM( int );

extern	GdkPixmap	*pixmap;
extern	GdkPixmap	*g_out;
extern	GdkPixmap	*g_low;
extern	GdkBitmap	*mask_out;
extern	GdkBitmap	*mask_low;
extern	GdkPixmap	*g_out_bk;
extern	GdkPixmap	*g_low_bk;
extern	GdkBitmap	*mask_out_bk;
extern	GdkBitmap	*mask_low_bk;
extern	GdkPixmap	*g_I_24c1;
extern	GdkPixmap	*g_I_24c2;
extern	GdkPixmap	*g_I_24c3;
extern	GdkPixmap	*g_I_24b1;
extern	GdkPixmap	*g_I_24b2;
extern	GdkPixmap	*g_I_24b3;
extern	GdkPixmap	*g_low070;
extern	GdkPixmap	*g_low040;
extern	GdkPixmap	*g_low010;
extern	GdkBitmap	*mask_24b3;
extern	GdkBitmap	*mask_24c3;
extern	GdkBitmap	*mask_24b1;
extern	GdkBitmap	*mask_24c1;
extern	GdkBitmap	*mask_low010;
extern	GdkBitmap	*mask_low040;
extern	GdkBitmap	*mask_low070;
extern	lmsm_socket	lmsm_s;
extern	int			sock_path;
extern	int			enable_cancel;
extern	int			printing_stat;
extern	char		wait_end;
extern	char		fault_stat;
extern	int			gDw,gDh;
extern	char		flag_noparent;
extern	char		*n_ptr_sub;	
extern	char		*disp_status;	
extern	char		sm_gap;	
extern	short		model_id;
extern	short		chd_kind;
extern	short		dev_type;
extern	short		cil_kind;
extern	char		outputdata_type;
extern	char		endcheck_retry;
extern  int			getPixmapsWidth();
extern  int			area_width;

extern  GdkPixmap	**ink_p[];
extern  GdkBitmap	**mask_p[];
extern  struct pat_t	pat_tbl[CHD_CNT2+1][CHD_AREA_MAX+1];
extern  struct pr_t	pr_tbl[MODEL_CNT];
#ifdef DEBUG
extern	void		write_log();
#endif

#define MES_SCANNER_DETECT	_("Scanner Cartridge detected.\nYou cannot print with this cartridge. Install another BJ Cartridge in your printer.\n")
#define MES_SCANNER_DETECT2	_("Scanner Cartridge detected!\nChange to appropriate cartridge.\nClick Cancel Printing to cancel operation.\n")
#define MES_UNSUPPORTED		_("Unsupported cartridge installed.\nChange to appropriate BJ cartridge.\n")
#define MES_CARTRIDGE_NONE	_("Cartridge not installed!\nInstall appropriate BJ cartridge in the printer.\n")
#define MES_CARTRIDGE_NONE2	_("Cartridge not installed!\nInstall appropriate BJ cartridge in the printer.\nor\nClick Cancel Printing if no cartridge available.\n")
#define MES_DONT_INK_WARN	_("\nThe printer is set not to display a low ink warning.\n")
#define MES_USEDTANK_FULL	_("The used ink tank is full.\nCancel printing and turn the printer off and then back on.\nIf this doesn't clear the error, see the printer manual for more details.")
#define MES_USEDTANK_FULL2	_("The waste ink absorber is full.\nContact the service center for the replacement of the waste ink absorber.")
#define MES_SERVICE_CALL	_("Service error %s.\nCancel printing and turn the printer off and then back on.\nIf this doesn't clear the error, see the printer manual for more detail.")
#define MES_CLEANING		_("Cleaning.\nPlease wait.\n")
#define MES_CARTRIDGE_REPLACE	_("Cartridge replacement is in progress.\nClose the front cover after replacement is completed.\n")
#define MES_CARTRIDGE_REPLACE2	_("The holder is at the replacement position.\nAfter replacing the print head or the ink tank, close the front cover of the printer. To replace only the ink tanks, do not remove the print head from the printer. Do not touch the locking lever.\n")
#define MES_CARTRIDGE_REPLACE3	_("The holder is at the replacement position.\nIf you need to replace the print head or the ink tank, first open the Inner Cover. After the replacement is completed, close the Inner Cover, and then close the cover of the printer.\nIf you are replacing only the ink tank, leave the print head attached. Do not touch the lock lever.\n")
#define MES_PRINTNG			_("Printing.\n")
#define MES_ANOTHER_INTF	_("Printer is in use via another interface.\nPlease wait.\n")
#define MES_PAPER_OUT		_("Paper out.\nLoad paper and press printer's RESUME button.\n")
#define MES_PAPER_OUT1		_("Paper out!\n")
#define MES_PAPER_OUT21		_("Load paper in the sheet feeder.\n")
#define MES_PAPER_OUT22		_("Load paper in the cassette.\n")
#define MES_PAPER_OUT3		_("Press printer's RESUME button.\n")
#define MES_PAPER_JAM		_("Paper jammed.\nRemove jammed paper and press printer's RESUME button.\n")
#define MES_PAPER_JAM251	_("Paper is jammed at the Paper ejection slot.\nRemove the jammed paper, and press the RESUME button. Printing resumes from the next paper.")
#define MES_PAPER_JAM03		_("Paper is jammed in the Transport unit.\nOpen the rear cover, remove the jammed paper, and then press the RESUME button. Printing resumes from the next sheet.\nIf the jammed paper cannot be removed, first remove the cassette and remove the paper from the cassette side.")
#define MES_PAPER_JAM04		_("Paper is jammed in the Duplexing Transport section.\nFirst, remove the cassette, push the printer over onto its back side, and then open the Duplexing Transport unit. Next, remove the jammed paper, and quickly return the printer to its upright position. Finally, press the RESUME button on the printer. Printing resumes from the next sheet.\nRefer to the user's guide for details.")
#define MES_OPERATOR_ERR	_("Operator error.\nPress printer's RESUME button.\nIf this doesn't clear the error, cancel printing and turn the printer off and then back on.\n")
#define MES_OPERATOR_ERR2	_("Operator error.\nThe connected printer cannot work correctly.\n")
#define MES_USEDTANK_ALMOST	_("The used ink tank is almost full.\nPress the printer's RESUME button to restart printing.")
#define MES_USEDTANK_ALMOST2	_("The waste ink absorber is almost full.\nPress the printer's RESUME button to restart printing.")
#define MES_PROTECTOR		_("Protector not installed.\nInstall a protector properly in the printer.\n")
#define MES_INKTANK_NONE	_("An ink tank is not installed properly.\nOpen the front cover of the printer, make sure the ink tank is installed properly, and close the front cover.\n")
#define MES_INKOUT			_("Ink has run out.\nReplace ink tank and close the front cover.\n")
#define MES_INKOUT2			_("Ink has run out.\nReplace ink tank and close the cover. When replacing the ink tank, make sure that the Inner Cover is open.\n\nYou can continue printing under the ink out condition by pressing the RESUME button. Replace the empty ink tank immediately after the printing. The printer may be damaged if printing is continued under the ink out condition.\n")
#define MES_COVER_OPEN		_("Front cover is open.\nUnable to print until the front cover is closed.\n")
#define MES_COVER_OPEN2		_("The printer's front cover is open.\nIf printing is in progress, the print head or ink tank cannot be changed. Close the front cover, wait for printing to finish, then open the cover again and change the print head or ink tank. If the CD-R Tray Feeder is installed, the print head or ink tank cannot be also changed. Close the front cover, remove the CD-R Tray Feeder, open the front cover again, and change the print head or ink tank.\n")
#define MES_COVER_OPEN2		_("The printer's front cover is open.\nIf printing is in progress, the print head or ink tank cannot be changed. Close the front cover, wait for printing to finish, then open the cover again and change the print head or ink tank. If the CD-R Tray Feeder is installed, the print head or ink tank cannot be also changed. Close the front cover, remove the CD-R Tray Feeder, open the front cover again, and change the print head or ink tank.\n")
#define MES_COVER_OPEN3		_("The printer's front cover is open.\nIf printing is in progress, the print head or ink tank cannot be changed. Close the front cover, wait for printing to finish, then open the cover again and change the print head or ink tank.\n")
#define MES_COVER_OPEN3		_("The printer's front cover is open.\nIf printing is in progress, the print head or ink tank cannot be changed. Close the front cover, wait for printing to finish, then open the cover again and change the print head or ink tank.\n")
#define MES_COVER_OPEN4		_("The printer's cover is open.\nIf printing is in progress, the print head or ink tank cannot be changed. Close the cover, wait for printing to finish, then open the cover again and change the print head or ink tank.\n")
#define MES_TRAY_GUIDE		_("The CD-R Tray Feeder is installed in the printer.\nAfter ensuring that the CD-R tray has been removed, remove the CD-R Tray Feeder and then press the RESUME button. Printing will then restart.\n")
#define MES_COVER_CLOSE		_("Document printing cannot be executed because the paper output tray is closed.\nOpen the paper output tray. Printing will restart.\n")
#define MES_GUIDE_OPEN		_("Printing cannot be executed because the Inner Cover is open.\nClose the Inner Cover, and then press the RESUME button on the printer. Printing will restart.")
#define MES_GUIDE_OPEN_P	_("During printing, the Inner Cover of the printer was opened.\nClose the Inner Cover, and press the RESUME button on the printer. Printing resumes from the next paper.")
#define MES_INK_LOW			_("\nThe following ink tanks are running low.\n")
#define MES_INKLEVEL_UK		_("\nThe remaining level of the following ink is unknown.\n")
#define MES_INKLEVEL_UK2	_("\nThere is a possibility that the printer cannot detect the remaining ink level properly. Reset the ink counter using the Maintenance tab of the printer driver whenever you have replaced any ink tank with new one.\n")
#define MES_USBDEVICE		_("A digital camera or device that is not supported by the direct printing function is connected to the Direct Print Port. Disconnect the cable from the Direct Print Port, and press the RESUME button.\n")
#define MES_HEADALIGNMENT	_("An error occurred during automatic print head alignment.\nPress the RESUME button to clear the error. Refer to the user's guide to check the error, and reexecute print head alignment. If an error still occurs, carry out manual alignment.\n")
#define STR_PRINTING		_("Printing.\n")
#define STR_ONLINE			_("Printer is online.\n")
#define STR_NOTRESPONDING	_("Printer not responding.\n")
#define STR_CARTRIDGE_TYPE	_("Cartridge Type:")
#define STR_THICKNESS_LEVER	_("Paper Thickness Lever Position:")
#define STR_RIGHT			_("right")
#define STR_LEFT			_("left")
#define STR_UP				_("up")
#define STR_DOWN			_("down")
#define STR_PHOTO			_("Photo")
#define STR_COLOR			_("Color")
#define STR_PHOTO_COLOR		_("Photo/Color")
#define STR_BLACK_COLOR		_("Black/Color")
#define STR_BLACK			_("Black")
#define STR_SCANNER_COLOR	_("Scanner/Color")
#define STR_SCANNER			_("Scanner")
#define STR_UNSUPPORTED		_("Unsupported")
#define STR_UNKNOWN			_("Unknown")
#define STR_BJCARTRIDGE		_("appropriate BJ cartridge")	
#define STR_I_PHOTO_BLACK	_(" Photo Black\n")
#define STR_I_PHOTO_CYAN	_(" Photo Cyan\n")
#define STR_I_PHOTO_MAGENTA	_(" Photo Magenta\n")
#define STR_I_MAGENTA		_(" Magenta\n")
#define STR_I_CYAN			_(" Cyan\n")
#define STR_I_YELLOW		_(" Yellow\n")
#define STR_I_BLACK			_(" Black\n")
#define STR_I_COLOR			_(" Color\n")
#define STR_I_BLACK_3e			_(" Black [3e]\n")

#define STR_I_BLACK_6			_(" Black [6]\n")
#define STR_I_RED_6				_(" Red [6]\n")
#define STR_I_PHOTO_CYAN_6		_(" Photo Cyan [6]\n")
#define STR_I_PHOTO_MAGENTA_6	_(" Photo Magenta [6]\n")
#define STR_I_MAGENTA_6			_(" Magenta [6]\n")
#define STR_I_CYAN_6			_(" Cyan [6]\n")
#define STR_I_YELLOW_6			_(" Yellow [6]\n")

#define STR_I_BLACK_7			_(" Black [7]\n")
#define STR_I_GREEN_7			_(" Green [7]\n")
#define STR_I_RED_7				_(" Red [7]\n")
#define STR_I_PHOTO_CYAN_7		_(" Photo Cyan [7]\n")
#define STR_I_PHOTO_MAGENTA_7	_(" Photo Magenta [7]\n")
#define STR_I_MAGENTA_7			_(" Magenta [7]\n")
#define STR_I_CYAN_7			_(" Cyan [7]\n")
#define STR_I_YELLOW_7			_(" Yellow [7]\n")

#define STR_I_BLACK_24			_(" Black [24]\n")
#define STR_I_COLOR_24			_(" Color [24]\n")

/*---------------------------------
 * Print Status Read
 ----------------------------------*/
void
print_sts_read(GtkWidget *window1)
{
	GtkWidget	*text1;
	GtkWidget	*label1;
	GtkWidget	*label2;
	GtkWidget	*print_cancel;
	GtkWidget	*drawingarea1;
	GdkRectangle	update_rect;
	GtkTextBuffer *buffer;

	int			r_size;
	char		*stat;
	char		*sts_str =0;
	char		*mes_str =0;

	union{
		int	sts_v;
		char	sts_vc[4];
	}sts;
	lmsm_socket	lmsm_s2;

#ifdef DEBUG
	write_log("Status READ   \n");
#endif

	if ( (sts_str = malloc( MAX_STATBUF))== NULL )	goto ret ;
	if ( (mes_str = malloc( MAX_STATBUF))== NULL )	goto ret ;
	
	r_size=read( sock_path, &lmsm_s, sizeof(lmsm_s));

	if( r_size == -1 ) goto ret;
//	if( lmsm_s.prn_data.stat.stat_buffer[1] == 0 ) return ;
	if( lmsm_s.socket_type == LMtoSM_TYPEMSG ) {
#ifdef DEBUG
	write_log(" Utility Print Mode Command Recieve!!  \n");
#endif
		outputdata_type = (char)lmsm_s.command;
		endcheck_retry  = 1;
		wait_end = 0;
		goto ret;
	}
	if( lmsm_s.socket_type != LMtoSM ) goto ret;

#ifdef DEBUG
	if( lmsm_s.prn_data.stat.printer_fault == 1 )
		write_log(" Fault ON \n");
	else if( lmsm_s.prn_data.stat.printer_fault == 0 )
		write_log(" Fault OFF\n");
	else if( lmsm_s.prn_data.stat.printer_fault == 2 )
		write_log(" Printer Offline\n");
#endif
	text1 = lookup_widget( window1, "text1" );
	label1 = lookup_widget( window1, "label1" );
	label2 = lookup_widget( window1, "label2" );
	print_cancel = lookup_widget( window1, "print_cancel" );
	
	
	// Device type Get
	dev_type = lmsm_s.device_type;

	// Offline Check	
	if( lmsm_s.prn_data.stat.printer_fault == 2 )
	{
#ifdef DEBUG
		write_log(" Printer Offline    \n");
#endif
		fault_stat = 2;	

		// Cartridge Area Clear
		cartridge_pixmap_clear( window1 );

		gtk_editable_delete_text( GTK_EDITABLE(text1), 0,-1);
		gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),
		                                  STR_NOTRESPONDING , -1);
		gtk_label_set_text( GTK_LABEL(label1), STR_CARTRIDGE_TYPE);
		if( sm_gap ) gtk_label_set_text( GTK_LABEL(label2), STR_THICKNESS_LEVER);

		if( (printing_stat != 0) || ( wait_end == 1 ) )
		{
 			socket_send_LM( LM_PRN_CANCEL );
		}							
		update_rect.x = 0;
		update_rect.y = 0;
		update_rect.width = window1->allocation.width;
		update_rect.height = window1->allocation.height;
		drawingarea1 = lookup_widget( window1, "drawingarea1" );
		gtk_widget_draw( drawingarea1, &update_rect );
		return ;
	}
	else if( fault_stat == 2 )
	{
		lmsm_s2.prn_data.com.com_size = SM_ComCreate( lmsm_s2.prn_data.com.com_buffer,
								SM_INIT_BJF850 );
		lmsm_s2.socket_type = SMtoLM;	
		lmsm_s2.command     = LM_PRN_COM;
		write( sock_path, &lmsm_s2, sizeof(lmsm_s));
	
		socket_send_LM( LM_PRN_START );

		fault_stat = 0;
#ifdef DEBUG
		write_log(" Restart Printer write LM_PRN_COM INIT_BJF850    \n");
#endif
	}

	/*--------------
	// check to change status V.2.3
	----------------*/
//	if( !(strcmp( disp_status, &(lmsm_s.prn_data.stat.stat_buffer[2]))) ) return;
    memset ( disp_status, 0, MAX_STATBUF);
	strncpy( disp_status, &(lmsm_s.prn_data.stat.stat_buffer[2]), MAX_STATBUF-1  );

#ifdef DEBUG
	write_log(disp_status);
	write_log("\n");
#endif

	/*-------------
	// BST: Check
	-------------*/
	memset( sts_str, 0, MAX_STATBUF );
	stat = SM_GetStatVal(disp_status,"BST:",sts_str);
	if(stat == 0) return ;

	if( fault_stat == 2 )	/* For F360 Retry at Power ON */
		fault_stat = 0;

	buffer = gtk_text_buffer_new (NULL);

	// Status Area Clear
	gtk_editable_delete_text( GTK_EDITABLE(text1), 0,-1);

	// Status Message Display
	sts.sts_v = strtol( sts_str,NULL,16 );
	if( sts.sts_vc[0] & 0x80 && !(SM_GetStatVal(disp_status,"DBS:DS",sts_str)) )
	{
		gtk_text_buffer_insert_at_cursor (buffer, STR_PRINTING , -1);
		enable_cancel = 1;
		printing_stat = 2;
	}
	else
	{
		if( printing_stat != 0 )
			wait_end = 1;
		sts.sts_v = strtol( sts_str,NULL,16 );
		if( (wait_end == 1) && (!(sts.sts_vc[0] & 0x28)) 
			&& outputdata_type != LM_PRN_UTIL && endcheck_retry==0 )
				socket_send_LM( LM_PRN_END );
		
		gtk_text_buffer_insert_at_cursor (buffer, STR_ONLINE  , -1);
		if( endcheck_retry == 0 )
		{
			enable_cancel = 0;
			printing_stat = 0;
		}
		endcheck_retry=0;
	}

	/*-------------
	// LVR: Check
	-------------*/
	memset( sts_str, 0, MAX_STATBUF );
	stat = SM_GetStatVal(disp_status,"LVR:",sts_str);
	sm_gap = 0;
	if( stat && pr_tbl[model_id].lvr_pos )
	{
		if( pr_tbl[model_id].lvr_pos == LVR_RL )
		{
			if( !(strcmp( sts_str, "GAL,W" )) )
				sprintf( mes_str, "%s%s",STR_THICKNESS_LEVER, STR_RIGHT );
			else if( !(strcmp( sts_str, "GAL,N" )) )
				sprintf( mes_str, "%s%s",STR_THICKNESS_LEVER, STR_LEFT );
		}
		else
		{
			if( !(strcmp( sts_str, "GAL,W" )) )
				sprintf( mes_str, "%s%s",STR_THICKNESS_LEVER, STR_DOWN );
			else if( !(strcmp( sts_str, "GAL,N" )) )
				sprintf( mes_str, "%s%s",STR_THICKNESS_LEVER, STR_UP );
		}
		gtk_label_set_text( GTK_LABEL(label2), mes_str);
		sm_gap = 1;
	}
	else
		gtk_label_set_text( GTK_LABEL(label2), "");

	/*-------------
	// CIL: Check
	-------------*/
	memset( sts_str, 0, MAX_STATBUF );
	stat = SM_GetStatVal(disp_status,"CIL:",sts_str);
	if( stat )
	{
		if( !(strncmp( sts_str, CIL_ON,2 )) )		// ink fill check ON ?
			cil_kind = 1;
		else
			cil_kind = 0;
	}
	/*-------------
	// CHD: Check
	-------------*/
	memset( sts_str, 0, MAX_STATBUF );
	stat = SM_GetStatVal(disp_status,"CHD:",sts_str);
	if( stat )
	{
		chd_kind = 0;	

		// Cartridge Area Clear
		cartridge_pixmap_clear( window1 );

#ifdef DEBUG
		write_log(" CHD Check      \n");
#endif
		// CHD_PHOTO_BJ for F850 F900 F9000
		if( !(strncmp( sts_str, CHD_PHOTO_BJ,2 )) ||		// LS
		    !(strncmp( sts_str+3, CHD_PHOTO_BJ,2 )) ){		// For F900
			sprintf( mes_str, "%s%s",STR_CARTRIDGE_TYPE, STR_PHOTO );
			gtk_label_set_text( GTK_LABEL(label1), mes_str );

			if( model_id == MODEL_ip8600 )		// ip8600
				chd_kind = CHD_ip8600;			// ip8600
			else if( model_id == MODEL_990i )		// 990i
				chd_kind = CHD_990i;			// 990i
			else if( model_id >= MODEL_950i )					// 950i ?
				chd_kind = CHD_BC_60;
			else
				chd_kind = CHD_BC_50;
		}
		// CHD_COLOR_BJ for F360 & S600 550i 850i 560i 860i
		else if( !(strncmp( sts_str, CHD_COLOR_VC,2 )) ||	// VC
			 !(strncmp( sts_str, CHD_COLOR_CL,2 )) ){		// CL
				sprintf( mes_str, "%s%s",STR_CARTRIDGE_TYPE, STR_COLOR );
				gtk_label_set_text( GTK_LABEL(label1), mes_str );

				if( SER_S300 ) {
					if( cil_kind != 1 )
						chd_kind = CHD_BC_24N;
					else
						chd_kind = CHD_BC_24;
				}else if( model_id==MODEL_860i || model_id==MODEL_ip4100 )
					chd_kind = CHD_860i;
				else
					chd_kind = CHD_BC_33;

		}
		// CHD_PHOTOS_BJ for F360 or F660
		else if( !(strncmp( sts_str, CHD_PHOTO_VL,2 )) ){	// VL

			if( strstr( sts_str+3, CHD_COLOR_VC ) ){		// For F660
				chd_kind = CHD_BC_3231;
				sprintf( mes_str, "%s%s",STR_CARTRIDGE_TYPE, STR_PHOTO_COLOR );
			}else{
				chd_kind = CHD_BC_34;
				sprintf( mes_str, "%s%s",STR_CARTRIDGE_TYPE, STR_PHOTO );
			}
			gtk_label_set_text( GTK_LABEL(label1), mes_str );
		}
		// CHD_BLACK_BJ for F360
		else if( !(strncmp( sts_str, CHD_BLACK_BK,2 )) ){

			if( strstr( sts_str+3, CHD_COLOR_VC ) ){ 		// For F660
				chd_kind = CHD_BC_3031;
				sprintf( mes_str, "%s%s",STR_CARTRIDGE_TYPE, STR_BLACK_COLOR );
			}else{
				chd_kind = CHD_BC_30;
				sprintf( mes_str, "%s%s",STR_CARTRIDGE_TYPE, STR_BLACK );
			}
			gtk_label_set_text( GTK_LABEL(label1), mes_str );
		}
		else if( !(strncmp( sts_str, CHD_SCANNER,2 )) ){
			gtk_editable_delete_text( GTK_EDITABLE(text1), 0,-1);
			gtk_text_buffer_insert_at_cursor (buffer,
				 	MES_SCANNER_DETECT , -1);	

			if( strstr( sts_str, CHD_COLOR_VC ) ){ 			// For F660
				chd_kind = CHD_BC_SC31;
				sprintf( mes_str,"%s%s",STR_CARTRIDGE_TYPE, STR_SCANNER_COLOR  );
			}else{
				chd_kind = CHD_BC_SC;
				sprintf( mes_str,"%s%s",STR_CARTRIDGE_TYPE, STR_SCANNER  );
			}
			gtk_label_set_text( GTK_LABEL(label1), mes_str );
		}
		else if( !(strncmp( sts_str, CHD_UNSUPORT,2 )) ){		
			sprintf( mes_str,"%s%s",STR_CARTRIDGE_TYPE, STR_UNSUPPORTED  );
			gtk_label_set_text( GTK_LABEL(label1), mes_str );

			gtk_editable_delete_text( GTK_EDITABLE(text1), 0,-1);	
			gtk_text_buffer_insert_at_cursor (buffer,
			 	MES_UNSUPPORTED , -1);	// 00.12.22	
		}
		else{ 
			sprintf( mes_str,"%s%s",STR_CARTRIDGE_TYPE, STR_UNKNOWN  );
			gtk_label_set_text( GTK_LABEL(label1), mes_str );

			gtk_editable_delete_text( GTK_EDITABLE(text1), 0,-1);	
			sprintf( mes_str,MES_CARTRIDGE_NONE);		// 00.12.22
			gtk_text_buffer_insert_at_cursor (buffer,mes_str, -1);
		}
	    gDw = (area_width - getPixmapsWidth())>>1;
	
		// Drawing Area Pixmap draw
		cartridge_pixmap_draw( window1 );
	}

	/*-----------------------------------
	// Operator Call & other Error Check
	------------------------------------*/
	print_err_check( window1, sts.sts_vc[0] );

	if( SER_S300 && cil_kind != 1 ) {
		gtk_text_buffer_insert_at_cursor (buffer,
		 	MES_DONT_INK_WARN,-1);
	}

	// Printer Status String Display
	gtk_text_view_set_buffer (GTK_TEXT_VIEW(text1), buffer);
	g_object_unref (buffer);

	// Cartridge Pixmap Display
	update_rect.x = 0;
	update_rect.y = 0;
	update_rect.width = window1->allocation.width;
	update_rect.height = window1->allocation.height;
	drawingarea1 = lookup_widget( window1, "drawingarea1" );
	gtk_widget_draw( drawingarea1, &update_rect );

	// Button Sensitive  
  	if( enable_cancel == 1 && dev_type != DEVICE_TYPE_USB )
  		gtk_widget_set_sensitive (print_cancel, TRUE );
	else
  		gtk_widget_set_sensitive (print_cancel, FALSE);

	// "Continue" Socket send to LanguageMonitor 
	if( printing_stat == 2 && fault_stat == 0 ){
		socket_send_LM( LM_PRN_CONT );
	}
ret:
	if( sts_str ) free (sts_str);
	if( mes_str ) free (mes_str);
	
	return ;
}



/*-----------------------------------
 * Operator Call & other Error Check
 -----------------------------------*/
void
print_err_check( GtkWidget *window1, char bst )
{
	GtkWidget	*text1;
	GtkWidget	*print_continue;
//	GtkWidget	*drawingarea1;
	GdkRectangle	update_rect;
	char 		*sts_str = NULL;
	char 		*warn_str = NULL;
	char 		*doc_str = NULL;
	char 		*mes_str = NULL;
	char		*val_str = NULL;
	char 		*stat;
	char		*n_ptr;
	int			ink_warning;	
	int			ink_warning2;	
	int			dbs_tp = 0;
	int			i,j;
	struct	 	{
		int	val;
		char	*dws_no070;
		char	*dws_no040;
	} inkf_t[4][MAX_INK_KIND] =
		{
			{
				{0,DWS_INKLOW070_BLACK,  DWS_INKLOW040_BLACK},
			 	{0,DWS_INKLOW070_PHOTO_C,DWS_INKLOW040_PHOTO_C},
			 	{0,DWS_INKLOW070_PHOTO_M,DWS_INKLOW040_PHOTO_M},
			 	{0,DWS_INKLOW070_CYAN,   DWS_INKLOW040_CYAN},
			 	{0,DWS_INKLOW070_MAGENTA,DWS_INKLOW040_MAGENTA},
			 	{0,DWS_INKLOW070_YELLOW, DWS_INKLOW040_YELLOW},
			 	{0,0,0},
			 	{0,0,0},
			},{
				{0,DWS_INKLOW070_CYAN,   DWS_INKLOW040_CYAN},
			 	{0,DWS_INKLOW070_PHOTO_C,DWS_INKLOW040_PHOTO_C},
			 	{0,DWS_INKLOW070_BLACK,  DWS_INKLOW040_BLACK},
			 	{0,DWS_INKLOW070_YELLOW, DWS_INKLOW040_YELLOW},
			 	{0,DWS_INKLOW070_PHOTO_M,DWS_INKLOW040_PHOTO_M},
			 	{0,DWS_INKLOW070_MAGENTA,DWS_INKLOW040_MAGENTA},
			 	{0,0,0},
			 	{0,0,0},
			},{
				{0,DWS_INKLOW070_PHOTO_M,DWS_INKLOW040_PHOTO_M},	// 990i
			 	{0,DWS_INKLOW070_RED,	 DWS_INKLOW040_RED},
			 	{0,DWS_INKLOW070_BLACK,  DWS_INKLOW040_BLACK},
			 	{0,DWS_INKLOW070_PHOTO_C,DWS_INKLOW040_PHOTO_C},
			 	{0,DWS_INKLOW070_CYAN,	 DWS_INKLOW040_CYAN},
			 	{0,DWS_INKLOW070_MAGENTA,DWS_INKLOW040_MAGENTA},
			 	{0,DWS_INKLOW070_YELLOW, DWS_INKLOW040_YELLOW},
			 	{0,0,0},
			},{
			 	{0,DWS_INKLOW070_GREEN,	 DWS_INKLOW040_GREEN},		// ip8600
			 	{0,DWS_INKLOW070_RED,	 DWS_INKLOW040_RED},
				{0,DWS_INKLOW070_PHOTO_M,DWS_INKLOW040_PHOTO_M},	
			 	{0,DWS_INKLOW070_BLACK,  DWS_INKLOW040_BLACK},
			 	{0,DWS_INKLOW070_PHOTO_C,DWS_INKLOW040_PHOTO_C},
			 	{0,DWS_INKLOW070_CYAN,	 DWS_INKLOW040_CYAN},
			 	{0,DWS_INKLOW070_MAGENTA,DWS_INKLOW040_MAGENTA},
			 	{0,DWS_INKLOW070_YELLOW, DWS_INKLOW040_YELLOW},
			}
		};

	if ( (sts_str = malloc( MAX_STATBUF))== NULL )	goto pr_ret ;
	if ( (doc_str = malloc( MAX_STATBUF))== NULL )	goto pr_ret ;
	if ( (mes_str = malloc( MAX_STATBUF))== NULL )	goto pr_ret ;
	if ( (warn_str = malloc( MAX_STATBUF))== NULL )	goto pr_ret ;
	if ( (val_str = malloc( MAX_STATBUF))== NULL )	goto pr_ret ;
	
	memset( n_ptr_sub, 0, MAX_STATBUF );	

	text1 = lookup_widget( window1, "text1" );
	update_rect.x = 0;
	update_rect.y = 0;
	update_rect.width = window1->allocation.width;
	update_rect.height = window1->allocation.height;

	/*---------------------
	 * Service Call
	 ----------------------*/
	// DSC: Check
	memset( sts_str, 0, MAX_STATBUF );
	stat = SM_GetStatVal(disp_status,"DSC:",sts_str);

	if( stat !=0 && (strcmp(sts_str,STS_NO)) && (strcmp(sts_str,STS_UNKOWN)) )
	{
		gtk_editable_delete_text( GTK_EDITABLE(text1), 0,-1);
		if( !(strcmp( sts_str, DSC_USEDTANK_FULL )))
		{
			if (model_id>=MODEL_ip1000)			// v2.5
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_USEDTANK_FULL2, -1);
			else
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_USEDTANK_FULL, -1);
		}
		else {
			sprintf( mes_str, MES_SERVICE_CALL, sts_str );
			gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)), mes_str , -1);
		}
		enable_cancel = 1;	

		socket_send_LM( LM_PRN_STOP );

		goto pr_ret ;
	}

	/*----------------- 
	 * Printer Busy Detail
	 ------------------*/
	memset( sts_str, 0, MAX_STATBUF );
	stat = SM_GetStatVal(disp_status,"DBS:",sts_str);

	if( stat != 0 ){
		if( !(strcmp( sts_str, DBS_CLEANING ))){
			gtk_editable_delete_text( GTK_EDITABLE(text1), 0,-1);
			gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_CLEANING, -1);
			enable_cancel = 0;
		}
		else if( !(strcmp( sts_str, DBS_CARTRIDGE_CHANGE ))){
			gtk_editable_delete_text( GTK_EDITABLE(text1), 0,-1);
			if (model_id>=MODEL_ip3100)			// v2.5
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_CARTRIDGE_REPLACE3,-1);
			else if (model_id>=MODEL_ip1000)			// v2.5
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_CARTRIDGE_REPLACE2,-1);
			else 
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_CARTRIDGE_REPLACE,-1);
			enable_cancel = 0;	
		}
		else if( !(strcmp( sts_str, DBS_TEST_PRINT ))){
			gtk_editable_delete_text( GTK_EDITABLE(text1), 0,-1);
			gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)), STR_PRINTING , -1);
			if( flag_noparent ) printing_stat = 1;	

			enable_cancel = 0;	
			dbs_tp = 1;
		}
		else if( !(strcmp( sts_str, DBS_PRINTER_USED ))){
			gtk_editable_delete_text( GTK_EDITABLE(text1), 0,-1);
			gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_ANOTHER_INTF , -1);
			enable_cancel = 0;	
		}
		else if( !(strcmp( sts_str, DBS_ENDING ))){
//			socket_send_LM( LM_PRN_CANCEL );
		}
		if( (strcmp( sts_str, STS_NO)) && (strcmp( sts_str, STS_UNKOWN ))){
			gtk_widget_show( window1 );
		}
	}

	/*--------------------
	 * Operator Call
	 ---------------------*/
	// DOC: Check
	memset( warn_str, 0, MAX_STATBUF );
	stat = SM_GetStatVal(disp_status,"DWS:",warn_str);
	memset( doc_str, 0, MAX_STATBUF );
	stat = SM_GetStatVal(disp_status,"DOC:",doc_str);

	if( stat != 0 ){
		if( (n_ptr = (char *)index( doc_str, ',' )) == NULL ) goto pr_ret;
		n_ptr++;
		if( (n_ptr = (char *)index( n_ptr  , ',' )) == NULL ) goto pr_ret;
		n_ptr++;

	   	if((strncmp( n_ptr, STS_NO, 2 )) && (strncmp(n_ptr,STS_UNKOWN,2)) ){

			fault_stat = 1;
#ifdef DEBUG
			write_log(" Operator Call !! fault_stat = 1 set      \n");
#endif
			print_continue = lookup_widget( window1, "print_continue" );
  			gtk_widget_set_sensitive (print_continue, FALSE);

			gtk_editable_delete_text( GTK_EDITABLE(text1), 0,-1);


		   	if( strstr( n_ptr, DOC_USBDEVICE )){
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_USBDEVICE,-1);
				enable_cancel = ( printing_stat != 0 )? 1:0;
				goto pr_ret;
			}
		   	else if( !(strncmp( n_ptr, DOC_COVER_CLOSE, 4 )))		// v2.5
			{
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_COVER_CLOSE, -1);
				enable_cancel = ( printing_stat != 0 )? 1:0;
			}
		   	else if( !(strncmp( n_ptr, DOC_PAPER_OUT, 4 )))
			{
				if (model_id>=MODEL_ip3100)			// v2.5
				{
					gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_PAPER_OUT1, -1);
					gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_PAPER_OUT21, -1);
					gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_PAPER_OUT3, -1);
				}
				else
					gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_PAPER_OUT, -1);
				enable_cancel = 0;
			}
		   	else if( !(strncmp( n_ptr, DOC_PAPER_OUT03, 4 )))	// v2.5
			{
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_PAPER_OUT1, -1);
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_PAPER_OUT22, -1);
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_PAPER_OUT3, -1);
				enable_cancel = 0;
			}
			else if( !(strncmp( n_ptr, DOC_PAPER_JAM, 4 )))
			{
				if (model_id>=MODEL_ip3100)			// v2.5
					gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_PAPER_JAM251, -1);
				else
					gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_PAPER_JAM, -1);
				enable_cancel = 0;
			}
			else if( !(strncmp( n_ptr, DOC_PAPER_JAM03, 4 )))	// 2.5
			{
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_PAPER_JAM03, -1);
				enable_cancel = 0;
			}
			else if( !(strncmp( n_ptr, DOC_PAPER_JAM04, 4 )))	// 2.5
			{
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_PAPER_JAM04, -1);
				enable_cancel = 0;
			}
			else if( !(strncmp( n_ptr, DOC_CARTRIDGE_NOTHING, 4 ))
				|| !(strncmp( n_ptr, DOC_CARTRIDGE_NOTHING2, 4 ))){	// for F360 Only
				if( printing_stat != 0 
  				  && dev_type != DEVICE_TYPE_USB ){
					enable_cancel = 1;
					sprintf( mes_str,MES_CARTRIDGE_NONE2);		// 00.12.22
				}else{
					enable_cancel = 0;	
					sprintf( mes_str, MES_CARTRIDGE_NONE);		// 00.12.22
				}
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),mes_str, -1);
			}
			else if( !(strncmp(n_ptr,"1402",4)) || !(strncmp(n_ptr,"1403",4)) || !(strncmp(n_ptr,"1405",4)) ){
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_OPERATOR_ERR, -1);
				enable_cancel = ( printing_stat != 0 )? 1:0;
			}

			else if( !(strncmp( n_ptr, DOC_SCANNER_DETECT, 4 ))){
				if( dbs_tp == 0 
  				  && dev_type != DEVICE_TYPE_USB ){
					gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_SCANNER_DETECT2, -1);
					enable_cancel = 1;
				}else{
					gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_SCANNER_DETECT , -1);	
					enable_cancel = 0;	
				}
			}

			else if( !(strncmp( n_ptr, DOC_UNSUPORTED_CHD, 4 ))){	
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_UNSUPPORTED , -1);	// 00.12.22	
				enable_cancel = 0;
			}

		   	else if( !(strncmp( n_ptr, DOC_USEDTANK_ALMOST, 4 ))||!(strncmp( n_ptr, DOC_USEDTANK_ALMOST2, 4 )))
			{
				if (model_id>=MODEL_ip1000)			// v2.5
					gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_USEDTANK_ALMOST2, -1);
				else
					gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_USEDTANK_ALMOST, -1);
				enable_cancel = ( printing_stat != 0 )? 1:0;
			}

		   	else if( !(strncmp( n_ptr, DOC_PROTECTOR, 4 ))){
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_PROTECTOR,-1);
				enable_cancel = ( printing_stat != 0 )? 1:0;
			}


			else if( strstr( n_ptr, DOC_INKOUT_PRE )	// DOC:16xx
				|| strstr( n_ptr, DOC_COVER_OPEN ))
			{

				if( strstr( n_ptr, DOC_INKOUT_PRE ) )
				{
					if( chd_kind  ){
						if( strstr(n_ptr, DOC_INKTANK_NOTHING) )	// i250 i255
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_INKTANK_NONE, -1);
						else
						{
							if (model_id>=MODEL_ip3100)			// v2.5
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_INKOUT2, -1);
							else
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_INKOUT, -1);
						}
						enable_cancel = ( printing_stat != 0 )? 1:0;
					}
				}
				else
				{
					if( printing_stat != 0 )
					{	
						if (model_id>=MODEL_ip3100)			// v2.5
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_COVER_OPEN4, -1);
							
						else if (model_id>=MODEL_ip1000)			// v2.5
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_COVER_OPEN3, -1);
					
						else if ( strstr( warn_str, DWS_CDR_GUIDE_ON ) ) 
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_COVER_OPEN2, -1);
						
						else
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_COVER_OPEN, -1);
					}
					else
					{
						if ( strstr( warn_str, DWS_CDR_GUIDE_ON ) ) 
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_COVER_OPEN2, -1);
						else
						{
							if (model_id>=MODEL_ip3100)			// v2.5
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_CARTRIDGE_REPLACE3,-1);
							
							else if (model_id>=MODEL_ip1000)			// v2.5
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_CARTRIDGE_REPLACE2,-1);
							
							else 
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_CARTRIDGE_REPLACE, -1);
						}
					}
					enable_cancel = ( printing_stat != 0 )? 1:0;
				}
				if( chd_kind !=0 ){ 

					if( strstr( n_ptr, DOC_INKOUT_BLACK_S )  )
					{
						if (model_id>=MODEL_ip3100)			// v2.5
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_BLACK_7, -1);
						else
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_BLACK_6, -1);
						strcat( n_ptr_sub, DOC_INKOUT_BLACK_S",");
					}

					if( strstr( n_ptr, DOC_INKOUT_BLACK )  )
					{
						if( chd_kind == CHD_BC_34 || chd_kind == CHD_BC_3231 )
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_PHOTO_BLACK, -1);
						else
						{
							if( model_id>=MODEL_560i)
							{
								if( (model_id==MODEL_860i)||(model_id==MODEL_560i)||(model_id==MODEL_ip3100)||(model_id==MODEL_ip4100) )
									gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_BLACK_3e, -1);
								else
								{
									if (model_id>=MODEL_ip3100)			// v2.5
										gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_BLACK_7, -1);
									else
										gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_BLACK_6, -1);
								}
							}else
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_BLACK, -1);
						}
						strcat( n_ptr_sub, DOC_INKOUT_BLACK",");	
					}
					
					if( strstr( n_ptr, DOC_INKOUT_PHOTO_C )  )
					{
						if( model_id>=MODEL_560i)
						{
							if (model_id>=MODEL_ip1000)			// v2.5
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_PHOTO_CYAN_7, -1);
							else
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_PHOTO_CYAN_6, -1);
						}
						else
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_PHOTO_CYAN, -1);
						strcat( n_ptr_sub, DOC_INKOUT_PHOTO_C",");
					}
	
					if( strstr( n_ptr, DOC_INKOUT_PHOTO_M )  )
					{
						if( model_id>=MODEL_560i)
						{
							if (model_id>=MODEL_ip1000)			// v2.5
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_PHOTO_MAGENTA_7, -1);
							else
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_PHOTO_MAGENTA_6, -1);
						}
						else
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_PHOTO_MAGENTA, -1);
						strcat( n_ptr_sub, DOC_INKOUT_PHOTO_M",");
					}

					if( strstr( n_ptr, DOC_INKOUT_RED )  )
					{
						if (model_id>=MODEL_ip1000)			// v2.5
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_RED_7, -1);
						else
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_RED_6, -1);
						strcat( n_ptr_sub, DOC_INKOUT_RED",");
					}

					if( strstr( n_ptr, DOC_INKOUT_CYAN )  )
					{
						if( model_id>=MODEL_560i)
						{
							if (model_id>=MODEL_ip1000)			// v2.5
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_CYAN_7, -1);
							else
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_CYAN_6, -1);
						}
						else
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_CYAN, -1);
						strcat( n_ptr_sub, DOC_INKOUT_CYAN",");	
					}

					if( strstr( n_ptr, DOC_INKOUT_MAGENTA )  )
					{
						if( model_id>=MODEL_560i)
						{
							if (model_id>=MODEL_ip1000)			// v2.5
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_MAGENTA_7, -1);
							else
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_MAGENTA_6, -1);
						}
						else
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_MAGENTA, -1);
						strcat( n_ptr_sub, DOC_INKOUT_MAGENTA",");
					}

					if( strstr( n_ptr, DOC_INKOUT_YELLOW )  )
					{
						if( model_id>=MODEL_560i)
						{
							if (model_id>=MODEL_ip1000)			// v2.5
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_YELLOW_7 , -1);
							else
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_YELLOW_6 , -1);
						}
						else
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_YELLOW , -1);
						strcat( n_ptr_sub, DOC_INKOUT_YELLOW",");
					}

					if( strstr( n_ptr, DOC_INKOUT_GREEN )  )
					{
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_GREEN_7, -1);
						strcat( n_ptr_sub, DOC_INKOUT_GREEN",");
					}

					DOC_InkOut_display( window1, n_ptr_sub );
				}	
		   	}//"16"

		   	else if( !(strncmp( n_ptr, DOC_CDR_GUIDE_ON, 4 )) || !(strncmp( n_ptr, DOC_CDR_GUIDE_OPEN, 4 )))	// v2.5
			{
				if (model_id>=MODEL_ip3100)			// v2.5
					gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_GUIDE_OPEN,-1);
				else
					gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_TRAY_GUIDE,-1);
				enable_cancel = ( printing_stat != 0 )? 1:0;
			}
		   	else if( !(strncmp( n_ptr, DOC_CDR_GUIDE_ON_P, 4 )) || !(strncmp( n_ptr, DOC_CDR_GUIDE_OPEN_P, 4 )))	// v2.5
			{
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_GUIDE_OPEN_P,-1);
				enable_cancel = ( printing_stat != 0 )? 1:0;
			}
		   	else if( !(strncmp( n_ptr, DOC_HEADALIGNMENT, 4 ))){
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_HEADALIGNMENT,-1);
				enable_cancel = ( printing_stat != 0 )? 1:0;
			}

			else {		
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_OPERATOR_ERR2 , -1);		
				enable_cancel = ( printing_stat != 0 )? 1:0;
			}	

			socket_send_LM( LM_PRN_STOP );

		}else{
			if( fault_stat == 1 ){
				fault_stat = 0;
				print_continue = lookup_widget( window1, "print_continue" );
  				gtk_widget_set_sensitive (print_continue, FALSE);
#ifdef DEBUG
		write_log(" fault_stat = 0 clear    \n");
#endif
			}
		}
	}

	/*-------------------
	 * CIR ( Ink Fill Detail)
	 -----------------------*/
	// CIR: Check
	memset( sts_str, 0, MAX_STATBUF );
	stat = SM_GetStatVal(disp_status,"CIR:",sts_str);

	j=0;
	if( chd_kind !=0 && stat!=NULL  && SER_F900 ){
		if (model_id==MODEL_950i)	j=1;
		if (model_id==MODEL_990i)	j=2;
		if (model_id==MODEL_ip8600)	j=3;
		memset( val_str, 0, MAX_STATBUF );

		for( i=0; i<MAX_INK_KIND; i++ ){
			stat = SM_GetStatValC( stat,"=",val_str);
			inkf_t[j][i].val =  atoi( val_str );
			if( inkf_t[j][i].val == 70 )	strcat( n_ptr_sub, inkf_t[j][i].dws_no070 );
			if( inkf_t[j][i].val == 40 )	strcat( n_ptr_sub, inkf_t[j][i].dws_no040 );
		}

		for( i=0; i<MAX_INK_KIND; i++ ){
			if( inkf_t[j][i].val ){
				DWS_InkLow_display( window1, n_ptr_sub );
				break;
			}
		}
	}

	/*------------------
	 * Warning
	 -------------------*/
	// DWS: Check
	ink_warning = ink_warning2 = 0;
	memset( doc_str, 0, MAX_STATBUF );
	stat = SM_GetStatVal(disp_status,"DOC:",doc_str);
	memset( warn_str, 0, MAX_STATBUF );
	stat = SM_GetStatVal(disp_status,"DWS:",warn_str);

	if( chd_kind !=0 && stat != 0 && (strncmp( warn_str, STS_NO, 2 ))&& (strncmp(warn_str,STS_UNKOWN,2)) ){
		if( !(strncmp( warn_str, DWS_INKLOW_PRE, 2 ))){

			if( (strstr( warn_str, DWS_INKLOW_24COLOR2 ))){		// S300 i250 i255
					if( ink_warning == 0 ){	
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_INK_LOW, -1);
						ink_warning = 1;		
					}	
					if (model_id >= MODEL_ip1000)
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_COLOR_24, -1);
					else								
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_COLOR, -1);
					strcat( n_ptr_sub, DWS_INKLOW_24COLOR2",");
			}
			// 860i 
			if( (strstr( warn_str, DWS_INKLOW_BLACK_S )))
			{
				if(!(strstr( doc_str, DOC_INKOUT_BLACK_S)))
				{	
					if( ink_warning == 0 )
					{	
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_INK_LOW, -1);
						ink_warning = 1;		
					}									
					if (model_id>=MODEL_ip3100)			// v2.5
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_BLACK_7, -1);
					else
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_BLACK_6, -1);
					strcat( n_ptr_sub, DWS_INKLOW_BLACK_S",");
				}		
			}
			if( (strstr( warn_str, DWS_INKLOW_BLACK )))
			{
				if(!(strstr( doc_str, DOC_INKOUT_BLACK)))
				{
					if( ink_warning == 0 )
					{	
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_INK_LOW, -1);
						ink_warning = 1;		
					}									
					if( chd_kind==CHD_BC_34 || chd_kind==CHD_BC_3231 )	
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_PHOTO_BLACK, -1);
					else
					{
						if( model_id>=MODEL_560i)
						{
							if( (model_id==MODEL_860i)||(model_id==MODEL_560i)||(model_id==MODEL_ip3100)||(model_id==MODEL_ip4100) )
								gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_BLACK_3e, -1);
							else
							{
								if (model_id>=MODEL_ip3100)			// v2.5
									gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_BLACK_7, -1);
								else if (model_id>=MODEL_ip1000)			// v2.5
									gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_BLACK_24, -1);
								else
									gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_BLACK_6, -1);
							}
						}
						else
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_BLACK, -1);
					}
					strcat( n_ptr_sub, DWS_INKLOW_BLACK",");
				}	
			}
			if( (strstr( warn_str, DWS_INKLOW_PHOTO_C )))
			{
				if(!(strstr( doc_str, DOC_INKOUT_PHOTO_C)))
				{	
					if( ink_warning == 0 )
					{	
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_INK_LOW, -1);
						ink_warning = 1;		
					}
					if( model_id>=MODEL_560i)
					{									
						if (model_id>=MODEL_ip1000)			// v2.5
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_PHOTO_CYAN_7, -1);
						else
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_PHOTO_CYAN_6, -1);
					}
					else
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_PHOTO_CYAN, -1);
					strcat( n_ptr_sub, DWS_INKLOW_PHOTO_C",");
				}		
			}
			if( (strstr( warn_str, DWS_INKLOW_PHOTO_M )))
			{
				if(!(strstr( doc_str, DOC_INKOUT_PHOTO_M)))
				{	
					if( ink_warning == 0 )
					{	
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_INK_LOW, -1);
						ink_warning = 1;		
					}									
					if( model_id>=MODEL_560i)
					{									
						if (model_id>=MODEL_ip1000)			// v2.5
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_PHOTO_MAGENTA_7 , -1);
						else
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_PHOTO_MAGENTA_6 , -1);
					}
					else
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_PHOTO_MAGENTA , -1);
					strcat( n_ptr_sub, DWS_INKLOW_PHOTO_M",");	
				}		
			}
			if( (strstr( warn_str, DWS_INKLOW_GREEN )))										// ip8600
			{
				if(!(strstr( doc_str, DOC_INKOUT_GREEN)))									// ip8600	
				{
					if( ink_warning == 0 )													// ip8600
					{
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_INK_LOW, -1);	// ip8600
						ink_warning = 1;													// ip8600
					}									
					gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_GREEN_7 , -1);	// ip8600
					strcat( n_ptr_sub, DWS_INKLOW_GREEN",");								// ip8600
				}	
			}
			if( (strstr( warn_str, DWS_INKLOW_RED )))										// 990i
			{
				if(!(strstr( doc_str, DOC_INKOUT_RED)))									// 990i	
				{
					if( ink_warning == 0 )													// 990i
					{
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_INK_LOW, -1);	// 990i
						ink_warning = 1;													// 990i
					}									
					if (model_id>=MODEL_ip1000)			// v2.5
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_RED_7 , -1);	// ip8600
					else
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_RED_6 , -1);		// 990i
					strcat( n_ptr_sub, DWS_INKLOW_RED",");									// 990i
				}	
			}
			if( (strstr( warn_str, DWS_INKLOW_CYAN ))){
				if(!(strstr( doc_str, DOC_INKOUT_CYAN))){
					if( ink_warning == 0 ){	
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_INK_LOW, -1);
						ink_warning = 1;		
					}									
					if( model_id>=MODEL_560i)	
					{								
						if (model_id>=MODEL_ip1000)			// v2.5
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_CYAN_7, -1);
						else
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_CYAN_6, -1);
					}
					else
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_CYAN, -1);
					strcat( n_ptr_sub, DWS_INKLOW_CYAN",");	
				}	
			}
			if( (strstr( warn_str, DWS_INKLOW_MAGENTA ))){
				if(!(strstr( doc_str, DOC_INKOUT_MAGENTA))){	
					if( ink_warning == 0 ){	
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_INK_LOW, -1);
						ink_warning = 1;		
					}									
					if( model_id>=MODEL_560i)
					{									
						if (model_id>=MODEL_ip1000)			// v2.5
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_MAGENTA_7, -1);
						else
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_MAGENTA_6, -1);
					}
					else
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_MAGENTA, -1);
					strcat( n_ptr_sub, DWS_INKLOW_MAGENTA",");
				}	
			}
			if( (strstr( warn_str, DWS_INKLOW_YELLOW ))){
				if(!(strstr( doc_str, DOC_INKOUT_YELLOW))){	
					if( ink_warning == 0 ){	
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_INK_LOW, -1);
						ink_warning = 1;		
					}									
					if( model_id>=MODEL_560i)
					{									
						if (model_id>=MODEL_ip1000)			// v2.5
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_YELLOW_7 , -1);
						else
							gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_YELLOW_6 , -1);
					}
					else
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_YELLOW , -1);
					strcat( n_ptr_sub, DWS_INKLOW_YELLOW",");
				}	
			}
			if( (strstr( warn_str, DWS_INKLOW_24COLOR3 )))		// S300 i250 i255
			{
					if( ink_warning != 3 )
					{
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_INKLEVEL_UK, -1);
						ink_warning = 3;		
					}									
					if (model_id >= MODEL_ip1000)
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_COLOR_24, -1);
					else								
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_COLOR, -1);
					strcat( n_ptr_sub, DWS_INKLOW_24COLOR3",");
			}
			if( (strstr( warn_str, DWS_INKLOW_24BLACK3 )))		// S300 i250 i255
 			{
					if( ink_warning != 3 )
					{
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_INKLEVEL_UK, -1);
						ink_warning = 3;		
					}									
					if (model_id>=MODEL_ip1000)			// v2.5
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_BLACK_24, -1);
					else
						gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),STR_I_BLACK, -1);
					strcat( n_ptr_sub, DWS_INKLOW_24BLACK3",");
			}
			if( ink_warning == 3 )
			{
				gtk_text_buffer_insert_at_cursor (gtk_text_view_get_buffer(GTK_TEXT_VIEW(text1)),MES_INKLEVEL_UK2, -1);
			}
			if( (strstr( warn_str, DWS_INKLOW_24COLOR1 )))		// S300 i250 i255
				strcat( n_ptr_sub, DWS_INKLOW_24COLOR1",");	//  for Bitmap  
			if( (strstr( warn_str, DWS_INKLOW_24BLACK1 )))		// S300 i250 i255
				strcat( n_ptr_sub, DWS_INKLOW_24BLACK1",");	//  for Bitmap

			DWS_InkLow_display( window1, n_ptr_sub );
		}
	}
pr_ret:
	if( sts_str ) free (sts_str);
	if( val_str ) free (val_str);
	if( warn_str ) free (warn_str);
	if( doc_str ) free (doc_str);
	if( mes_str ) free (mes_str);

}

/*
 * Command Create
 *  Return: Length of Command Total
 */
int SM_ComCreate( char *com_str, char *com) 
{
	struct{
		char	esc[4];
		short	tot_l;
		char	init[2];
		char 	cm_len[2];
		char	cm_str[MAX_COMBUF];
	}tmp_com2;
	union{
		short	com_s;
		char	com_c[2];
	}cml;
	
	memset( &tmp_com2, 0, sizeof(tmp_com2));
	memset ( &cml, 0, sizeof(cml));
	
	cml.com_s = strlen( com ) +2 ;

	strncpy( &(tmp_com2.esc[1]), "\x1b[K", strlen("\x1b[K"));
	tmp_com2.tot_l = cml.com_s + 2;

	strncpy( &(tmp_com2.init[1]),"\x1e", strlen("\x1e"));
	tmp_com2.cm_len[0] = cml.com_c[1];
	tmp_com2.cm_len[1] = cml.com_c[0];
	strncpy( (char *)&(tmp_com2.cm_str), com, MAX_COMBUF-1);

	memcpy( com_str, (char *)&(tmp_com2.esc[1]), cml.com_s+9);

	return (cml.com_s+9);
}

/*
 * Get Value from Status String
 *  Return: Pointer to Value
 *	    or NULL 
 */
static char
*SM_GetStatVal(	char	*sts_ptr,	
		char	*get_str,
		char	*sts_str)
{
	char *s_ptr,*t_ptr;
	int   s_len;

	if( (s_ptr = (char *)strstr( sts_ptr, get_str )) == NULL)
		return NULL; 

	s_ptr += 4;
	if( (t_ptr = (char *)index( s_ptr,';' )) == NULL )
	{
		strncpy( sts_str, s_ptr, strlen(s_ptr) );
		return s_ptr;
	}
	s_len = t_ptr-s_ptr;
	memcpy( sts_str, s_ptr, s_len );
	
	return s_ptr;
}

/*
 * Get Value from Status String with ','
 *  Return: Pointer to Value
 *	    or NULL 
 */
static char *SM_GetStatValC(	char	*sts_ptr,	
		char	*get_str,
		char	*sts_str)
{
	char *s_ptr,*t_ptr;
	int   s_len;

	if( (s_ptr = (char *)strstr( sts_ptr, get_str )) == NULL)
		return NULL; 

	s_ptr += 1;
	if( (t_ptr = (char *)index( s_ptr,',' )) == NULL )
	{
		strncpy( sts_str, s_ptr, strlen(s_ptr) );
		return s_ptr;
	}
	s_len = t_ptr-s_ptr;
	memcpy( sts_str, s_ptr, s_len );
	
	return s_ptr;
}

/*
 * Display Ink-Out pixmaps
 *
 */
void DOC_InkOut_display( GtkWidget *window1, char *s_str )
{
	if( (strstr( s_str, DOC_INKOUT_BLACK ))){
		if( chd_kind == CHD_BC_30 || chd_kind == CHD_BC_33 || 
			chd_kind == CHD_BC_3031 || chd_kind == CHD_860i )	// 860i
				ink_pixmap_display( window1, I_bb, INK_OUT );
		else if( chd_kind == CHD_BC_34 || chd_kind == CHD_BC_3231 )
				ink_pixmap_display( window1, I_pb, INK_OUT );
		else 
				ink_pixmap_display( window1, I_bk, INK_OUT );
	}

	if( (strstr( s_str, DOC_INKOUT_GREEN )))			// ip8600
		ink_pixmap_display( window1, I_gr, INK_OUT );	// ip8600

	if( (strstr( s_str, DOC_INKOUT_BLACK_S )))			// 860i
		ink_pixmap_display( window1, I_pb, INK_OUT );	// 860i

	if( (strstr( s_str, DOC_INKOUT_RED )))				// 990i
		ink_pixmap_display( window1, I_re, INK_OUT );	// 990i

	if( (strstr( s_str, DOC_INKOUT_YELLOW )))
		ink_pixmap_display( window1, I_ye, INK_OUT );

	if( (strstr( s_str, DOC_INKOUT_MAGENTA )))
		ink_pixmap_display( window1, I_ma, INK_OUT );

	if( (strstr( s_str, DOC_INKOUT_CYAN )))
		ink_pixmap_display( window1, I_cy, INK_OUT );

	if( (strstr( s_str, DOC_INKOUT_PHOTO_M )))
		ink_pixmap_display( window1, I_pm, INK_OUT );

	if( (strstr( s_str, DOC_INKOUT_PHOTO_C )))
		ink_pixmap_display( window1, I_pc, INK_OUT );
}
/*
 * Display Ink-Low pixmaps
 *
 */
void DWS_InkLow_display( GtkWidget *window1, char *s_str )
{

	if( (strstr( s_str, DWS_INKLOW_BLACK ))){
		if( chd_kind == CHD_BC_30 || chd_kind == CHD_BC_33 || 
			chd_kind == CHD_BC_3031 || chd_kind == CHD_860i )	// 860i
			ink_pixmap_display( window1, I_bb, INK_LOW );
		else if( chd_kind == CHD_BC_34 || chd_kind == CHD_BC_3231 )
			ink_pixmap_display( window1, I_pb, INK_LOW );
		else if( chd_kind == CHD_BC_24 )
			ink_pixmap_display( window1, I_24bf, INK_LOW );
		else
			ink_pixmap_display( window1, I_bk, INK_LOW );
	}

	if( (strstr( s_str, DWS_INKLOW_GREEN )))			// ip8600
		ink_pixmap_display( window1, I_gr, INK_LOW );	// ip8600

	if( (strstr( s_str, DWS_INKLOW_BLACK_S )))			// 860i
		ink_pixmap_display( window1, I_pb, INK_LOW );	// 860i

	if( (strstr( s_str, DWS_INKLOW_RED )))				// 990i
		ink_pixmap_display( window1, I_re, INK_LOW );	// 990i

	if( (strstr( s_str, DWS_INKLOW_YELLOW )))
		ink_pixmap_display( window1, I_ye, INK_LOW );

	if( (strstr( s_str, DWS_INKLOW_MAGENTA )))
		ink_pixmap_display( window1, I_ma, INK_LOW );

	if( (strstr( s_str, DWS_INKLOW_CYAN )))
		ink_pixmap_display( window1, I_cy, INK_LOW );

	if( (strstr( s_str, DWS_INKLOW_PHOTO_M )))
		ink_pixmap_display( window1, I_pm, INK_LOW );

	if( (strstr( s_str, DWS_INKLOW_PHOTO_C )))
		ink_pixmap_display( window1, I_pc, INK_LOW );

	if( (strstr( s_str, DWS_INKLOW_24BLACK1 )))		// S300 i250 i255
		ink_pixmap_display( window1, I_24bf, INK_LOW1 );

	if( (strstr( s_str, DWS_INKLOW_24BLACK3 )))		// S300 i250 i255
		ink_pixmap_display( window1, I_24bf, INK_OUT );

	if( (strstr( s_str, DWS_INKLOW_24COLOR1 )))		// S300 i250 i255
		ink_pixmap_display( window1, I_24cf, INK_LOW1 );

	if( (strstr( s_str, DWS_INKLOW_24COLOR2 )))		// S300 i250 i255
		ink_pixmap_display( window1, I_24cf, INK_LOW );

	if( (strstr( s_str, DWS_INKLOW_24COLOR3 )))		// S300 i250 i255
		ink_pixmap_display( window1, I_24cf, INK_OUT );


	if( (strstr( s_str, DWS_INKLOW070_GREEN  )))			// ip8600
		ink_pixmap_display( window1, I_gr, INK_LOW070 );	// ip8600

	if( (strstr( s_str, DWS_INKLOW070_RED  )))				// 990i
		ink_pixmap_display( window1, I_re, INK_LOW070 );	// 990i

	if( (strstr( s_str, DWS_INKLOW070_BLACK  )))
		ink_pixmap_display( window1, I_bk, INK_LOW070 );

	if( (strstr( s_str, DWS_INKLOW070_YELLOW )))
		ink_pixmap_display( window1, I_ye, INK_LOW070 );

	if( (strstr( s_str, DWS_INKLOW070_PHOTO_C )))
		ink_pixmap_display( window1, I_pc, INK_LOW070 );

	if( (strstr( s_str, DWS_INKLOW070_MAGENTA )))
		ink_pixmap_display( window1, I_ma, INK_LOW070 );

	if( (strstr( s_str, DWS_INKLOW070_CYAN )))
		ink_pixmap_display( window1, I_cy, INK_LOW070 );

	if( (strstr( s_str, DWS_INKLOW070_PHOTO_M )))
		ink_pixmap_display( window1, I_pm, INK_LOW070 );


	if( (strstr( s_str, DWS_INKLOW040_GREEN  )))			// ip8600
		ink_pixmap_display( window1, I_gr, INK_LOW040 );	// ip8600

	if( (strstr( s_str, DWS_INKLOW040_RED  )))				// 990i
		ink_pixmap_display( window1, I_re, INK_LOW040 );	// 990i

	if( (strstr( s_str, DWS_INKLOW040_BLACK  )))
		ink_pixmap_display( window1, I_bk, INK_LOW040 );

	if( (strstr( s_str, DWS_INKLOW040_YELLOW )))
		ink_pixmap_display( window1, I_ye, INK_LOW040 );

	if( (strstr( s_str, DWS_INKLOW040_PHOTO_C )))
		ink_pixmap_display( window1, I_pc, INK_LOW040 );

	if( (strstr( s_str, DWS_INKLOW040_MAGENTA )))
		ink_pixmap_display( window1, I_ma, INK_LOW040 );

	if( (strstr( s_str, DWS_INKLOW040_CYAN )))
		ink_pixmap_display( window1, I_cy, INK_LOW040 );

	if( (strstr( s_str, DWS_INKLOW040_PHOTO_M )))
		ink_pixmap_display( window1, I_pm, INK_LOW040 );
}
/*
 * 
 *
 */
static void ink_pixmap_display(GtkWidget *window1,short	pat_n,short	flag)
{
	int		i;
	int		disp_x;
	GdkPixmap 	**pix;
	GdkBitmap 	**pix_m;

//	for( i=0; i<CHD_CNT2; i++ ){
	for( i=0; i<CHD_AREA_MAX; i++ )
	{
		if( pat_tbl[chd_kind-1][i].pat_no == pat_n ) 	break;
		if( pat_tbl[chd_kind-1][i+1].pat_no == 0 )	return;
	}
	if (i==CHD_AREA_MAX)	return;
	
	disp_x = (pat_tbl[chd_kind-1][i].p_x);

	if( pat_n == I_bb )
	{
		if( flag == INK_OUT )
		{	
			pix = &g_out_bk;
			pix_m = &mask_out_bk;
		}else{
			pix_m = &mask_low_bk;
			pix = &g_low_bk;
		}
	}
	else if( pat_n == I_24bf )			// S300 i250 i255
	{
		pix_m = &mask_24b3;
		if( flag == INK_OUT )	
			pix = &g_I_24b3;
			
		else if( flag == INK_LOW1 )
		{ 	
			pix = &g_I_24b1;
			pix_m = &mask_24b1;
		}
		else 			
			pix = &g_I_24b2;
	}
	else if( pat_n == I_24cf )			// S300 i250 i255
	{
		pix_m = &mask_24c3;
		if( flag == INK_OUT )		
			pix = &g_I_24c3;
			
		else if( flag == INK_LOW1 )
		{ 	
			pix = &g_I_24c1;
			pix_m = &mask_24c1;
		}
		else 			
			pix = &g_I_24c2;
	}
	else
	{
		switch( flag )
		{
		 case INK_OUT:	
					pix = &g_out;
					pix_m = &mask_out;
					break;
		 case INK_LOW070:
					pix = &g_low070;
					pix_m = &mask_low070;
					break;
		 case INK_LOW040:
					pix = &g_low040;
					pix_m = &mask_low040;
					break;
		 default:
				if( SER_F900 )
				{
					pix = &g_low010;
					pix_m = &mask_low010;
					break;
				}
				else
				{
					pix = &g_low;
					pix_m = &mask_low;
					break;
				}
		}
	}
	/* Low/Out pixmap display */
	gdk_gc_set_clip_mask( window1->style->bg_gc[GTK_WIDGET_STATE(window1)], *pix_m);
	gdk_gc_set_clip_origin( window1->style->bg_gc[GTK_WIDGET_STATE(window1)], 
		gDw+disp_x, gDh );

	gdk_draw_pixmap( pixmap,
		window1->style->bg_gc[GTK_WIDGET_STATE(window1)],
		*pix,0,0,
		gDw+disp_x, gDh,
		window1->allocation.width,
		window1->allocation.height);

	gdk_gc_set_clip_mask( window1->style->bg_gc[GTK_WIDGET_STATE(window1)], NULL );
}
/*
 * Cartridge Pixmap Draw 
 */
void cartridge_pixmap_draw( GtkWidget *window1 )
{
	int	i;

	if( chd_kind == 0 )	return;

	for( i=0; i<CHD_AREA_MAX; i++)
	{
		if( pat_tbl[chd_kind-1][i].pat_no == 0 ) break;
		
		gdk_gc_set_clip_mask( 
			window1->style->bg_gc[GTK_WIDGET_STATE(window1)], 
			*mask_p[(pat_tbl[chd_kind-1][i].pat_no)-1] );

		gdk_gc_set_clip_origin( 
			window1->style->bg_gc[GTK_WIDGET_STATE(window1)], 
			gDw+(pat_tbl[chd_kind-1][i].p_x) ,
			gDh+CHD_Y_OFF );

		gdk_draw_pixmap( 
			pixmap ,
			window1->style->bg_gc[GTK_WIDGET_STATE(window1)],
			*ink_p[(pat_tbl[chd_kind-1][i].pat_no)-1],
			0,0,
			gDw+(pat_tbl[chd_kind-1][i].p_x) ,
			gDh+CHD_Y_OFF,
			window1->allocation.width,
			window1->allocation.height);
		
		// Mask Clear
		gdk_gc_set_clip_mask( 
			window1->style->bg_gc[GTK_WIDGET_STATE(window1)], 
			NULL);
	}
}
/*
 * Cartridge Area Clear
 */
static void cartridge_pixmap_clear( GtkWidget *window1 )
{
	gdk_draw_rectangle( 
		pixmap,
		window1->style->bg_gc[GTK_WIDGET_STATE(window1)],
		TRUE,
		0,0,
		window1->allocation.width,
		window1->allocation.height);
}

/* 
 * Socket Send to LM
 */
static void socket_send_LM( 	int	cmd )
{
	int	ret;

	lmsm_s.socket_type = SMtoLM;	
	lmsm_s.command     = cmd;
	ret = write( sock_path, &lmsm_s, sizeof(lmsm_s));
}
