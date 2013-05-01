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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "lmsmsock.h"
#include "smstat.h"

void	SM_Free_Pixmaps();
void	SM_Load_Pixmap(GdkPixmap **,GtkWidget *,GdkBitmap **,const gchar *);
gchar*  SM_Check_File_Exists(const gchar  *, const gchar *);
void    SM_Add_Pixmap_Directory( const gchar  *);

extern void	print_sts_read(GtkWidget *);
extern void	DOC_InkOut_display(GtkWidget *, char * );
extern void	DWS_InkLow_display(GtkWidget *, char * );
extern void	cartridge_pixmap_draw( GtkWidget * );

extern lmsm_socket	lmsm_s;
extern gint		sock_path;
extern	GdkPixmap	*pixmap;
extern	GdkPixmap	*g_out;
extern	GdkPixmap	*g_low;
extern	GdkBitmap	*mask_low;
extern	GdkBitmap	*mask_out;
extern	GdkBitmap	*mask_ls;
extern	GdkBitmap	*mask_icon;
extern	GdkPixmap	*g_out_bk;
extern	GdkPixmap	*g_low_bk;
extern	GdkBitmap	*mask_low_bk;
extern	GdkBitmap	*mask_out_bk;
extern	GdkPixmap	*g_I_el;
extern	GdkPixmap	*g_I_er;
extern	GdkPixmap	*g_I_pb;
extern	GdkPixmap	*g_I_bk;
extern	GdkPixmap	*g_I_bb;
extern	GdkPixmap	*g_I_pm;
extern	GdkPixmap	*g_I_pc;
extern	GdkPixmap	*g_I_cy;
extern	GdkPixmap	*g_I_ma;
extern	GdkPixmap	*g_I_ye;
extern	GdkPixmap	*g_I_re;	// 990i
extern	GdkPixmap	*g_I_gr;	// ip8600
extern	GdkPixmap	*g_I_sp;
extern	GdkPixmap	*g_I_24c;
extern	GdkPixmap	*g_I_24b;
extern	GdkPixmap	*g_I_24cf;
extern	GdkPixmap	*g_I_24bf;
extern	GdkPixmap	*g_I_24c1;
extern	GdkPixmap	*g_I_24c2;
extern	GdkPixmap	*g_I_24c3;
extern	GdkPixmap	*g_I_24b1;
extern	GdkPixmap	*g_I_24b2;
extern	GdkPixmap	*g_I_24b3;
extern	GdkPixmap	*g_low070;
extern	GdkPixmap	*g_low040;
extern	GdkPixmap	*g_low010;
extern	GdkPixmap	*g_I_sc;
// extern	GdkPixmap	*g_I_s4;
extern	GdkBitmap	*mask_el;
extern	GdkBitmap	*mask_er;
extern	GdkBitmap	*mask_nal;
extern	GdkBitmap	*mask_wid;
extern	GdkBitmap	*mask_sp;
extern	GdkBitmap	*mask_24c;
extern	GdkBitmap	*mask_24b;
extern	GdkBitmap	*mask_24cf;
extern	GdkBitmap	*mask_24bf;
extern	GdkBitmap	*mask_24c3;
extern	GdkBitmap	*mask_24b3;
extern	GdkBitmap	*mask_24c1;
extern	GdkBitmap	*mask_24b1;
extern	GdkBitmap	*mask_low070;
extern	GdkBitmap	*mask_low040;
extern	GdkBitmap	*mask_low010;
extern	GdkBitmap	*mask_sc;
// extern	GdkBitmap	*mask_s4;
extern	GdkPixmap	**ink_p[INK_CNT];
extern	int			gDw,gDh;
extern  volatile	int	flag_term;
extern  volatile	int	flag_redisp;
extern  char		*n_ptr_sub;	
extern  short		model_id;
extern	short		chd_kind;
extern	int			area_width;

// extern  short		back_pat[MODEL_CNT];
extern  struct pat_t	pat_tbl[CHD_CNT2+1][CHD_AREA_MAX+1];
extern  struct pr_t	pr_tbl[MODEL_CNT];
extern	gchar		*sm_pixmaps_dir;

