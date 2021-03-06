/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
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
create_mainWindow (void)
{
  GtkWidget *mainWindow;
  GtkWidget *vboxMain;
  GtkWidget *scrolledMain;
  GtkWidget *textMainStatus;
  GtkWidget *hseparatorMain1;
  GtkWidget *labelMainCartridgeType;
  GtkWidget *labelMainLeverPosition;
  GtkWidget *hseparatorMain2;
  GtkWidget *drawMainCartridge;
  GtkWidget *hseparatorMain3;
  GtkWidget *hbuttonboxMain;
  GtkWidget *buttonMainContinue;
  GtkWidget *buttonMainCancel;

  mainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (mainWindow), "mainWindow", mainWindow);
  gtk_window_set_title (GTK_WINDOW (mainWindow), _("title"));

  vboxMain = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vboxMain);
  gtk_object_set_data_full (GTK_OBJECT (mainWindow), "vboxMain", vboxMain,
                            (GDestroyNotify) g_object_unref);
  gtk_widget_show (vboxMain);
  gtk_container_add (GTK_CONTAINER (mainWindow), vboxMain);

  scrolledMain = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledMain);
  gtk_object_set_data_full (GTK_OBJECT (mainWindow), "scrolledMain", scrolledMain,
                            (GDestroyNotify) g_object_unref);
  gtk_widget_show (scrolledMain);
  gtk_box_pack_start (GTK_BOX (vboxMain), scrolledMain, TRUE, TRUE, 10);
  gtk_widget_set_usize (scrolledMain, 320, 230);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledMain), 10);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledMain), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  textMainStatus = gtk_text_view_new ();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textMainStatus), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textMainStatus), FALSE);

  gtk_widget_ref (textMainStatus);
  gtk_object_set_data_full (GTK_OBJECT (mainWindow), "textMainStatus", textMainStatus,
                            (GDestroyNotify) g_object_unref);
  gtk_widget_show (textMainStatus);
  gtk_container_add (GTK_CONTAINER (scrolledMain), textMainStatus);
  gtk_text_buffer_set_text (gtk_text_view_get_buffer(GTK_TEXT_VIEW (textMainStatus)), _("printer status message"), -1);

  hseparatorMain1 = gtk_hseparator_new ();
  gtk_widget_ref (hseparatorMain1);
  gtk_object_set_data_full (GTK_OBJECT (mainWindow), "hseparatorMain1", hseparatorMain1,
                            (GDestroyNotify) g_object_unref);
  gtk_widget_show (hseparatorMain1);
  gtk_box_pack_start (GTK_BOX (vboxMain), hseparatorMain1, FALSE, FALSE, 0);

  labelMainCartridgeType = gtk_label_new (_("Cartridge Type:"));
  gtk_widget_ref (labelMainCartridgeType);
  gtk_object_set_data_full (GTK_OBJECT (mainWindow), "labelMainCartridgeType", labelMainCartridgeType,
                            (GDestroyNotify) g_object_unref);
  gtk_widget_show (labelMainCartridgeType);
  gtk_box_pack_start (GTK_BOX (vboxMain), labelMainCartridgeType, FALSE, TRUE, 0);
  gtk_label_set_justify (GTK_LABEL (labelMainCartridgeType), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (labelMainCartridgeType), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (labelMainCartridgeType), 10, 3);

  labelMainLeverPosition = gtk_label_new ("");
  gtk_widget_ref (labelMainLeverPosition);
  gtk_object_set_data_full (GTK_OBJECT (mainWindow), "labelMainLeverPosition", labelMainLeverPosition,
                            (GDestroyNotify) g_object_unref);
  gtk_widget_show (labelMainLeverPosition);
  gtk_box_pack_start (GTK_BOX (vboxMain), labelMainLeverPosition, FALSE, TRUE, 0);
  gtk_label_set_justify (GTK_LABEL (labelMainLeverPosition), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (labelMainLeverPosition), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (labelMainLeverPosition), 10, 3);

  hseparatorMain2 = gtk_hseparator_new ();
  gtk_widget_ref (hseparatorMain2);
  gtk_object_set_data_full (GTK_OBJECT (mainWindow), "hseparatorMain2", hseparatorMain2,
                            (GDestroyNotify) g_object_unref);
  gtk_widget_show (hseparatorMain2);
  gtk_box_pack_start (GTK_BOX (vboxMain), hseparatorMain2, FALSE, FALSE, 0);

  drawMainCartridge = gtk_drawing_area_new ();
  gtk_widget_ref (drawMainCartridge);
  gtk_object_set_data_full (GTK_OBJECT (mainWindow), "drawMainCartridge", drawMainCartridge,
                            (GDestroyNotify) g_object_unref);
  gtk_widget_show (drawMainCartridge);
  gtk_box_pack_start (GTK_BOX (vboxMain), drawMainCartridge, FALSE, TRUE, 0);
  gtk_widget_set_usize (drawMainCartridge, 50, 90);

  hseparatorMain3 = gtk_hseparator_new ();
  gtk_widget_ref (hseparatorMain3);
  gtk_object_set_data_full (GTK_OBJECT (mainWindow), "hseparatorMain3", hseparatorMain3,
                            (GDestroyNotify) g_object_unref);
  gtk_widget_show (hseparatorMain3);
  gtk_box_pack_start (GTK_BOX (vboxMain), hseparatorMain3, FALSE, FALSE, 0);

  hbuttonboxMain = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonboxMain);
  gtk_object_set_data_full (GTK_OBJECT (mainWindow), "hbuttonboxMain", hbuttonboxMain,
                            (GDestroyNotify) g_object_unref);
  gtk_widget_show (hbuttonboxMain);
  gtk_box_pack_start (GTK_BOX (vboxMain), hbuttonboxMain, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbuttonboxMain), 5);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonboxMain), GTK_BUTTONBOX_END);
  gtk_box_set_spacing (GTK_BOX (hbuttonboxMain), 0);
  gtk_button_box_set_child_size (GTK_BUTTON_BOX (hbuttonboxMain), 85, 21);
  gtk_button_box_set_child_ipadding (GTK_BUTTON_BOX (hbuttonboxMain), 11, 0);

  buttonMainContinue = gtk_button_new_with_label (_("Continue"));
  gtk_widget_ref (buttonMainContinue);
  gtk_object_set_data_full (GTK_OBJECT (mainWindow), "buttonMainContinue", buttonMainContinue,
                            (GDestroyNotify) g_object_unref);
  gtk_widget_show (buttonMainContinue);
  gtk_container_add (GTK_CONTAINER (hbuttonboxMain), buttonMainContinue);
  gtk_widget_set_can_default(buttonMainContinue, TRUE);

  buttonMainCancel = gtk_button_new_with_label (_("Cancel Printing"));
  gtk_widget_ref (buttonMainCancel);
  gtk_object_set_data_full (GTK_OBJECT (mainWindow), "buttonMainCancel", buttonMainCancel,
                            (GDestroyNotify) g_object_unref);
  gtk_widget_show (buttonMainCancel);
  gtk_container_add (GTK_CONTAINER (hbuttonboxMain), buttonMainCancel);
  gtk_widget_set_sensitive (buttonMainCancel, FALSE);
  gtk_widget_set_can_default(buttonMainCancel, TRUE);

  gtk_signal_connect_full (GTK_OBJECT (mainWindow), "destroy",
                      G_CALLBACK (on_mainWindow_destroy),
                      NULL,
                      NULL, NULL, 0, 0);
  gtk_signal_connect_full (GTK_OBJECT (mainWindow), "delete_event",
                      G_CALLBACK (on_mainWindow_delete_event),
                      NULL,
                      NULL, NULL, 0, 0);
  gtk_signal_connect_full (GTK_OBJECT (drawMainCartridge), "configure_event",
                      G_CALLBACK (on_drawMainCartridge_configure_event),
                      NULL,
                      NULL, NULL, 0, 0);
  gtk_signal_connect_full (GTK_OBJECT (drawMainCartridge), "expose_event",
                      G_CALLBACK (on_drawMainCartridge_expose_event),
                      NULL,
                      NULL, NULL, 0, 0);
  gtk_signal_connect_full (GTK_OBJECT (buttonMainContinue), "clicked",
                      G_CALLBACK (on_buttonMainContinue_clicked),
                      NULL,
                      NULL, NULL, 0, 0);
  gtk_signal_connect_full (GTK_OBJECT (buttonMainCancel), "clicked",
                      G_CALLBACK (on_buttonMainCancel_clicked),
                      NULL,
                      NULL, NULL, 0, 0);

  return mainWindow;
}

