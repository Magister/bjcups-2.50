/*
 * 編集禁止! - このファイルはGladeによって生成されています.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

GtkWidget*
create_window1 (void)
{
  GtkWidget *window1;
  GtkWidget *vbox5;
  GtkWidget *scrolledwindow6;
  GtkWidget *text1;
  GtkWidget *hseparator2;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *hseparator1;
  GtkWidget *drawingarea1;
  GtkWidget *hseparator3;
  GtkWidget *hbuttonbox1;
  GtkWidget *print_continue;
  GtkWidget *print_cancel;

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (window1), "window1", window1);
  gtk_widget_set_uposition (window1, 0, 0);
  gtk_window_set_title (GTK_WINDOW (window1), _("Canon BJF360 Status Monitor"));
  gtk_window_set_policy (GTK_WINDOW (window1), FALSE, TRUE, TRUE);

  vbox5 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox5);
  gtk_object_set_data_full (GTK_OBJECT (window1), "vbox5", vbox5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox5);
  gtk_container_add (GTK_CONTAINER (window1), vbox5);

  scrolledwindow6 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow6);
  gtk_object_set_data_full (GTK_OBJECT (window1), "scrolledwindow6", scrolledwindow6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow6);
  gtk_box_pack_start (GTK_BOX (vbox5), scrolledwindow6, TRUE, TRUE, 10);
  gtk_widget_set_usize (scrolledwindow6, 320, 230);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow6), 10);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow6), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  text1 = gtk_text_new (NULL, NULL);
  gtk_widget_ref (text1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "text1", text1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (text1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow6), text1);
  gtk_text_buffer_set_text (gtk_text_view_get_buffer(GTK_TEXT_VIEW (text1)),
                   _("Collecting printer status. "), -1);

  hseparator2 = gtk_hseparator_new ();
  gtk_widget_ref (hseparator2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "hseparator2", hseparator2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hseparator2);
  gtk_box_pack_start (GTK_BOX (vbox5), hseparator2, FALSE, FALSE, 0);

  label1 = gtk_label_new (_("Cartridge Type:"));
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox5), label1, FALSE, TRUE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label1), 10, 3);

  label2 = gtk_label_new ("");
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (vbox5), label2, FALSE, TRUE, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label2), 10, 3);

  hseparator1 = gtk_hseparator_new ();
  gtk_widget_ref (hseparator1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "hseparator1", hseparator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hseparator1);
  gtk_box_pack_start (GTK_BOX (vbox5), hseparator1, FALSE, FALSE, 0);

  drawingarea1 = gtk_drawing_area_new ();
  gtk_widget_ref (drawingarea1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "drawingarea1", drawingarea1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (drawingarea1);
  gtk_box_pack_start (GTK_BOX (vbox5), drawingarea1, FALSE, TRUE, 0);
  gtk_widget_set_usize (drawingarea1, 50, 90);

  hseparator3 = gtk_hseparator_new ();
  gtk_widget_ref (hseparator3);
  gtk_object_set_data_full (GTK_OBJECT (window1), "hseparator3", hseparator3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hseparator3);
  gtk_box_pack_start (GTK_BOX (vbox5), hseparator3, FALSE, TRUE, 0);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "hbuttonbox1", hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox5), hbuttonbox1, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbuttonbox1), 5);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (hbuttonbox1), 0);
  gtk_button_box_set_child_size (GTK_BUTTON_BOX (hbuttonbox1), 85, 21);
  gtk_button_box_set_child_ipadding (GTK_BUTTON_BOX (hbuttonbox1), 11, 0);

  print_continue = gtk_button_new_with_label (_("Continue"));
  gtk_widget_ref (print_continue);
  gtk_object_set_data_full (GTK_OBJECT (window1), "print_continue", print_continue,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), print_continue);
  GTK_WIDGET_SET_FLAGS (print_continue, GTK_CAN_DEFAULT);

  print_cancel = gtk_button_new_with_label (_("Cancel Printing"));
  gtk_widget_ref (print_cancel);
  gtk_object_set_data_full (GTK_OBJECT (window1), "print_cancel", print_cancel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (print_cancel);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), print_cancel);
  GTK_WIDGET_SET_FLAGS (print_cancel, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (window1), "destroy",
                      GTK_SIGNAL_FUNC (on_window1_destroy),
                      window1);
  gtk_signal_connect (GTK_OBJECT (window1), "delete_event",
                      GTK_SIGNAL_FUNC (on_window1_delete_event),
                      window1);
  gtk_signal_connect (GTK_OBJECT (window1), "destroy_event",
                      GTK_SIGNAL_FUNC (on_window1_destroy_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (drawingarea1), "configure_event",
                      GTK_SIGNAL_FUNC (on_drawingarea1_configure_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (drawingarea1), "expose_event",
                      GTK_SIGNAL_FUNC (on_drawingarea1_expose_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (print_continue), "clicked",
                      GTK_SIGNAL_FUNC (on_print_continue_clicked),
                      window1);
  gtk_signal_connect (GTK_OBJECT (print_cancel), "clicked",
                      GTK_SIGNAL_FUNC (on_print_cancel_clicked),
                      window1);

  return window1;
}