#ifdef DEBUG
extern  void	write_log();
#endif

void
print_sts_recv( gpointer		data,
		gint			source,
		GdkInputCondition	condition)
{
  if( flag_redisp ){
	gtk_widget_show( data );
	flag_redisp = 0;
  }

  print_sts_read(data);

  if( flag_term ){
	SM_Free_Pixmaps();
  	gtk_main_quit();
  }

}

gint
stsmon_show_check( gpointer		data)
{
  if( flag_redisp ){
	gtk_widget_show( data );
	flag_redisp = 0;
  }
  return TRUE;
}

void
sighand_term()
{
#ifdef DEBUG
  write_log("SIGTERM\n");
#endif
  flag_term = 1;		// 00.10.27
}

void
sighand_usr1()
{
#ifdef DEBUG
  write_log("SIGUSR1\n");
#endif
  flag_redisp = 1;
}


void
on_window1_destroy                     (GtkObject       *object,
                                        gpointer         user_data)
{
  	gtk_main_quit();
}


gboolean
on_window1_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
#ifdef DEBUG
  write_log(" delete event\n");
#endif
  gtk_widget_hide( user_data );
  return TRUE ;
}


void
on_print_continue_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	lmsm_s.socket_type = SMtoLM;
	lmsm_s.command	   = LM_PRN_RETRY;
	write( sock_path, &lmsm_s, sizeof( lmsm_s ));
#ifdef DEBUG
  write_log("LM_PRN_RETRY write\n");
#endif
}


void
on_print_cancel_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	lmsm_s.socket_type = SMtoLM;
	lmsm_s.command	   = LM_PRN_CANCEL;
	write( sock_path, &lmsm_s, sizeof( lmsm_s ));
#ifdef DEBUG
  write_log("LM_PRN_CANCEL write\n");
#endif
}

int
getPixmapsWidth()
{
	int i = 0;
	int	ret_val = 0;
	
	if (chd_kind == 0)
		return 150;
		
	for (i=CHD_AREA_MAX; i>0; i--)
	{
		if ((ret_val = pat_tbl[chd_kind-1][i-1].p_x) != 0)	break;
	}
	return ret_val+3;	// add I_er's width
}

