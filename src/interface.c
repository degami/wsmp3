/*
 * NON MODIFICARE QUESTO FILE - generato automaticamente da Glade.
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
#include "cw_glob_var.h"

GtkWidget*
create_WsMp3_ChatWin (void)
{
  /*
  GtkWidget *WsMp3_ChatWin;
  GtkWidget *vbox1;
  GtkWidget *menubar1;
  GtkWidget *file1;
  GtkWidget *file1_menu;
  GtkAccelGroup *file1_menu_accels;
  GtkWidget *connectto1;
  GtkWidget *separatore1;
  GtkWidget *esci1;
  GtkWidget *vbox2;
  GtkWidget *hbox2;
  GtkWidget *scrolledwindow1;
  GtkWidget *text1;
  GtkWidget *vbox3;
  GtkWidget *vbox4;
  GtkWidget *list1;
  GtkWidget *hbox5;
  GtkWidget *b_sel_all;
  GtkWidget *b_unsel_all;
  GtkWidget *hbox4;
  GtkWidget *b_aggiungi_ip;
  GtkWidget *b_rimuovi_ip;
  GtkWidget *hbox1;
  GtkWidget *entry1;
  GtkWidget *btn_invia;
  GtkWidget *statusbar1;
  */

  WsMp3_ChatWin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (WsMp3_ChatWin), "WsMp3_ChatWin", WsMp3_ChatWin);
  gtk_widget_set_usize (WsMp3_ChatWin, 400, 300);
  gtk_widget_set_extension_events (WsMp3_ChatWin, GDK_EXTENSION_EVENTS_ALL);
  gtk_window_set_title (GTK_WINDOW (WsMp3_ChatWin), "WsMp3-ChatWin");
  gtk_window_set_position (GTK_WINDOW (WsMp3_ChatWin), GTK_WIN_POS_CENTER);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (WsMp3_ChatWin), vbox1);

  menubar1 = gtk_menu_bar_new ();
  gtk_widget_ref (menubar1);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "menubar1", menubar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (menubar1);
  gtk_box_pack_start (GTK_BOX (vbox1), menubar1, FALSE, FALSE, 0);

  file1 = gtk_menu_item_new_with_label ("File");
  gtk_widget_ref (file1);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "file1", file1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (file1);
  gtk_container_add (GTK_CONTAINER (menubar1), file1);

  file1_menu = gtk_menu_new ();
  gtk_widget_ref (file1_menu);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "file1_menu", file1_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (file1), file1_menu);
  file1_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (file1_menu));

  connectto1 = gtk_menu_item_new_with_label ("Connect To");
  gtk_widget_ref (connectto1);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "connectto1", connectto1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (connectto1);
  gtk_container_add (GTK_CONTAINER (file1_menu), connectto1);

  separatore1 = gtk_menu_item_new ();
  gtk_widget_ref (separatore1);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "separatore1", separatore1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separatore1);
  gtk_container_add (GTK_CONTAINER (file1_menu), separatore1);
  gtk_widget_set_sensitive (separatore1, FALSE);

  esci1 = gtk_menu_item_new_with_label ("Esci");
  gtk_widget_ref (esci1);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "esci1", esci1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (esci1);
  gtk_container_add (GTK_CONTAINER (file1_menu), esci1);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "vbox2", vbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), vbox2, TRUE, TRUE, 0);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox2);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "hbox2", hbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox2, TRUE, TRUE, 0);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow1);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "scrolledwindow1", scrolledwindow1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (hbox2), scrolledwindow1, TRUE, TRUE, 0);
  gtk_widget_set_usize (scrolledwindow1, 216, -2);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  text1 = gtk_text_new (NULL, NULL);
  gtk_widget_ref (text1);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "text1", text1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (text1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), text1);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox3);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "vbox3", vbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox3);
  gtk_box_pack_start (GTK_BOX (hbox2), vbox3, TRUE, TRUE, 0);

  vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox4);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "vbox4", vbox4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox4);
  gtk_box_pack_start (GTK_BOX (vbox3), vbox4, TRUE, TRUE, 0);

  list1 = gtk_list_new ();
  gtk_widget_ref (list1);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "list1", list1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (list1);
  gtk_box_pack_start (GTK_BOX (vbox4), list1, TRUE, TRUE, 0);
  gtk_widget_set_usize (list1, -2, 200);
  gtk_list_set_selection_mode (GTK_LIST (list1), GTK_SELECTION_MULTIPLE);

  hbox5 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox5);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "hbox5", hbox5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox5);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox5, TRUE, TRUE, 0);

  b_sel_all = gtk_button_new_with_label ("Sel. All");
  gtk_widget_ref (b_sel_all);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "b_sel_all", b_sel_all,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (b_sel_all);
  gtk_box_pack_start (GTK_BOX (hbox5), b_sel_all, TRUE, TRUE, 0);

  b_unsel_all = gtk_button_new_with_label ("Unsel. All");
  gtk_widget_ref (b_unsel_all);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "b_unsel_all", b_unsel_all,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (b_unsel_all);
  gtk_box_pack_start (GTK_BOX (hbox5), b_unsel_all, TRUE, TRUE, 0);

  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox4);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "hbox4", hbox4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox4);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox4, TRUE, TRUE, 0);

  b_aggiungi_ip = gtk_button_new_with_label ("Add IP");
  gtk_widget_ref (b_aggiungi_ip);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "b_aggiungi_ip", b_aggiungi_ip,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (b_aggiungi_ip);
  gtk_box_pack_start (GTK_BOX (hbox4), b_aggiungi_ip, TRUE, TRUE, 0);

  b_rimuovi_ip = gtk_button_new_with_label ("Rem IP");
  gtk_widget_ref (b_rimuovi_ip);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "b_rimuovi_ip", b_rimuovi_ip,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (b_rimuovi_ip);
  gtk_box_pack_start (GTK_BOX (hbox4), b_rimuovi_ip, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "hbox1", hbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox1, FALSE, TRUE, 0);
  gtk_widget_set_usize (hbox1, 206, 18);

  entry1 = gtk_entry_new ();
  gtk_widget_ref (entry1);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "entry1", entry1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry1);
  gtk_box_pack_start (GTK_BOX (hbox1), entry1, TRUE, TRUE, 0);
  gtk_widget_set_usize (entry1, 158, 18);

  btn_invia = gtk_button_new_with_label ("Invia!");
  gtk_widget_ref (btn_invia);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "btn_invia", btn_invia,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btn_invia);
  gtk_box_pack_start (GTK_BOX (hbox1), btn_invia, FALSE, FALSE, 0);

  statusbar1 = gtk_statusbar_new ();
  gtk_widget_ref (statusbar1);
  gtk_object_set_data_full (GTK_OBJECT (WsMp3_ChatWin), "statusbar1", statusbar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (statusbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), statusbar1, FALSE, FALSE, 0);

  gtk_signal_connect (GTK_OBJECT (WsMp3_ChatWin), "destroy",
                      GTK_SIGNAL_FUNC (on_WsMp3_ChatWin_destroy),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (WsMp3_ChatWin), "destroy_event",
                      GTK_SIGNAL_FUNC (on_WsMp3_ChatWin_destroy_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (file1), "activate",
                      GTK_SIGNAL_FUNC (on_file1_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (connectto1), "activate",
                      GTK_SIGNAL_FUNC (on_connectto1_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (esci1), "activate",
                      GTK_SIGNAL_FUNC (on_esci1_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (b_sel_all), "clicked",
                      GTK_SIGNAL_FUNC (on_b_sel_all_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (b_unsel_all), "clicked",
                      GTK_SIGNAL_FUNC (on_b_unsel_all_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (b_aggiungi_ip), "clicked",
                      GTK_SIGNAL_FUNC (on_b_aggiungi_ip_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (b_rimuovi_ip), "clicked",
                      GTK_SIGNAL_FUNC (on_b_rimuovi_ip_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (entry1), "changed",
                      GTK_SIGNAL_FUNC (on_entry1_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btn_invia), "clicked",
                      GTK_SIGNAL_FUNC (on_btn_invia_clicked),
                      NULL);

  return WsMp3_ChatWin;
}

GtkWidget*
create_dialog1 (void)
{
  /*
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *entry2;
  GtkWidget *dialog_action_area1;
  GtkWidget *b_diag_ok;
  */

  dialog1 = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (dialog1), "dialog1", dialog1);
  gtk_window_set_title (GTK_WINDOW (dialog1), "Add Ip");
  GTK_WINDOW (dialog1)->type = GTK_WINDOW_DIALOG;
  gtk_window_set_position (GTK_WINDOW (dialog1), GTK_WIN_POS_CENTER);
  gtk_window_set_policy (GTK_WINDOW (dialog1), TRUE, TRUE, FALSE);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_object_set_data (GTK_OBJECT (dialog1), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);
  gtk_widget_set_usize (dialog_vbox1, 200, 60);

  entry2 = gtk_entry_new ();
  gtk_widget_ref (entry2);
  gtk_object_set_data_full (GTK_OBJECT (dialog1), "entry2", entry2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry2);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), entry2, FALSE, FALSE, 0);
  gtk_widget_set_usize (entry2, -2, 23);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_object_set_data (GTK_OBJECT (dialog1), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_widget_set_usize (dialog_action_area1, -2, 34);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 3);

  b_diag_ok = gtk_button_new_with_label ("Ok");
  gtk_widget_ref (b_diag_ok);
  gtk_object_set_data_full (GTK_OBJECT (dialog1), "b_diag_ok", b_diag_ok,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (b_diag_ok);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), b_diag_ok, FALSE, TRUE, 0);

  gtk_signal_connect (GTK_OBJECT (entry2), "changed",
                      GTK_SIGNAL_FUNC (on_entry2_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (b_diag_ok), "clicked",
                      GTK_SIGNAL_FUNC (on_b_diag_ok_clicked),
                      NULL);

  return dialog1;
}