gboolean
on_drawingarea1_configure_event        (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
#ifdef DEBUG
  write_log(" configure_event   \n");
#endif

  SM_Free_Pixmaps();

  SM_Load_Pixmap( &g_low,widget,&mask_low,	"Ink_low.xpm"  );
  SM_Load_Pixmap( &g_out,widget,&mask_out,	"Ink_out.xpm"   );

  if( pr_tbl[model_id].back_pat == BACK_BLACK )
  {
	SM_Load_Pixmap( &g_I_el,widget,	&mask_el,"Inkg_el.xpm");
  	SM_Load_Pixmap( &g_I_er,widget,	&mask_er,"Inkg_er.xpm");
  	SM_Load_Pixmap( &g_I_pb,widget,	NULL, 	 "Inkg_pb.xpm");
  	SM_Load_Pixmap( &g_I_bk,widget,	NULL,	 "Inkg_bk.xpm");
  	SM_Load_Pixmap( &g_I_bb,widget,	&mask_wid,"Inkg_bb.xpm");
  	SM_Load_Pixmap( &g_I_pm,widget,	NULL, 	 "Inkg_pm.xpm");
  	SM_Load_Pixmap( &g_I_pc,widget,	NULL,	 "Inkg_pc.xpm");
  	SM_Load_Pixmap( &g_I_ma,widget,	NULL,	 "Inkg_ma.xpm");
  	SM_Load_Pixmap( &g_I_cy,widget,	NULL,	 "Inkg_cy.xpm");
  	SM_Load_Pixmap( &g_I_ye,widget,	&mask_nal,"Inkg_ye.xpm");
  	SM_Load_Pixmap( &g_I_sp,widget,	&mask_sp,"Inkg_sp.xpm");
  	SM_Load_Pixmap( &g_I_re,widget,	NULL,	 "Inkg_re.xpm");	// 990i
  	SM_Load_Pixmap( &g_I_gr,widget,	NULL,	 "Inkg_gr.xpm");	// ip8600
  }
  else
  {
	SM_Load_Pixmap( &g_I_el,widget,	&mask_el,"Ink_el.xpm");
  	SM_Load_Pixmap( &g_I_er,widget,	&mask_er,"Ink_er.xpm");
  	SM_Load_Pixmap( &g_I_pb,widget,	NULL,	 "Ink_pb.xpm");
  	SM_Load_Pixmap( &g_I_bk,widget,	NULL,	 "Ink_bk.xpm");
  	SM_Load_Pixmap( &g_I_bb,widget,	&mask_wid,"Ink_bb.xpm");
  	SM_Load_Pixmap( &g_I_pm,widget,	NULL,	 "Ink_pm.xpm");
  	SM_Load_Pixmap( &g_I_pc,widget,	NULL,	 "Ink_pc.xpm");
  	SM_Load_Pixmap( &g_I_ma,widget,	NULL,	 "Ink_ma.xpm");
	SM_Load_Pixmap( &g_I_cy,widget,	NULL,	 "Ink_cy.xpm");
  	SM_Load_Pixmap( &g_I_ye,widget,	&mask_nal,"Ink_ye.xpm");
  	SM_Load_Pixmap( &g_I_sp,widget,	&mask_sp,"Ink_sp.xpm");
  	SM_Load_Pixmap( &g_I_re,widget,	NULL,	 "Ink_re.xpm");		// 990i
  	SM_Load_Pixmap( &g_I_gr,widget,	NULL,	 "Ink_gr.xpm");		// ip8600
  }

  SM_Load_Pixmap( &g_low_bk,widget,&mask_low_bk,"Ink_low_bb.xpm");
  SM_Load_Pixmap( &g_out_bk,widget,&mask_out_bk,"Ink_out_bb.xpm");

  SM_Load_Pixmap( &g_low070,widget,&mask_low070,"Ink_low070.xpm");
  SM_Load_Pixmap( &g_low040,widget,&mask_low040,"Ink_low040.xpm");
  SM_Load_Pixmap( &g_low010,widget,&mask_low010,"Ink_low010.xpm");

  // Only S300
  SM_Load_Pixmap( &g_I_24b ,widget,&mask_24b,	"Ink_24b.xpm"   );
  SM_Load_Pixmap( &g_I_24c ,widget,&mask_24c,	"Ink_24c.xpm"   );
  SM_Load_Pixmap( &g_I_24bf,widget,&mask_24bf,	"Ink_24bf.xpm"   );
  SM_Load_Pixmap( &g_I_24cf,widget,&mask_24cf,	"Ink_24cf.xpm"   );
  SM_Load_Pixmap( &g_I_24b1,widget,&mask_24b1,	"Ink_24b1.xpm"   );
  SM_Load_Pixmap( &g_I_24b2,widget,NULL,      	"Ink_24b2.xpm"   );
  SM_Load_Pixmap( &g_I_24b3,widget,&mask_24b3,	"Ink_24b3.xpm"   );
  SM_Load_Pixmap( &g_I_24c1,widget,&mask_24c1,	"Ink_24c1.xpm"   );
  SM_Load_Pixmap( &g_I_24c2,widget,NULL,      	"Ink_24c2.xpm"   );
  SM_Load_Pixmap( &g_I_24c3,widget,&mask_24c3,	"Ink_24c3.xpm"   );

  // Create Off-Screen Pixmap
  pixmap = gdk_pixmap_new( 
			widget->window,
			widget->allocation.width,
			widget->allocation.height,
			-1);
  // Drawing Area Clear
  gdk_draw_rectangle( 
			pixmap,
			widget->style->bg_gc[GTK_WIDGET_STATE(widget)],
			TRUE,
			0,0,
			widget->allocation.width,
			widget->allocation.height);

  // Pixmap Size Get & Display distance set
  gDw = (widget->allocation.width - getPixmapsWidth())>>1;
  gDh = (widget->allocation.height- CHD_Y_MAX)>>1;
  area_width = widget->allocation.width;

  // Cartridge Draw
  cartridge_pixmap_draw( widget );

  // Ink Out&Low Disp
  DOC_InkOut_display( widget, n_ptr_sub );
  DWS_InkLow_display( widget, n_ptr_sub );
  return TRUE;
}


gboolean
on_drawingarea1_expose_event           (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
  gdk_draw_pixmap(
		widget->window,
		widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		pixmap,
		event->area.x, event->area.y,
		event->area.x, event->area.y,
		event->area.width, event->area.height);
  return FALSE;
}

gboolean
on_window1_destroy_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  	gtk_main_quit();
  	return FALSE;
}




void SM_Load_Pixmap  (
			GdkPixmap	**gdkpixmap,
			GtkWidget       *widget,
			GdkBitmap	**mask,
                        const gchar     *filename)
{
  gchar *found_filename = NULL;

  found_filename = SM_Check_File_Exists (sm_pixmaps_dir, filename);

  *gdkpixmap = gdk_pixmap_create_from_xpm ( widget->window, &(*mask),
                                                   NULL, found_filename);
  if (gdkpixmap == NULL)
    {
      g_warning (_("Error loading pixmap file: %s"), found_filename);
      g_free (found_filename);
      return;
    }
  g_free (found_filename);
}

void SM_Free_Pixmap( GdkPixmap *gdkpixmap, GdkBitmap *mask)
{
  if( gdkpixmap ) gdk_pixmap_unref (gdkpixmap);
  if( mask      ) gdk_bitmap_unref (mask);
}




/* This is an internally used function to check if a pixmap file exists. */
gchar*
SM_Check_File_Exists                      (const gchar     *directory,
                                        const gchar     *filename)
{
  gchar *full_filename;
  struct stat s;
  gint status;

  if ( (full_filename = (gchar*) g_malloc (strlen (directory) + 1
                                     + strlen (filename) + 1)) == NULL)
	return NULL;

  memset (full_filename, 0, (strlen (directory) + 1 + strlen (filename) + 1));
  
  strncpy (full_filename, directory, strlen(directory));
  strncat (full_filename, G_DIR_SEPARATOR_S, strlen(G_DIR_SEPARATOR_S));
  strncat (full_filename, filename, strlen(filename));

  status = stat (full_filename, &s);
  if (status == 0 && S_ISREG (s.st_mode))
    return full_filename;
  g_free (full_filename);
  return NULL;
}

void SM_Free_Pixmaps(){
	SM_Free_Pixmap( pixmap, NULL );
	SM_Free_Pixmap( g_out , mask_out );
	SM_Free_Pixmap( g_low , NULL );
	SM_Free_Pixmap( g_I_sc, mask_sc );
	SM_Free_Pixmap( g_I_sp, mask_sp );
	SM_Free_Pixmap( g_I_ye, mask_nal );
	SM_Free_Pixmap( g_I_cy, NULL );
	SM_Free_Pixmap( g_I_ma, NULL );
	SM_Free_Pixmap( g_I_pc, NULL );
	SM_Free_Pixmap( g_I_pm, NULL );
	SM_Free_Pixmap( g_I_re, NULL );
	SM_Free_Pixmap( g_I_gr, NULL );		// ip8600
	SM_Free_Pixmap( g_I_bb, NULL );
	SM_Free_Pixmap( g_I_bk, NULL );
	SM_Free_Pixmap( g_I_pb, NULL );
	SM_Free_Pixmap( g_I_er, mask_er );
	SM_Free_Pixmap( g_I_el, mask_el );
	SM_Free_Pixmap( g_out_bk, mask_out_bk );
	SM_Free_Pixmap( g_low_bk, NULL );
}
